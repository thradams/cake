struct _iobuf {
    void * _Placeholder;
};

struct enumerator_list {
    struct enumerator * head;
    struct enumerator * tail;
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

struct block_item_list {
    struct block_item * head;
    struct block_item * tail;
};

struct label_list {
    struct label_list_item * head;
    struct label_list_item * tail;
};

struct condition {
    struct expression * expression;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator * p_init_declarator;
    struct token * first_token;
    struct token * last_token;
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

struct labeled_statement {
    struct label * label;
    struct statement * statement;
};

struct designation {
    struct designator_list * designator_list;
    struct token * token;
};

struct attribute_argument_clause {
    struct balanced_token_sequence * p_balanced_token_sequence;
    struct token * token;
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

struct alignment_specifier {
    struct type_name * type_name;
    struct expression * constant_expression;
    struct token * token;
};

struct member_declaration_list {
    struct token * first_token;
    struct token * last_token;
    struct member_declaration * head;
    struct member_declaration * tail;
};

struct argument_expression_list {
    struct argument_expression * head;
    struct argument_expression * tail;
};

struct generic_association {
    struct type  type;
    struct type_name * p_type_name;
    struct expression * expression;
    struct token * first_token;
    struct token * last_token;
    struct generic_association * next;
};

struct defer_defer_scope {
    struct declarator * declarator;
    struct defer_statement * defer_statement;
    struct try_statement * p_try_statement;
    struct selection_statement * p_selection_statement;
    struct iteration_statement * p_iteration_statement;
    struct statement * p_statement;
    struct compound_statement * p_compound_statement;
    struct compound_statement * p_function_body;
    struct secondary_block * p_secondary_block;
    struct primary_block * p_primary_block;
    struct defer_defer_scope * last_child;
    struct defer_defer_scope * previous;
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

struct hash_map {
    struct map_entry ** table;
    int capacity;
    int size;
};

struct attribute_specifier_sequence {
    struct token * first_token;
    struct token * last_token;
    int   attributes_flags;
    struct attribute_specifier * head;
    struct attribute_specifier * tail;
};

struct static_assert_declaration {
    struct token * first_token;
    struct token * last_token;
    struct expression * constant_expression;
    struct token * string_literal_opt;
};

struct expression_statement {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct expression * expression_opt;
};

union _struct_tag_11 {
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

struct balanced_token_sequence {
    struct balanced_token * head;
    struct balanced_token * tail;
};

struct member_declarator_list {
    struct member_declarator * head;
    struct member_declarator * tail;
};

struct attribute_list {
    int   attributes_flags;
    struct attribute * head;
    struct attribute * tail;
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

struct argument_expression {
    struct expression * expression;
    struct argument_expression * next;
    unsigned char   set_unkown;
};

struct object {
    int   state;
    int   value_type;
    struct type  type;
    char * debug_name;
    union _struct_tag_11  value;
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
    struct flow_object * p_flow_object;
    struct expression * p_expression_true;
    struct expression * p_expression_false;
    struct expression * p_alias_of_expression;
    struct type  type;
};

struct secondary_block {
    struct token * first_token;
    struct token * last_token;
    struct statement * statement;
};

struct case_label_list {
    struct label * head;
    struct label * tail;
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

struct declaration_specifier {
    struct storage_class_specifier * storage_class_specifier;
    struct type_specifier_qualifier * type_specifier_qualifier;
    struct function_specifier * function_specifier;
    struct declaration_specifier * next;
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

struct storage_class_specifier {
    int   flags;
    struct token * token;
};

struct type_qualifier_list {
    int   flags;
    struct type_qualifier * head;
    struct type_qualifier * tail;
};

struct direct_declarator {
    struct token * name_opt;
    struct token * p_calling_convention;
    struct declarator * declarator;
    struct array_declarator * array_declarator;
    struct function_declarator * function_declarator;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
};

struct jump_statement {
    struct token * label;
    struct token * first_token;
    struct token * last_token;
    struct expression * expression_opt;
    int label_id;
    struct defer_list  defer_list;
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

struct try_statement {
    struct secondary_block * secondary_block;
    struct secondary_block * catch_secondary_block_opt;
    struct token * first_token;
    struct token * last_token;
    struct token * catch_token_opt;
    int catch_label_id;
};

struct initializer_list {
    struct token * first_token;
    struct initializer_list_item * head;
    struct initializer_list_item * tail;
    int size;
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

struct attribute_specifier {
    struct token * first_token;
    struct token * last_token;
    struct attribute_list * attribute_list;
    struct attribute_specifier * next;
};

struct unlabeled_statement {
    struct expression_statement * expression_statement;
    struct primary_block * primary_block;
    struct jump_statement * jump_statement;
};

struct type_name {
    struct token * first_token;
    struct token * last_token;
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct declarator * abstract_declarator;
    struct type  type;
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

struct attribute_token {
    int   attributes_flags;
    struct token * token;
};

struct defer_statement {
    struct token * first_token;
    struct token * last_token;
    struct secondary_block * secondary_block;
};

struct defer_visit_ctx {
    struct secondary_block * catch_secondary_block_opt;
    struct parser_ctx * ctx;
    struct ast  ast;
    struct defer_defer_scope * tail_block;
    int parameter_list;
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

struct attribute {
    int   attributes_flags;
    struct attribute_token * attribute_token;
    struct attribute_argument_clause * attribute_argument_clause;
    struct attribute * next;
};

union _struct_tag_0 {
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
    union _struct_tag_0  data;
};

struct member_declaration {
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct member_declarator_list * member_declarator_list_opt;
    struct static_assert_declaration * static_assert_declaration;
    struct pragma_declaration * pragma_declaration;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct member_declaration * next;
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

struct label_list_item {
    struct token * p_last_usage;
    struct token * p_defined;
    struct label_list_item * next;
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

struct atomic_type_specifier {
    struct token * token;
    struct type_name * type_name;
};

struct parameter_list {
    struct parameter_declaration * head;
    struct parameter_declaration * tail;
};

struct parameter_type_list {
    unsigned char   is_var_args;
    unsigned char   is_void;
    struct parameter_list * parameter_list;
};

struct statement {
    struct labeled_statement * labeled_statement;
    struct unlabeled_statement * unlabeled_statement;
};

struct balanced_token {
    struct token * token;
    struct balanced_token * next;
};

struct typeof_specifier {
    struct token * first_token;
    struct token * last_token;
    struct typeof_specifier_argument * typeof_specifier_argument;
    struct type  type;
};

struct member_declarator {
    struct declarator * declarator;
    struct expression * constant_expression;
    struct member_declarator * next;
};

struct braced_initializer {
    struct token * first_token;
    struct token * last_token;
    struct initializer_list * initializer_list;
};

struct array_declarator {
    struct direct_declarator * direct_declarator;
    struct expression * assignment_expression;
    struct expression * expression;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * token;
    struct token * static_token_opt;
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

struct block_item {
    struct token * first_token;
    struct declaration * declaration;
    struct unlabeled_statement * unlabeled_statement;
    struct label * label;
    struct block_item * next;
};

struct designator {
    struct expression * constant_expression_opt;
    struct token * token;
    struct designator * next;
};

struct pointer {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * calling_convention;
    struct pointer * pointer;
};

struct designator_list {
    struct designator * head;
    struct designator * tail;
};

struct typeof_specifier_argument {
    struct expression * expression;
    struct type_name * type_name;
};

struct param {
    struct type  type;
    struct param * next;
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

struct init_statement {
    struct expression_statement * p_expression_statement;
    struct simple_declaration * p_simple_declaration;
};

struct function_specifier {
    int   flags;
    struct token * token;
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


int _Avx2WmemEnabledWeakValue = 0;
void *calloc(unsigned int nmemb, unsigned int size);

static struct defer_defer_scope *defer_visit_ctx_push_tail_block(struct defer_visit_ctx * ctx)
{
    struct defer_defer_scope * p_block;

    p_block = calloc(1, 48U);
    if (p_block)
    {
        p_block->previous = ctx->tail_block;
        ctx->tail_block = p_block;
    }
    else
    {
    }
    return ctx->tail_block;
}

static struct defer_defer_scope *defer_visit_ctx_push_child(struct defer_visit_ctx * ctx)
{
    struct defer_defer_scope * child;

    if (ctx->tail_block == 0U)
    {
        return 0U;
    }
    child = calloc(1, 48U);
    if (child)
    {
        child->previous = ctx->tail_block->last_child;
        ctx->tail_block->last_child = child;
    }
    else
    {
    }
    return ctx->tail_block->last_child;
}

void free(void * ptr);

static void defer_defer_scope_delete_one(struct defer_defer_scope * p)
{
    struct defer_defer_scope * p_block;
    struct defer_defer_scope * child;

    p_block = p;
    child = p_block->last_child;
    while (child != 0U)
    {
        struct defer_defer_scope * previous;

        previous = child->previous;
        child->previous = 0U;
        defer_defer_scope_delete_one(child);
        child = previous;
    }
    ;
    free(p_block);
}

static void defer_visit_ctx_pop_tail_block(struct defer_visit_ctx * ctx)
{
    if (ctx->tail_block)
    {
        struct defer_defer_scope * previous;

        previous = ctx->tail_block->previous;
        ctx->tail_block->previous = 0U;
        defer_defer_scope_delete_one(ctx->tail_block);
        ctx->tail_block = previous;
    }
}

void defer_list_add(struct defer_list * list, struct defer_list_item * p_item);
static void defer_visit_secondary_block(struct defer_visit_ctx * ctx, struct secondary_block * p_secondary_block);

static void defer_exit_block_visit(struct defer_visit_ctx * ctx, struct defer_defer_scope * deferblock, struct token * position_token, struct defer_list * p_defer_list)
{
    /*try*/ if (1)
    {
        struct defer_defer_scope * deferchild;

        deferchild = deferblock->last_child;
        while (deferchild != 0U)
        {
            if (deferchild->defer_statement)
            {
                struct defer_list_item * p_item;

                p_item = calloc(1, 12U);
                if (p_item == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
                p_item->defer_statement = deferchild->defer_statement;
                defer_list_add(p_defer_list, p_item);
                defer_visit_secondary_block(ctx, deferchild->defer_statement->secondary_block);
            }
            else
            {
                if (deferchild->declarator)
                {
                    struct defer_list_item * p_item;

                    p_item = calloc(1, 12U);
                    if (p_item == 0U)
                    {
                        /*throw*/ goto _CKL0;
                    }
                    p_item->declarator = deferchild->declarator;
                    defer_list_add(p_defer_list, p_item);
                }
            }
            deferchild = deferchild->previous;
        }
    }
    /*catch*/ else _CKL0:
    {
    }
}

static void defer_check_all_defer_until_try(struct defer_visit_ctx * ctx, struct defer_defer_scope * deferblock, struct token * position_token, struct defer_list * p_defer_list)
{
    struct defer_defer_scope * p_defer;

    p_defer = deferblock;
    while (p_defer != 0U)
    {
        defer_exit_block_visit(ctx, p_defer, position_token, p_defer_list);
        if (p_defer->p_try_statement)
        {
            break;
        }
        p_defer = p_defer->previous;
    }
}

int __cdecl strcmp(char * _Str1, char * _Str2);
static unsigned char  defer_find_label_unlabeled_statement(struct defer_visit_ctx * ctx, struct unlabeled_statement * p_unlabeled_statement, char * label);

static unsigned char  defer_find_label_block_item_list(struct defer_visit_ctx * ctx, struct block_item_list * p_block_item, char * label)
{
    struct block_item * block_item;

    block_item = p_block_item->head;
    while (block_item)
    {
        if (block_item->label && block_item->label->p_identifier_opt && strcmp(block_item->label->p_identifier_opt->lexeme, label) == 0)
        {
            return 1;
        }
        else
        {
            if (block_item->unlabeled_statement)
            {
                if (defer_find_label_unlabeled_statement(ctx, block_item->unlabeled_statement, label))
                {
                    return 1;
                }
            }
        }
        block_item = block_item->next;
    }
    return 0;
}

static unsigned char  defer_find_label_statement(struct defer_visit_ctx * ctx, struct statement * statement, char * label);

static unsigned char  defer_find_label_unlabeled_statement(struct defer_visit_ctx * ctx, struct unlabeled_statement * p_unlabeled_statement, char * label)
{
    if (p_unlabeled_statement->primary_block)
    {
        if (p_unlabeled_statement->primary_block->compound_statement)
        {
            if (defer_find_label_block_item_list(ctx, &p_unlabeled_statement->primary_block->compound_statement->block_item_list, label))
            {
                return 1;
            }
        }
        else
        {
            if (p_unlabeled_statement->primary_block->selection_statement)
            {
                if (defer_find_label_statement(ctx, p_unlabeled_statement->primary_block->selection_statement->secondary_block->statement, label))
                {
                    return 1;
                }
                if (p_unlabeled_statement->primary_block->selection_statement->else_secondary_block_opt)
                {
                    if (defer_find_label_statement(ctx, p_unlabeled_statement->primary_block->selection_statement->else_secondary_block_opt->statement, label))
                    {
                        return 1;
                    }
                }
            }
            else
            {
                if (p_unlabeled_statement->primary_block->try_statement)
                {
                    if (defer_find_label_statement(ctx, p_unlabeled_statement->primary_block->try_statement->secondary_block->statement, label))
                    {
                        return 1;
                    }
                    if (p_unlabeled_statement->primary_block->try_statement->catch_secondary_block_opt)
                    {
                        if (defer_find_label_statement(ctx, p_unlabeled_statement->primary_block->try_statement->catch_secondary_block_opt->statement, label))
                        {
                            return 1;
                        }
                    }
                }
                else
                {
                    if (p_unlabeled_statement->primary_block->iteration_statement)
                    {
                        if (defer_find_label_statement(ctx, p_unlabeled_statement->primary_block->iteration_statement->secondary_block->statement, label))
                        {
                            return 1;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

static unsigned char  defer_find_label_statement(struct defer_visit_ctx * ctx, struct statement * statement, char * label)
{
    if (statement->labeled_statement && statement->labeled_statement->label->p_identifier_opt)
    {
        if (strcmp(statement->labeled_statement->label->p_identifier_opt->lexeme, label) == 0)
        {
            return 1;
        }
    }
    else
    {
        if (statement->unlabeled_statement)
        {
            if (defer_find_label_unlabeled_statement(ctx, statement->unlabeled_statement, label))
            {
                return 1;
            }
        }
    }
    return 0;
}

static unsigned char  defer_find_label_scope(struct defer_visit_ctx * ctx, struct defer_defer_scope * deferblock, char * label)
{
    if (deferblock->p_iteration_statement)
    {
        if (defer_find_label_statement(ctx, deferblock->p_iteration_statement->secondary_block->statement, label))
        {
            return 1;
        }
    }
    else
    {
        if (deferblock->p_selection_statement)
        {
            if (defer_find_label_statement(ctx, deferblock->p_selection_statement->secondary_block->statement, label))
            {
                return 1;
            }
            if (deferblock->p_selection_statement->else_secondary_block_opt)
            {
                if (defer_find_label_statement(ctx, deferblock->p_selection_statement->else_secondary_block_opt->statement, label))
                {
                    return 1;
                }
            }
        }
        else
        {
            if (deferblock->p_try_statement)
            {
                if (defer_find_label_statement(ctx, deferblock->p_try_statement->secondary_block->statement, label))
                {
                    return 1;
                }
                if (deferblock->p_try_statement->catch_secondary_block_opt)
                {
                    if (defer_find_label_statement(ctx, deferblock->p_try_statement->catch_secondary_block_opt->statement, label))
                    {
                        return 1;
                    }
                }
            }
            else
            {
                if (deferblock->p_statement)
                {
                    if (defer_find_label_statement(ctx, deferblock->p_statement, label))
                    {
                        return 1;
                    }
                }
                else
                {
                    if (deferblock->p_function_body)
                    {
                        if (defer_find_label_block_item_list(ctx, &deferblock->p_function_body->block_item_list, label))
                        {
                            return 1;
                        }
                    }
                    else
                    {
                        if (deferblock->p_compound_statement)
                        {
                            if (defer_find_label_block_item_list(ctx, &deferblock->p_compound_statement->block_item_list, label))
                            {
                                return 1;
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

static void defer_check_all_defer_until_label(struct defer_visit_ctx * ctx, struct defer_defer_scope * deferblock, char * label, struct token * position_token, struct defer_list * p_defer_list)
{
    struct defer_defer_scope * p_defer;

    p_defer = deferblock;
    while (p_defer != 0U)
    {
        if (!defer_find_label_scope(ctx, p_defer, label))
        {
            defer_exit_block_visit(ctx, p_defer, position_token, p_defer_list);
        }
        else
        {
            break;
        }
        p_defer = p_defer->previous;
    }
}

static void defer_check_all_defer_until_iter(struct defer_visit_ctx * ctx, struct defer_defer_scope * deferblock, struct token * position_token, struct defer_list * p_defer_list)
{
    struct defer_defer_scope * p_defer;

    p_defer = deferblock;
    while (p_defer != 0U)
    {
        defer_exit_block_visit(ctx, p_defer, position_token, p_defer_list);
        if (p_defer->p_iteration_statement)
        {
            break;
        }
        p_defer = p_defer->previous;
    }
}

static void defer_exit_iteration_or_switch_statement_visit(struct defer_visit_ctx * ctx, struct defer_defer_scope * deferblock, struct token * position_token, struct defer_list * p_defer_list)
{
    struct defer_defer_scope * p_defer;

    p_defer = deferblock;
    while (p_defer != 0U)
    {
        defer_exit_block_visit(ctx, p_defer, position_token, p_defer_list);
        if (p_defer->p_iteration_statement)
        {
            break;
        }
        if (p_defer->p_selection_statement && p_defer->p_selection_statement->first_token->type == 9034)
        {
            break;
        }
        p_defer = p_defer->previous;
    }
}

static void defer_exit_function_visit(struct defer_visit_ctx * ctx, struct defer_defer_scope * deferblock, struct token * position_token, struct defer_list * p_defer_list)
{
    struct defer_defer_scope * p_defer;

    p_defer = deferblock;
    while (p_defer != 0U)
    {
        defer_exit_block_visit(ctx, p_defer, position_token, p_defer_list);
        p_defer = p_defer->previous;
    }
}

static void defer_visit_statement(struct defer_visit_ctx * ctx, struct statement * p_statement);

static void defer_visit_secondary_block(struct defer_visit_ctx * ctx, struct secondary_block * p_secondary_block)
{
    defer_visit_statement(ctx, p_secondary_block->statement);
}

static void defer_visit_defer_statement(struct defer_visit_ctx * ctx, struct defer_statement * p_defer_statement)
{
    struct defer_defer_scope * p_defer;

    p_defer = defer_visit_ctx_push_child(ctx);
    if (p_defer)
    {
        p_defer->defer_statement = p_defer_statement;
    }
}

static void defer_visit_declarator(struct defer_visit_ctx * ctx, struct declarator * p_declarator);

static void defer_visit_init_declarator(struct defer_visit_ctx * ctx, struct init_declarator * p_init_declarator)
{
    defer_visit_declarator(ctx, p_init_declarator->p_declarator);
}

static void defer_visit_init_declarator_list(struct defer_visit_ctx * ctx, struct init_declarator_list * p_init_declarator_list);

static void defer_visit_simple_declaration(struct defer_visit_ctx * ctx, struct simple_declaration * p_simple_declaration)
{
    defer_visit_init_declarator_list(ctx, &p_simple_declaration->init_declarator_list);
}

static void defer_visit_if_statement(struct defer_visit_ctx * ctx, struct selection_statement * p_selection_statement)
{
    struct defer_defer_scope * p_defer;

    p_defer = defer_visit_ctx_push_tail_block(ctx);
    if (p_defer == 0U)
    {
        return;
    }
    p_defer->p_selection_statement = p_selection_statement;
    if (p_selection_statement->p_init_statement && p_selection_statement->p_init_statement->p_simple_declaration)
    {
        defer_visit_simple_declaration(ctx, p_selection_statement->p_init_statement->p_simple_declaration);
    }
    if (p_selection_statement->condition && p_selection_statement->condition->p_init_declarator)
    {
        defer_visit_init_declarator(ctx, p_selection_statement->condition->p_init_declarator);
    }
    ;
    defer_visit_secondary_block(ctx, p_selection_statement->secondary_block);
    if (p_selection_statement->else_secondary_block_opt)
    {
        defer_visit_secondary_block(ctx, p_selection_statement->else_secondary_block_opt);
    }
    defer_exit_block_visit(ctx, p_defer, p_selection_statement->last_token, &p_selection_statement->defer_list);
    defer_visit_ctx_pop_tail_block(ctx);
}

static void defer_visit_try_statement(struct defer_visit_ctx * ctx, struct try_statement * p_try_statement)
{
    /*try*/ if (1)
    {
        struct secondary_block * catch_secondary_block_old;
        struct defer_defer_scope * p_defer;

        catch_secondary_block_old = ctx->catch_secondary_block_opt;
        ctx->catch_secondary_block_opt = p_try_statement->catch_secondary_block_opt;
        p_defer = defer_visit_ctx_push_tail_block(ctx);
        if (p_defer == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_defer->p_try_statement = p_try_statement;
        defer_visit_secondary_block(ctx, p_try_statement->secondary_block);
        if (p_try_statement->catch_secondary_block_opt)
        {
            defer_visit_secondary_block(ctx, p_try_statement->catch_secondary_block_opt);
        }
        defer_visit_ctx_pop_tail_block(ctx);
        ctx->catch_secondary_block_opt = catch_secondary_block_old;
    }
    /*catch*/ else _CKL0:
    {
    }
}

static void defer_visit_switch_statement(struct defer_visit_ctx * ctx, struct selection_statement * p_selection_statement)
{
    struct defer_defer_scope * p_defer;

    p_defer = defer_visit_ctx_push_tail_block(ctx);
    if (p_defer == 0U)
    {
        return;
    }
    p_defer->p_selection_statement = p_selection_statement;
    defer_visit_secondary_block(ctx, p_selection_statement->secondary_block);
    defer_exit_block_visit(ctx, p_defer, p_selection_statement->secondary_block->last_token, &p_selection_statement->defer_list);
    defer_visit_ctx_pop_tail_block(ctx);
}

static void defer_visit_selection_statement(struct defer_visit_ctx * ctx, struct selection_statement * p_selection_statement)
{
    if (p_selection_statement->first_token->type == 9017)
    {
        defer_visit_if_statement(ctx, p_selection_statement);
    }
    else
    {
        if (p_selection_statement->first_token->type == 9034)
        {
            defer_visit_switch_statement(ctx, p_selection_statement);
        }
        else
        {
            ;
        }
    }
}

static void defer_visit_block_item(struct defer_visit_ctx * ctx, struct block_item * p_block_item);

static void defer_visit_block_item_list(struct defer_visit_ctx * ctx, struct block_item_list * p_block_item_list)
{
    struct block_item * p_block_item;

    p_block_item = p_block_item_list->head;
    while (p_block_item)
    {
        defer_visit_block_item(ctx, p_block_item);
        p_block_item = p_block_item->next;
    }
}

static void defer_visit_compound_statement(struct defer_visit_ctx * ctx, struct compound_statement * p_compound_statement)
{
    struct defer_defer_scope * p_defer;

    p_defer = defer_visit_ctx_push_tail_block(ctx);
    if (p_defer == 0U)
    {
        return;
    }
    p_defer->p_compound_statement = p_compound_statement;
    defer_visit_block_item_list(ctx, &p_compound_statement->block_item_list);
    defer_exit_block_visit(ctx, p_defer, p_compound_statement->last_token, &p_compound_statement->defer_list);
    defer_visit_ctx_pop_tail_block(ctx);
}

static void defer_visit_do_while_statement(struct defer_visit_ctx * ctx, struct iteration_statement * p_iteration_statement)
{
    ;
    /*try*/ if (1)
    {
        struct defer_defer_scope * p_defer;

        p_defer = defer_visit_ctx_push_tail_block(ctx);
        if (p_defer == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_defer->p_iteration_statement = p_iteration_statement;
        defer_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
        defer_exit_block_visit(ctx, p_defer, p_iteration_statement->secondary_block->last_token, &p_iteration_statement->defer_list);
        defer_visit_ctx_pop_tail_block(ctx);
    }
    /*catch*/ else _CKL0:
    {
    }
}

static void defer_visit_while_statement(struct defer_visit_ctx * ctx, struct iteration_statement * p_iteration_statement)
{
    struct defer_defer_scope * p_defer;

    ;
    if (p_iteration_statement->expression1 == 0U)
    {
        return;
    }
    p_defer = defer_visit_ctx_push_tail_block(ctx);
    if (p_defer == 0U)
    {
        return;
    }
    p_defer->p_iteration_statement = p_iteration_statement;
    defer_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
    defer_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
    defer_exit_block_visit(ctx, p_defer, p_iteration_statement->secondary_block->last_token, &p_iteration_statement->defer_list);
    defer_visit_ctx_pop_tail_block(ctx);
}

unsigned char  secondary_block_ends_with_jump(struct secondary_block * p_secondary_block);

static void defer_visit_for_statement(struct defer_visit_ctx * ctx, struct iteration_statement * p_iteration_statement)
{
    ;
    /*try*/ if (1)
    {
        struct defer_defer_scope * p_defer;
        unsigned char   b_secondary_block_ends_with_jump;

        if (p_iteration_statement->declaration && p_iteration_statement->declaration->init_declarator_list.head)
        {
            defer_visit_init_declarator_list(ctx, &p_iteration_statement->declaration->init_declarator_list);
        }
        p_defer = defer_visit_ctx_push_tail_block(ctx);
        if (p_defer == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_defer->p_iteration_statement = p_iteration_statement;
        defer_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
        defer_visit_ctx_pop_tail_block(ctx);
        b_secondary_block_ends_with_jump = secondary_block_ends_with_jump(p_iteration_statement->secondary_block);
        if (!b_secondary_block_ends_with_jump)
        {
            struct defer_defer_scope * p_defer2;

            p_defer2 = defer_visit_ctx_push_tail_block(ctx);
            if (p_defer2 == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            p_defer2->p_iteration_statement = p_iteration_statement;
            defer_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
            defer_exit_block_visit(ctx, p_defer2, p_iteration_statement->secondary_block->last_token, &p_iteration_statement->defer_list);
            defer_visit_ctx_pop_tail_block(ctx);
        }
    }
    /*catch*/ else _CKL0:
    {
    }
}

static void defer_visit_iteration_statement(struct defer_visit_ctx * ctx, struct iteration_statement * p_iteration_statement)
{
    /*switch*/
    {
        register int   _R0 = p_iteration_statement->first_token->type;
        if (_R0 == 9042) goto _CKL1; /*case 9042*/
        if (_R0 == 9008) goto _CKL2; /*case 9008*/
        if (_R0 == 9015) goto _CKL3; /*case 9015*/
        goto /*default*/ _CKL4;

        {
            /*case 9042*/ _CKL1:
            defer_visit_while_statement(ctx, p_iteration_statement);
            /*break*/ goto _CKL0;

            /*case 9008*/ _CKL2:
            defer_visit_do_while_statement(ctx, p_iteration_statement);
            /*break*/ goto _CKL0;

            /*case 9015*/ _CKL3:
            defer_visit_for_statement(ctx, p_iteration_statement);
            /*break*/ goto _CKL0;

            /*default*/ _CKL4:
            ;
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
}

static void defer_visit_jump_statement(struct defer_visit_ctx * ctx, struct jump_statement * p_jump_statement)
{
    ;
    /*try*/ if (1)
    {
        if (p_jump_statement->first_token->type == 9037)
        {
            defer_check_all_defer_until_try(ctx, ctx->tail_block, p_jump_statement->first_token, &p_jump_statement->defer_list);
        }
        else
        {
            if (p_jump_statement->first_token->type == 9027)
            {
                defer_exit_function_visit(ctx, ctx->tail_block, p_jump_statement->first_token, &p_jump_statement->defer_list);
            }
            else
            {
                if (p_jump_statement->first_token->type == 9005)
                {
                    defer_check_all_defer_until_iter(ctx, ctx->tail_block, p_jump_statement->first_token, &p_jump_statement->defer_list);
                }
                else
                {
                    if (p_jump_statement->first_token->type == 9000)
                    {
                        defer_exit_iteration_or_switch_statement_visit(ctx, ctx->tail_block, p_jump_statement->first_token, &p_jump_statement->defer_list);
                    }
                    else
                    {
                        if (p_jump_statement->first_token->type == 9016)
                        {
                            ;
                            defer_check_all_defer_until_label(ctx, ctx->tail_block, p_jump_statement->label->lexeme, p_jump_statement->first_token, &p_jump_statement->defer_list);
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
    /*catch*/ else _CKL0:
    {
    }
}

static void defer_visit_labeled_statement(struct defer_visit_ctx * ctx, struct labeled_statement * p_labeled_statement)
{
    defer_visit_statement(ctx, p_labeled_statement->statement);
}

static void defer_visit_primary_block(struct defer_visit_ctx * ctx, struct primary_block * p_primary_block)
{
    if (p_primary_block->defer_statement)
    {
        defer_visit_defer_statement(ctx, p_primary_block->defer_statement);
    }
    else
    {
        if (p_primary_block->compound_statement)
        {
            defer_visit_compound_statement(ctx, p_primary_block->compound_statement);
        }
        else
        {
            if (p_primary_block->iteration_statement)
            {
                defer_visit_iteration_statement(ctx, p_primary_block->iteration_statement);
            }
            else
            {
                if (p_primary_block->selection_statement)
                {
                    defer_visit_selection_statement(ctx, p_primary_block->selection_statement);
                }
                else
                {
                    if (p_primary_block->try_statement)
                    {
                        defer_visit_try_statement(ctx, p_primary_block->try_statement);
                    }
                }
            }
        }
    }
}

static void defer_visit_unlabeled_statement(struct defer_visit_ctx * ctx, struct unlabeled_statement * p_unlabeled_statement)
{
    if (p_unlabeled_statement->primary_block)
    {
        defer_visit_primary_block(ctx, p_unlabeled_statement->primary_block);
    }
    else
    {
        if (p_unlabeled_statement->expression_statement)
        {
        }
        else
        {
            if (p_unlabeled_statement->jump_statement)
            {
                defer_visit_jump_statement(ctx, p_unlabeled_statement->jump_statement);
            }
            else
            {
                ;
            }
        }
    }
}

static void defer_visit_statement(struct defer_visit_ctx * ctx, struct statement * p_statement)
{
    if (p_statement->labeled_statement)
    {
        defer_visit_labeled_statement(ctx, p_statement->labeled_statement);
    }
    else
    {
        if (p_statement->unlabeled_statement)
        {
            defer_visit_unlabeled_statement(ctx, p_statement->unlabeled_statement);
        }
    }
}

void defer_visit_declaration(struct defer_visit_ctx * ctx, struct declaration * p_declaration);

static void defer_visit_block_item(struct defer_visit_ctx * ctx, struct block_item * p_block_item)
{
    if (p_block_item->declaration)
    {
        defer_visit_declaration(ctx, p_block_item->declaration);
    }
    else
    {
        if (p_block_item->unlabeled_statement)
        {
            defer_visit_unlabeled_statement(ctx, p_block_item->unlabeled_statement);
        }
        else
        {
            if (p_block_item->label)
            {
            }
        }
    }
}

static void defer_visit_direct_declarator(struct defer_visit_ctx * ctx, struct direct_declarator * p_direct_declarator)
{
    if (p_direct_declarator->function_declarator)
    {
        struct parameter_declaration * parameter;

        parameter = 0U;
        if (p_direct_declarator->function_declarator->parameter_type_list_opt && p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list)
        {
            parameter = p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head;
        }
        ctx->parameter_list++;
        while (parameter)
        {
            if (parameter->declarator)
            {
                defer_visit_declarator(ctx, parameter->declarator);
            }
            parameter = parameter->next;
        }
        ctx->parameter_list--;
    }
    else
    {
        if (p_direct_declarator->array_declarator)
        {
        }
    }
}

static void defer_visit_declarator(struct defer_visit_ctx * ctx, struct declarator * p_declarator)
{
    /*try*/ if (1)
    {
        if (ctx->tail_block && p_declarator->type.category != 1)
        {
            struct defer_defer_scope * p_defer;

            if (ctx->parameter_list > 1)
            {
                return;
            }
            p_defer = defer_visit_ctx_push_child(ctx);
            if (p_defer == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            p_defer->declarator = p_declarator;
        }
        if (p_declarator->direct_declarator)
        {
            defer_visit_direct_declarator(ctx, p_declarator->direct_declarator);
        }
    }
    /*catch*/ else _CKL0:
    {
    }
}

static void defer_visit_init_declarator_list(struct defer_visit_ctx * ctx, struct init_declarator_list * p_init_declarator_list)
{
    struct init_declarator * p_init_declarator;

    p_init_declarator = p_init_declarator_list->head;
    while (p_init_declarator)
    {
        defer_visit_init_declarator(ctx, p_init_declarator);
        p_init_declarator = p_init_declarator->next;
    }
}

void defer_visit_declaration(struct defer_visit_ctx * ctx, struct declaration * p_declaration)
{
    if (p_declaration->init_declarator_list.head)
    {
        defer_visit_init_declarator_list(ctx, &p_declaration->init_declarator_list);
    }
    if (p_declaration->function_body)
    {
        defer_visit_compound_statement(ctx, p_declaration->function_body);
    }
}

void defer_start_visit_declaration(struct defer_visit_ctx * ctx, struct declaration * p_declaration)
{
    if (p_declaration->function_body)
    {
        struct defer_defer_scope * p_defer;

        ;
        p_defer = defer_visit_ctx_push_tail_block(ctx);
        if (p_defer == 0U)
        {
            return;
        }
        p_defer->p_function_body = p_declaration->function_body;
        defer_visit_declaration(ctx, p_declaration);
        ;
        if (ctx->tail_block)
        {
            defer_exit_block_visit(ctx, ctx->tail_block, p_declaration->function_body->last_token, &p_declaration->defer_list);
        }
        defer_visit_ctx_pop_tail_block(ctx);
    }
    else
    {
        struct defer_defer_scope * p_defer;

        p_defer = defer_visit_ctx_push_tail_block(ctx);
        if (p_defer == 0U)
        {
            return;
        }
        defer_visit_declaration(ctx, p_declaration);
        defer_visit_ctx_pop_tail_block(ctx);
    }
}

void defer_visit_ctx_destroy(struct defer_visit_ctx * p)
{
    ;
}

