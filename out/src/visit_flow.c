struct designator {
    struct expression * constant_expression_opt;
    struct token * token;
    struct designator * next;
};

struct declaration_list {
    struct declaration * head;
    struct declaration * tail;
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

struct switch_value {
    long long value;
    struct label * p_label;
    struct switch_value * next;
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

struct block_item_list {
    struct block_item * head;
    struct block_item * tail;
};

struct init_declarator {
    unsigned char   has_shared_ownership;
    struct declarator * p_declarator;
    struct initializer * initializer;
    struct init_declarator * next;
};

struct member_declaration_list {
    struct token * first_token;
    struct token * last_token;
    struct member_declaration * head;
    struct member_declaration * tail;
};

struct macro;
struct unlabeled_statement {
    struct expression_statement * expression_statement;
    struct primary_block * primary_block;
    struct jump_statement * jump_statement;
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

union _struct_tag_1 {
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
    union _struct_tag_1  value;
    struct object * parent;
    struct expression * p_init_expression;
    struct object * members;
    struct object * next;
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

struct attribute {
    int   attributes_flags;
    struct attribute_token * attribute_token;
    struct attribute_argument_clause * attribute_argument_clause;
    struct attribute * next;
};

struct primary_block {
    struct compound_statement * compound_statement;
    struct selection_statement * selection_statement;
    struct iteration_statement * iteration_statement;
    struct defer_statement * defer_statement;
    struct try_statement * try_statement;
};

struct member_declarator_list {
    struct member_declarator * head;
    struct member_declarator * tail;
};

struct attribute_token {
    int   attributes_flags;
    struct token * token;
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

struct initializer_list {
    struct token * first_token;
    struct initializer_list_item * head;
    struct initializer_list_item * tail;
    int size;
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

struct parameter_type_list {
    unsigned char   is_var_args;
    unsigned char   is_void;
    struct parameter_list * parameter_list;
};

struct array_declarator {
    struct direct_declarator * direct_declarator;
    struct expression * assignment_expression;
    struct expression * expression;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * token;
    struct token * static_token_opt;
};

struct member_declaration {
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct member_declarator_list * member_declarator_list_opt;
    struct static_assert_declaration * static_assert_declaration;
    struct pragma_declaration * pragma_declaration;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct member_declaration * next;
};

struct flow_objects_view {
    struct flow_object ** data;
    int size;
    int capacity;
};

struct parameter_declaration {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct declaration_specifiers * declaration_specifiers;
    struct declarator * declarator;
    struct parameter_declaration * next;
};

struct type_name {
    struct token * first_token;
    struct token * last_token;
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct declarator * abstract_declarator;
    struct type  type;
};

struct param {
    struct type  type;
    struct param * next;
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
    int try_catch_block_index;
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

struct parameter_list {
    struct parameter_declaration * head;
    struct parameter_declaration * tail;
};

struct condition {
    struct expression * expression;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator * p_init_declarator;
    struct token * first_token;
    struct token * last_token;
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

struct init_declarator_list {
    struct init_declarator * head;
    struct init_declarator * tail;
};

struct flow_objects {
    struct flow_object ** data;
    int size;
    int capacity;
};

struct type_qualifier_list {
    int   flags;
    struct type_qualifier * head;
    struct type_qualifier * tail;
};

struct block_item {
    struct token * first_token;
    struct declaration * declaration;
    struct unlabeled_statement * unlabeled_statement;
    struct label * label;
    struct block_item * next;
};

struct ast {
    struct token_list  token_list;
    struct declaration_list  declaration_list;
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

struct braced_initializer {
    struct token * first_token;
    struct token * last_token;
    struct initializer_list * initializer_list;
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

struct true_false_set_item {
    struct expression * p_expression;
    int   true_branch_state;
    int   false_branch_state;
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

struct flow_object_state {
    char * dbg_name;
    int state_number;
    struct flow_object * pointed;
    int   state;
    struct flow_objects_view  alternatives;
    struct flow_object_state * next;
};

struct type_qualifier {
    int   flags;
    struct token * token;
    struct type_qualifier * next;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct initializer_list_item {
    struct designation * designation;
    struct initializer * initializer;
    struct initializer_list_item * next;
};

struct attribute_list {
    int   attributes_flags;
    struct attribute * head;
    struct attribute * tail;
};

struct labeled_statement {
    struct label * label;
    struct statement * statement;
};

struct generic_assoc_list {
    struct generic_association * head;
    struct generic_association * tail;
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

struct generic_selection {
    struct expression * expression;
    struct type_name * type_name;
    struct expression * p_view_selected_expression;
    struct generic_assoc_list  generic_assoc_list;
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

struct direct_declarator {
    struct token * name_opt;
    struct declarator * declarator;
    struct array_declarator * array_declarator;
    struct function_declarator * function_declarator;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
};

struct declaration_specifier {
    struct storage_class_specifier * storage_class_specifier;
    struct type_specifier_qualifier * type_specifier_qualifier;
    struct function_specifier * function_specifier;
    struct declaration_specifier * next;
};

struct argument_expression_list {
    struct argument_expression * head;
    struct argument_expression * tail;
};

struct member_declarator {
    struct declarator * declarator;
    struct expression * constant_expression;
    struct member_declarator * next;
};

struct init_statement {
    struct expression_statement * p_expression_statement;
    struct simple_declaration * p_simple_declaration;
};

struct true_false_set {
    struct true_false_set_item * data;
    int size;
    int capacity;
};

struct function_specifier {
    int   flags;
    struct token * token;
};

struct initializer {
    struct token * first_token;
    struct braced_initializer * braced_initializer;
    struct expression * assignment_expression;
};

struct label {
    struct expression * constant_expression;
    struct token * p_identifier_opt;
    struct token * p_first_token;
};

struct simple_declaration {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator_list  init_declarator_list;
    struct token * first_token;
    struct token * last_token;
};

struct struct_entry;
struct __crt_multibyte_data;
struct __crt_locale_data;
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

struct pointer {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct pointer * pointer;
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

struct secondary_block {
    struct token * first_token;
    struct token * last_token;
    struct statement * statement;
};

struct defer_list_item {
    struct declarator * declarator;
    struct defer_statement * defer_statement;
    struct defer_list_item * next;
};

struct alignment_specifier {
    struct type_name * type_name;
    struct expression * constant_expression;
    struct token * token;
};

struct _iobuf {
    void * _Placeholder;
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

struct balanced_token {
    struct token * token;
    struct balanced_token * next;
};

struct defer_statement {
    struct token * first_token;
    struct token * last_token;
    struct secondary_block * secondary_block;
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

struct switch_value_list {
    struct switch_value * head;
    struct switch_value * tail;
    struct switch_value * p_default;
};

struct attribute_argument_clause {
    struct balanced_token_sequence * p_balanced_token_sequence;
    struct token * token;
};

struct generic_association {
    struct type  type;
    struct type_name * p_type_name;
    struct expression * expression;
    struct token * first_token;
    struct token * last_token;
    struct generic_association * next;
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

struct type_specifier_qualifier {
    struct type_specifier * type_specifier;
    struct type_qualifier * type_qualifier;
    struct alignment_specifier * alignment_specifier;
    struct type_specifier_qualifier * next;
};

struct typeof_specifier {
    struct token * first_token;
    struct token * last_token;
    struct typeof_specifier_argument * typeof_specifier_argument;
    struct type  type;
};

struct designator_list {
    struct designator * head;
    struct designator * tail;
};

struct typeof_specifier_argument {
    struct expression * expression;
    struct type_name * type_name;
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

struct expression_statement {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct expression * expression_opt;
};

struct balanced_token_sequence {
    struct balanced_token * head;
    struct balanced_token * tail;
};

struct atomic_type_specifier {
    struct token * token;
    struct type_name * type_name;
};

struct storage_class_specifier {
    int   flags;
    struct token * token;
};

struct attribute_specifier {
    struct token * first_token;
    struct token * last_token;
    struct attribute_list * attribute_list;
    struct attribute_specifier * next;
};

struct jump_statement {
    struct token * label;
    struct token * first_token;
    struct token * last_token;
    struct expression * expression_opt;
    int try_catch_block_index;
    struct defer_list  defer_list;
};

struct designation {
    struct designator_list * designator_list;
    struct token * token;
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

struct statement {
    struct labeled_statement * labeled_statement;
    struct unlabeled_statement * unlabeled_statement;
};


void free(void * ptr);

void true_false_set_clear(struct true_false_set * p)
{
    free(p->data);
    p->data = ((void *)0);
    p->size = 0;
    p->capacity = 0;
}

void true_false_set_destroy(struct true_false_set * p)
{
    free(p->data);
}

void *realloc(void * ptr, unsigned int size);

int true_false_set_reserve(struct true_false_set * p, int n)
{
    if (n > p->capacity)
    {
        if ((unsigned int)n > (4294967295U / (sizeof (p->data[0]))))
        {
            return 132;
        }
        void * pnew = realloc(p->data, n * sizeof (p->data[0]));
        if (pnew == ((void *)0))
        {
            return 12;
        }
        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}

int true_false_set_push_back(struct true_false_set * p, struct true_false_set_item * book)
{
    if (p->size == 2147483647)
    {
        return 132;
    }
    if (p->size + 1 > p->capacity)
    {
        int new_capacity = 0;
        if (p->capacity > (2147483647 - p->capacity / 2))
        {
            new_capacity = 2147483647;
        }
        else
        {
            new_capacity = p->capacity + p->capacity / 2;
            if (new_capacity < p->size + 1)
            {
                new_capacity = p->size + 1;
            }
        }
        int error = true_false_set_reserve(p, new_capacity);
        if (error != 0)
        {
            return error;
        }
    }
    p->data[p->size] =  *book;
    p->size++;
    return 0;
}

static void true_false_set_invert(struct true_false_set * true_false_set)
{
    {
        int i = 0;
        for (; i < true_false_set->size; i++)
        {
            int   temp = true_false_set->data[i].true_branch_state;
            true_false_set->data[i].true_branch_state = true_false_set->data[i].false_branch_state;
            true_false_set->data[i].false_branch_state = temp;
        }
    }
}

static void true_false_set_swap(struct true_false_set * a, struct true_false_set * b)
{
    struct true_false_set  temp =  *a;
     *a =  *b;
     *b = temp;
}

static int find_item_index_by_expression(struct true_false_set * a, struct expression * p_expression)
{
    {
        int i = 0;
        for (; i < a->size; i++)
        {
            if (a->data[i].p_expression == p_expression)
            {
                return i;
            }
        }
    }
    return  -1;
}

void true_false_set_merge(struct true_false_set * result, struct true_false_set * a, struct true_false_set * b, int   options_true, int   options_false)
{
    {
        int i = 0;
        for (; i < a->size; i++)
        {
            struct true_false_set_item * p_item_a = &a->data[i];
            struct true_false_set_item  new_item = {0};
            new_item.p_expression = p_item_a->p_expression;
            if (options_true & 1)
            {
                new_item.true_branch_state = p_item_a->true_branch_state;
            }
            if (options_true & 2)
            {
                new_item.true_branch_state = p_item_a->false_branch_state;
            }
            if (options_false & 1)
            {
                new_item.false_branch_state = p_item_a->true_branch_state;
            }
            if (options_false & 2)
            {
                new_item.true_branch_state = p_item_a->false_branch_state;
            }
            true_false_set_push_back(result, &new_item);
        }
    }
    {
        int k = 0;
        for (; k < b->size; k++)
        {
            struct true_false_set_item * p_item_b = &b->data[k];
            int index = find_item_index_by_expression(result, p_item_b->p_expression);
            if (index ==  -1)
            {
                index = result->size;
                struct true_false_set_item  item2 = {0};
                item2.p_expression = p_item_b->p_expression;
                true_false_set_push_back(result, &item2);
            }
            struct true_false_set_item * p_item_result = &result->data[index];
            if (options_true & 4)
            {
                p_item_result->true_branch_state = p_item_b->true_branch_state;
            }
            if (options_true & 8)
            {
                p_item_result->true_branch_state = p_item_b->false_branch_state;
            }
            if (options_false & 4)
            {
                p_item_result->false_branch_state = p_item_b->true_branch_state;
            }
            if (options_false & 8)
            {
                p_item_result->true_branch_state = p_item_b->false_branch_state;
            }
        }
    }
}

struct flow_object *expression_get_flow_object(struct flow_visit_ctx * ctx, struct expression * p_expression, unsigned char   nullable_enabled);

static void true_false_set_set_objects_to_core_branch(struct flow_visit_ctx * ctx, struct true_false_set * a, unsigned char   nullable_enabled, unsigned char   true_branch)
{
    {
        int i = 0;
        for (; i < a->size; i++)
        {
            if (a->data[i].p_expression != ((void *)0))
            {
                struct flow_object * p_object = expression_get_flow_object(ctx, a->data[i].p_expression, nullable_enabled);
                if (p_object)
                {
                    if (p_object->current.state == 4 || p_object->current.state == 2 || p_object->current.state == 8 || p_object->current.state == 32 || p_object->current.state == 64 || p_object->current.state == 128)
                    {
                        continue;
                    }
                    int   flag = true_branch ? a->data[i].true_branch_state : a->data[i].false_branch_state;
                    if ((flag & 1) && (flag & 2))
                    {
                    }
                    else
                    {
                        if (flag & 2)
                        {
                            p_object->current.state =  ~4;
                            p_object->current.state =  ~8;
                        }
                        else
                        {
                            if (flag & 1)
                            {
                                p_object->current.state =  ~2;
                                p_object->current.state =  ~32;
                            }
                        }
                    }
                }
            }
        }
    }
}

static void true_false_set_set_objects_to_true_branch(struct flow_visit_ctx * ctx, struct true_false_set * a, unsigned char   nullable_enabled)
{
    true_false_set_set_objects_to_core_branch(ctx, a, nullable_enabled, 1);
}

static void true_false_set_set_objects_to_false_branch(struct flow_visit_ctx * ctx, struct true_false_set * a, unsigned char   nullable_enabled)
{
    true_false_set_set_objects_to_core_branch(ctx, a, nullable_enabled, 0);
}

static void flow_visit_secondary_block(struct flow_visit_ctx * ctx, struct secondary_block * p_secondary_block);
unsigned char  compiler_diagnostic_message(int   w, struct parser_ctx * ctx, struct token * p_token, struct marker * p_marker, char * fmt, ...);
void flow_end_of_block_visit(struct flow_visit_ctx * ctx, struct type * p_type, unsigned char   type_is_view, struct flow_object * p_object, struct token * position_token, char * previous_names);
unsigned char  type_is_view(struct type * p_type);

static void flow_exit_block_visit_defer_item(struct flow_visit_ctx * ctx, struct defer_list_item * p_item, struct token * position_token)
{
    if (p_item->defer_statement)
    {
        int error_count = ctx->ctx->p_report->error_count;
        int warnings_count = ctx->ctx->p_report->warnings_count;
        int info_count = ctx->ctx->p_report->info_count;
        flow_visit_secondary_block(ctx, p_item->defer_statement->secondary_block);
        if (error_count != ctx->ctx->p_report->error_count || warnings_count != ctx->ctx->p_report->warnings_count || info_count != ctx->ctx->p_report->info_count)
        {
            compiler_diagnostic_message(62, ctx->ctx, position_token, ((void *)0), "defer end of scope");
        }
    }
    else
    {
        if (p_item->declarator)
        {
            struct declarator * p_declarator = p_item->declarator;
            char * name = p_declarator->name_opt ? p_declarator->name_opt->lexeme : "?";
            if (p_declarator->p_object)
            {
                flow_end_of_block_visit(ctx, &p_declarator->type, type_is_view(&p_declarator->type), p_declarator->p_object, position_token, name);
            }
            else
            {
                ;
            }
        }
    }
}

static void flow_exit_block_visit_defer_list(struct flow_visit_ctx * ctx, struct defer_list * p_defer_list, struct token * position_token)
{
    struct defer_list_item * p_item = p_defer_list->head;
    while (p_item)
    {
        flow_exit_block_visit_defer_item(ctx, p_item, position_token);
        p_item = p_item->next;
    }
}

void flow_object_set_end_of_lifetime(struct type * p_type, struct flow_object * p_object);

static void flow_defer_item_set_end_of_lifetime(struct flow_visit_ctx * ctx, struct defer_list_item * p_item, struct token * position_token)
{
    if (p_item->defer_statement)
    {
    }
    else
    {
        if (p_item->declarator)
        {
            struct declarator * p_declarator = p_item->declarator;
            if (p_declarator->p_object)
            {
                flow_object_set_end_of_lifetime(&p_declarator->type, p_declarator->p_object);
            }
            else
            {
                ;
            }
        }
    }
}

static void flow_defer_list_set_end_of_lifetime(struct flow_visit_ctx * ctx, struct defer_list * p_defer_list, struct token * position_token)
{
    struct defer_list_item * p_item = p_defer_list->head;
    while (p_item)
    {
        flow_defer_item_set_end_of_lifetime(ctx, p_item, position_token);
        p_item = p_item->next;
    }
}

static void flow_visit_statement(struct flow_visit_ctx * ctx, struct statement * p_statement);

static void flow_visit_secondary_block(struct flow_visit_ctx * ctx, struct secondary_block * p_secondary_block)
{
    flow_visit_statement(ctx, p_secondary_block->statement);
}

static void flow_visit_defer_statement(struct flow_visit_ctx * ctx, struct defer_statement * p_defer_statement)
{
}

void flow_object_add_new_state_as_a_copy_of_current_state(struct flow_object * object, char * name, int state_number);

static int arena_add_copy_of_current_state(struct flow_visit_ctx * ctx, char * name)
{
    int state_number = ctx->state_number_generator;
    ctx->state_number_generator++;
    {
        int i = 0;
        for (; i < ctx->arena.size; i++)
        {
            flow_object_add_new_state_as_a_copy_of_current_state(ctx->arena.data[i], name, state_number);
        }
    }
    return state_number;
}

void *calloc(int nmemb, unsigned int size);
int flow_object_add_state(struct flow_object * p, struct flow_object_state * pnew);

static int arena_add_empty_state(struct flow_visit_ctx * ctx, char * name)
{
    int state_number = ctx->state_number_generator;
    ctx->state_number_generator++;
    {
        int i = 0;
        for (; i < ctx->arena.size; i++)
        {
            struct flow_object * p_object = ctx->arena.data[i];
            struct flow_object_state * p_flow_object_state = calloc(1, sizeof  *p_flow_object_state);
            if (p_flow_object_state)
            {
                p_flow_object_state->dbg_name = name;
                p_flow_object_state->state_number = state_number;
                flow_object_add_state(p_object, p_flow_object_state);
            }
        }
    }
    return state_number;
}

void flow_object_state_copy(struct flow_object_state * to, struct flow_object_state * from);

void flow_object_set_state_from_current(struct flow_object * object, int state_number)
{
    struct flow_object_state * p_flow_object_state = object->current.next;
    while (p_flow_object_state)
    {
        if (p_flow_object_state->state_number == state_number)
        {
            flow_object_state_copy(p_flow_object_state, &object->current);
            break;
        }
        p_flow_object_state = p_flow_object_state->next;
    }
}

static void arena_set_state_from_current(struct flow_visit_ctx * ctx, int number_state)
{
    {
        int i = 0;
        for (; i < ctx->arena.size; i++)
        {
            struct flow_object * p_object = ctx->arena.data[i];
            flow_object_set_state_from_current(p_object, number_state);
        }
    }
}

struct flow_object *arena_new_object(struct flow_visit_ctx * ctx);
int objects_view_push_back(struct flow_objects_view * p, struct flow_object * p_object);

static int flow_object_merge_current_with_state(struct flow_visit_ctx * ctx, struct flow_object * object, int state_number)
{
    if (1)
    {
        struct flow_object_state * it = object->current.next;
        while (it)
        {
            if (it->state_number == state_number)
            {
                it->state = object->current.state;
                if (object->current.pointed == ((void *)0) && it->pointed == ((void *)0))
                {
                }
                else
                {
                    if (object->current.pointed == ((void *)0) && it->pointed != ((void *)0))
                    {
                    }
                    else
                    {
                        if (object->current.pointed != ((void *)0) && it->pointed == ((void *)0))
                        {
                            it->pointed = object->current.pointed;
                        }
                        else
                        {
                            if (object->current.pointed != it->pointed)
                            {
                                ;
                                ;
                                struct flow_object * p_new_object = arena_new_object(ctx);
                                if (p_new_object == ((void *)0))
                                {
                                    goto _catch_label_1;
                                }
                                struct flow_object_state * p_new_state = calloc(1, sizeof  *p_new_state);
                                if (p_new_state == ((void *)0))
                                {
                                    goto _catch_label_1;
                                }
                                p_new_state->dbg_name = "merged";
                                p_new_state->state_number = state_number;
                                objects_view_push_back(&p_new_state->alternatives, object->current.pointed);
                                objects_view_push_back(&p_new_state->alternatives, it->pointed);
                                flow_object_add_state(p_new_object, p_new_state);
                                int n_childs_1 = object->current.pointed->members.size;
                                int n_childs_2 = it->pointed->members.size;
                                if (n_childs_1 == n_childs_2)
                                {
                                    {
                                        int j = 0;
                                        for (; j < n_childs_1; j++)
                                        {
                                            struct flow_object * p_new_child = arena_new_object(ctx);
                                            if (p_new_child == ((void *)0))
                                            {
                                                goto _catch_label_1;
                                            }
                                            p_new_child->parent = p_new_object;
                                            struct flow_object * child1 = object->current.pointed->members.data[j];
                                            struct flow_object * child2 = it->pointed->members.data[j];
                                            struct flow_object_state * p_child_new_state = calloc(1, sizeof  *p_child_new_state);
                                            if (p_child_new_state == ((void *)0))
                                            {
                                                goto _catch_label_1;
                                            }
                                            p_child_new_state->dbg_name = "merged";
                                            p_child_new_state->state_number = state_number;
                                            p_child_new_state->state = child1->current.state | it->state;
                                            objects_view_push_back(&p_child_new_state->alternatives, child1);
                                            objects_view_push_back(&p_child_new_state->alternatives, child2);
                                            flow_object_add_state(p_new_child, p_child_new_state);
                                            objects_view_push_back(&p_new_object->members, p_new_child);
                                        }
                                    }
                                }
                                else
                                {
                                }
                                it->pointed = p_new_object;
                            }
                        }
                    }
                }
                break;
            }
            it = it->next;
        }
    }
    else _catch_label_1:
    {
    }
    return 0;
}

static void arena_merge_current_state_with_state_number(struct flow_visit_ctx * ctx, int number_state)
{
    {
        int i = 0;
        for (; i < ctx->arena.size; i++)
        {
            flow_object_merge_current_with_state(ctx, ctx->arena.data[i], number_state);
        }
    }
}

static void object_restore_current_state_from2(struct flow_visit_ctx * ctx, struct flow_object * object, int state_number)
{
    struct flow_object_state * it = object->current.next;
    while (it)
    {
        if (it->state_number == state_number)
        {
            flow_object_state_copy(&object->current, it);
            break;
        }
        it = it->next;
    }
}

static void arena_restore_current_state_from(struct flow_visit_ctx * ctx, int number_state)
{
    {
        int i = 0;
        for (; i < ctx->arena.size; i++)
        {
            object_restore_current_state_from2(ctx, ctx->arena.data[i], number_state);
        }
    }
}

void flow_object_remove_state(struct flow_object * object, int state_number);

static void arena_remove_state(struct flow_visit_ctx * ctx, int state_number)
{
    {
        int i = 0;
        for (; i < ctx->arena.size; i++)
        {
            flow_object_remove_state(ctx->arena.data[i], state_number);
        }
    }
}

void flow_object_set_zero(struct type * p_type, struct flow_object * p_object);

static void braced_initializer_set_object(struct braced_initializer * p, struct type * type, struct flow_object * object)
{
    if (p->initializer_list == ((void *)0))
    {
    }
    flow_object_set_zero(type, object);
}

static void flow_visit_declarator(struct flow_visit_ctx * ctx, struct declarator * p_declarator);
static void flow_visit_expression(struct flow_visit_ctx * ctx, struct expression * p_expression, struct true_false_set * a);
static void flow_visit_bracket_initializer_list(struct flow_visit_ctx * ctx, struct braced_initializer * p_bracket_initializer_list);
void flow_check_assignment(struct flow_visit_ctx * ctx, struct token * error_position, struct marker * p_a_marker, struct marker * p_b_marker, int   assigment_type, unsigned char   check_uninitialized_b, unsigned char   a_type_is_view, unsigned char   a_type_is_nullable, struct type * p_a_type, struct flow_object * p_a_object, struct type * p_b_type, struct flow_object * p_b_object, unsigned char  * set_argument_to_unkown);
unsigned char  type_is_nullable(struct type * p_type, unsigned char   nullable_enabled);
unsigned char  expression_is_malloc(struct expression * p);
struct type type_remove_pointer(struct type * p_type);
struct flow_object *make_flow_object(struct flow_visit_ctx * ctx, struct type * p_type, struct declarator * p_declarator_opt, struct expression * p_expression_origin);
void object_set_pointer(struct flow_object * p_object, struct flow_object * p_object2);
void type_destroy(struct type * p_type);
unsigned char  expression_is_calloc(struct expression * p);
void flow_object_set_uninitialized(struct type * p_type, struct flow_object * p_object);

static void flow_visit_init_declarator(struct flow_visit_ctx * ctx, struct init_declarator * p_init_declarator)
{
    unsigned char   nullable_enabled = ctx->ctx->options.null_checks_enabled;
    if (1)
    {
        flow_visit_declarator(ctx, p_init_declarator->p_declarator);
        if (p_init_declarator->initializer)
        {
            if (p_init_declarator->initializer->assignment_expression)
            {
                struct true_false_set  a = {0};
                flow_visit_expression(ctx, p_init_declarator->initializer->assignment_expression, &a);
                true_false_set_destroy(&a);
            }
            else
            {
                if (p_init_declarator->initializer->braced_initializer)
                {
                    flow_visit_bracket_initializer_list(ctx, p_init_declarator->initializer->braced_initializer);
                }
                else
                {
                    ;
                }
            }
        }
        if (p_init_declarator->p_declarator->type.category != 1)
        {
            if (p_init_declarator->initializer && p_init_declarator->initializer->assignment_expression)
            {
                struct flow_object * p_right_object = expression_get_flow_object(ctx, p_init_declarator->initializer->assignment_expression, nullable_enabled);
                if (p_right_object)
                {
                    struct marker  a_marker = {0, 0, 0, 0, 0, 0, 0};
                    a_marker.p_token_begin = p_init_declarator->p_declarator->first_token_opt;
                    a_marker.p_token_end = p_init_declarator->p_declarator->last_token_opt;
                    struct marker  b_marker = {0, 0, 0, 0, 0, 0, 0};
                    b_marker.p_token_begin = p_init_declarator->initializer->assignment_expression->first_token;
                    b_marker.p_token_end = p_init_declarator->initializer->assignment_expression->last_token;
                    ;
                    flow_check_assignment(ctx, p_init_declarator->initializer->assignment_expression->first_token, &a_marker, &b_marker, 2, 0, type_is_view(&p_init_declarator->p_declarator->type), type_is_nullable(&p_init_declarator->p_declarator->type, ctx->ctx->options.null_checks_enabled), &p_init_declarator->p_declarator->type, p_init_declarator->p_declarator->p_object, &p_init_declarator->initializer->assignment_expression->type, p_right_object, ((void *)0));
                }
                if (expression_is_malloc(p_init_declarator->initializer->assignment_expression))
                {
                    struct type  t = type_remove_pointer(&p_init_declarator->p_declarator->type);
                    struct flow_object * po = make_flow_object(ctx, &t, p_init_declarator->p_declarator, ((void *)0));
                    object_set_pointer(p_init_declarator->p_declarator->p_object, po);
                    type_destroy(&t);
                    p_init_declarator->p_declarator->p_object->current.state = 4 | 2;
                }
                else
                {
                    if (expression_is_calloc(p_init_declarator->initializer->assignment_expression))
                    {
                        struct type  t = type_remove_pointer(&p_init_declarator->p_declarator->type);
                        struct flow_object * po = make_flow_object(ctx, &t, p_init_declarator->p_declarator, ((void *)0));
                        if (po == ((void *)0))
                        {
                            type_destroy(&t);
                            goto _catch_label_1;
                        }
                        flow_object_set_zero(&t, po);
                        object_set_pointer(p_init_declarator->p_declarator->p_object, po);
                        type_destroy(&t);
                        p_init_declarator->p_declarator->p_object->current.state = 4 | 2;
                    }
                }
            }
            else
            {
                if (p_init_declarator->initializer && p_init_declarator->initializer->braced_initializer)
                {
                    struct flow_object * po = make_flow_object(ctx, &p_init_declarator->p_declarator->type, p_init_declarator->p_declarator, ((void *)0));
                    if (po == ((void *)0))
                    {
                        goto _catch_label_1;
                    }
                    braced_initializer_set_object(p_init_declarator->initializer->braced_initializer, &p_init_declarator->p_declarator->type, po);
                    struct marker  a_marker = {0, 0, 0, 0, 0, 0, 0};
                    a_marker.p_token_begin = p_init_declarator->p_declarator->first_token_opt;
                    a_marker.p_token_end = p_init_declarator->p_declarator->last_token_opt;
                    struct marker  b_marker = {0, 0, 0, 0, 0, 0, 0};
                    b_marker.p_token_begin = p_init_declarator->initializer->braced_initializer->first_token;
                    b_marker.p_token_end = p_init_declarator->initializer->braced_initializer->last_token;
                    struct flow_object * p_right_object = po;
                    flow_check_assignment(ctx, p_init_declarator->p_declarator->first_token_opt, &a_marker, &b_marker, 2, 0, type_is_view(&p_init_declarator->p_declarator->type), type_is_nullable(&p_init_declarator->p_declarator->type, ctx->ctx->options.null_checks_enabled), &p_init_declarator->p_declarator->type, p_init_declarator->p_declarator->p_object, &p_init_declarator->p_declarator->type, p_right_object, ((void *)0));
                }
                else
                {
                    if (p_init_declarator->p_declarator->declaration_specifiers && ((p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & 2) || (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & 4)))
                    {
                        flow_object_set_zero(&p_init_declarator->p_declarator->type, p_init_declarator->p_declarator->p_object);
                    }
                    else
                    {
                        flow_object_set_uninitialized(&p_init_declarator->p_declarator->type, p_init_declarator->p_declarator->p_object);
                    }
                }
            }
        }
    }
    else _catch_label_1:
    {
    }
}

static void flow_visit_declaration_specifiers(struct flow_visit_ctx * ctx, struct declaration_specifiers * p_declaration_specifiers, struct type * p_type);
static void flow_visit_init_declarator_list(struct flow_visit_ctx * ctx, struct init_declarator_list * p_init_declarator_list);

static void flow_visit_simple_declaration(struct flow_visit_ctx * ctx, struct simple_declaration * p_simple_declaration)
{
    flow_visit_declaration_specifiers(ctx, p_simple_declaration->p_declaration_specifiers, ((void *)0));
    flow_visit_init_declarator_list(ctx, &p_simple_declaration->init_declarator_list);
}



int __stdio_common_vfprintf(unsigned __int64 _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);

inline unsigned __int64 *__local_stdio_printf_options(void)
{
    static unsigned __int64 _OptionsStorage;
    return &_OptionsStorage;
}
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
void print_object_line(struct flow_object * p_object, int cols);

void print_arena(struct flow_visit_ctx * ctx)
{
    int extra_cols = 0;
    {
        int i = 0;
        for (; i < ctx->arena.size; i++)
        {
            struct flow_object * p_object = ctx->arena.data[i];
            struct flow_object_state * p_state = p_object->current.next;
            int count = 0;
            while (p_state)
            {
                count++;
                p_state = p_state->next;
            }
            if (count > extra_cols)
            {
                extra_cols = count;
            }
        }
    }
    printf("\n");
    printf("\xe2\x94\x8c\xe2\x94\x80\xe2\x94\x80\xe2\x94\xac\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\xac\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80");
    if (extra_cols > 0)
    {
        {
            int i = 0;
            for (; i < extra_cols; i++)
            {
                if (i < extra_cols - 1)
                {
                    printf("\xe2\x94\xac\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80");
                }
                else
                {
                    printf("\xe2\x94\xac\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80");
                }
            }
        }
    }
    printf("\xe2\x94\x90");
    printf("\n");
    {
        int i = 0;
        for (; i < ctx->arena.size; i++)
        {
            struct flow_object * p = ctx->arena.data[i];
            print_object_line(p, extra_cols);
        }
    }
    printf("\xe2\x94\x94\xe2\x94\x80\xe2\x94\x80\xe2\x94\xb4\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\xb4\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80");
    if (extra_cols > 0)
    {
        {
            int i = 0;
            for (; i < extra_cols; i++)
            {
                if (i < extra_cols - 1)
                {
                    printf("\xe2\x94\xb4\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80");
                }
                else
                {
                    printf("\xe2\x94\xb4\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80");
                }
            }
        }
    }
    printf("\xe2\x94\x98");
    printf("\n");
    printf("\n");
}

static void flow_visit_expression_statement(struct flow_visit_ctx * ctx, struct expression_statement * p_expression_statement);
static void flow_check_pointer_used_as_bool(struct flow_visit_ctx * ctx, struct expression * p_expression);
unsigned char  secondary_block_ends_with_jump(struct secondary_block * p_secondary_block);

static void flow_visit_if_statement(struct flow_visit_ctx * ctx, struct selection_statement * p_selection_statement)
{
    unsigned char   nullable_enabled = ctx->ctx->options.null_checks_enabled;
    if (p_selection_statement->p_init_statement && p_selection_statement->p_init_statement->p_expression_statement)
    {
        flow_visit_expression_statement(ctx, p_selection_statement->p_init_statement->p_expression_statement);
    }
    if (p_selection_statement->p_init_statement && p_selection_statement->p_init_statement->p_simple_declaration)
    {
        flow_visit_simple_declaration(ctx, p_selection_statement->p_init_statement->p_simple_declaration);
    }
    struct true_false_set  true_false_set = {0};
    if (p_selection_statement->condition && p_selection_statement->condition->expression)
    {
        flow_check_pointer_used_as_bool(ctx, p_selection_statement->condition->expression);
        flow_visit_expression(ctx, p_selection_statement->condition->expression, &true_false_set);
    }
    if (p_selection_statement->condition && p_selection_statement->condition->p_init_declarator)
    {
        flow_visit_init_declarator(ctx, p_selection_statement->condition->p_init_declarator);
    }
    ;
    int before_if_state_number = arena_add_copy_of_current_state(ctx, "before-if");
    true_false_set_set_objects_to_true_branch(ctx, &true_false_set, nullable_enabled);
    flow_visit_secondary_block(ctx, p_selection_statement->secondary_block);
    unsigned char   true_branch_ends_with_jump = secondary_block_ends_with_jump(p_selection_statement->secondary_block);
    int left_true_branch_state_number = arena_add_copy_of_current_state(ctx, "left-true-branch");
    arena_restore_current_state_from(ctx, before_if_state_number);
    if (p_selection_statement->else_secondary_block_opt)
    {
        true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);
        flow_visit_secondary_block(ctx, p_selection_statement->else_secondary_block_opt);
    }
    else
    {
        true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);
    }
    unsigned char   else_ends_with_jump = p_selection_statement->else_secondary_block_opt ? secondary_block_ends_with_jump(p_selection_statement->else_secondary_block_opt) : 0;
    if (true_branch_ends_with_jump)
    {
        if (else_ends_with_jump)
        {
        }
        else
        {
        }
    }
    else
    {
        if (else_ends_with_jump)
        {
            arena_restore_current_state_from(ctx, left_true_branch_state_number);
        }
        else
        {
            arena_merge_current_state_with_state_number(ctx, left_true_branch_state_number);
            arena_restore_current_state_from(ctx, left_true_branch_state_number);
        }
    }
    flow_exit_block_visit_defer_list(ctx, &p_selection_statement->defer_list, p_selection_statement->last_token);
    flow_defer_list_set_end_of_lifetime(ctx, &p_selection_statement->defer_list, p_selection_statement->last_token);
    arena_remove_state(ctx, before_if_state_number);
    arena_remove_state(ctx, left_true_branch_state_number);
    true_false_set_destroy(&true_false_set);
}

static void flow_visit_try_statement(struct flow_visit_ctx * ctx, struct try_statement * p_try_statement)
{
    if (1)
    {
        int throw_join_state_old = ctx->throw_join_state;
        struct secondary_block * catch_secondary_block_old = ctx->catch_secondary_block_opt;
        ctx->catch_secondary_block_opt = p_try_statement->catch_secondary_block_opt;
        ctx->throw_join_state = arena_add_empty_state(ctx, "try");
        int original_state_number = arena_add_copy_of_current_state(ctx, "original");
        flow_visit_secondary_block(ctx, p_try_statement->secondary_block);
        arena_set_state_from_current(ctx, original_state_number);
        if (p_try_statement->catch_secondary_block_opt)
        {
            arena_restore_current_state_from(ctx, ctx->throw_join_state);
            flow_visit_secondary_block(ctx, p_try_statement->catch_secondary_block_opt);
        }
        unsigned char   try_reached_the_end =  !secondary_block_ends_with_jump(p_try_statement->secondary_block);
        unsigned char   catch_reached_the_end =  !secondary_block_ends_with_jump(p_try_statement->catch_secondary_block_opt);
        if (try_reached_the_end && catch_reached_the_end)
        {
            arena_merge_current_state_with_state_number(ctx, original_state_number);
            arena_restore_current_state_from(ctx, original_state_number);
        }
        else
        {
            if (try_reached_the_end)
            {
                arena_restore_current_state_from(ctx, original_state_number);
            }
            else
            {
                if (catch_reached_the_end)
                {
                }
            }
        }
        arena_remove_state(ctx, original_state_number);
        arena_remove_state(ctx, ctx->throw_join_state);
        ctx->throw_join_state = throw_join_state_old;
        ctx->catch_secondary_block_opt = catch_secondary_block_old;
    }
    else _catch_label_1:
    {
    }
}

static void flow_visit_switch_statement(struct flow_visit_ctx * ctx, struct selection_statement * p_selection_statement)
{
    int old_initial_state = ctx->initial_state;
    int old_break_join_state = ctx->break_join_state;
    ctx->initial_state = arena_add_copy_of_current_state(ctx, "original");
    ctx->break_join_state = arena_add_empty_state(ctx, "break join");
    flow_visit_secondary_block(ctx, p_selection_statement->secondary_block);
    unsigned char   reached_the_end =  !secondary_block_ends_with_jump(p_selection_statement->secondary_block);
    if ( !reached_the_end)
    {
        arena_restore_current_state_from(ctx, ctx->break_join_state);
    }
    flow_exit_block_visit_defer_list(ctx, &p_selection_statement->defer_list, p_selection_statement->secondary_block->last_token);
    flow_defer_list_set_end_of_lifetime(ctx, &p_selection_statement->defer_list, p_selection_statement->secondary_block->last_token);
    arena_remove_state(ctx, ctx->initial_state);
    arena_remove_state(ctx, ctx->break_join_state);
    ctx->initial_state = old_initial_state;
    ctx->break_join_state = old_break_join_state;
}

static void flow_visit_selection_statement(struct flow_visit_ctx * ctx, struct selection_statement * p_selection_statement)
{
    if (p_selection_statement->first_token->type == 9017)
    {
        flow_visit_if_statement(ctx, p_selection_statement);
    }
    else
    {
        if (p_selection_statement->first_token->type == 9034)
        {
            flow_visit_switch_statement(ctx, p_selection_statement);
        }
        else
        {
            ;
        }
    }
}

static void flow_visit_initializer_list(struct flow_visit_ctx * ctx, struct initializer_list * p_initializer_list);

static void flow_visit_bracket_initializer_list(struct flow_visit_ctx * ctx, struct braced_initializer * p_bracket_initializer_list)
{
    if (p_bracket_initializer_list->initializer_list == ((void *)0))
    {
    }
    else
    {
        flow_visit_initializer_list(ctx, p_bracket_initializer_list->initializer_list);
    }
}

static void flow_visit_designation(struct flow_visit_ctx * ctx, struct designation * p_designation)
{
}

static void flow_visit_initializer(struct flow_visit_ctx * ctx, struct initializer * p_initializer);

static void flow_visit_initializer_list_item(struct flow_visit_ctx * ctx, struct initializer_list_item * p_initializer)
{
    if (p_initializer->designation)
    {
        flow_visit_designation(ctx, p_initializer->designation);
    }
    if (p_initializer->initializer)
    {
        flow_visit_initializer(ctx, p_initializer->initializer);
    }
}

static void flow_visit_initializer(struct flow_visit_ctx * ctx, struct initializer * p_initializer)
{
    if (p_initializer->assignment_expression)
    {
        struct true_false_set  a = {0};
        flow_visit_expression(ctx, p_initializer->assignment_expression, &a);
        true_false_set_destroy(&a);
    }
    else
    {
        if (p_initializer->braced_initializer)
        {
            flow_visit_bracket_initializer_list(ctx, p_initializer->braced_initializer);
        }
    }
}

static void flow_visit_initializer_list(struct flow_visit_ctx * ctx, struct initializer_list * p_initializer_list)
{
    struct initializer_list_item * p_initializer = p_initializer_list->head;
    while (p_initializer)
    {
        flow_visit_initializer_list_item(ctx, p_initializer);
        p_initializer = p_initializer->next;
    }
}

static void flow_visit_type_qualifier(struct flow_visit_ctx * ctx, struct type_qualifier * p_type_qualifier)
{
}

static void flow_visit_type_specifier(struct flow_visit_ctx * ctx, struct type_specifier * p_type_specifier);

static void flow_visit_specifier_qualifier(struct flow_visit_ctx * ctx, struct type_specifier_qualifier * p_specifier_qualifier)
{
    if (p_specifier_qualifier->type_specifier)
    {
        flow_visit_type_specifier(ctx, p_specifier_qualifier->type_specifier);
    }
    if (p_specifier_qualifier->type_qualifier)
    {
        flow_visit_type_qualifier(ctx, p_specifier_qualifier->type_qualifier);
    }
}

static void flow_visit_struct_or_union_specifier(struct flow_visit_ctx * ctx, struct struct_or_union_specifier * p_struct_or_union_specifier);
static void flow_visit_enum_specifier(struct flow_visit_ctx * ctx, struct enum_specifier * p_enum_specifier);

static void flow_visit_specifier_qualifier_list(struct flow_visit_ctx * ctx, struct specifier_qualifier_list * p_specifier_qualifier_list_opt, struct type * p_type)
{
    if (p_specifier_qualifier_list_opt == ((void *)0))
    {
        return;
    }
    if (p_specifier_qualifier_list_opt->struct_or_union_specifier)
    {
        flow_visit_struct_or_union_specifier(ctx, p_specifier_qualifier_list_opt->struct_or_union_specifier);
    }
    else
    {
        if (p_specifier_qualifier_list_opt->enum_specifier)
        {
            flow_visit_enum_specifier(ctx, p_specifier_qualifier_list_opt->enum_specifier);
        }
        else
        {
            if (p_specifier_qualifier_list_opt->typedef_declarator)
            {
            }
            else
            {
                struct type_specifier_qualifier * p_specifier_qualifier = p_specifier_qualifier_list_opt->head;
                while (p_specifier_qualifier)
                {
                    flow_visit_specifier_qualifier(ctx, p_specifier_qualifier);
                    p_specifier_qualifier = p_specifier_qualifier->next;
                }
            }
        }
    }
}

static void flow_visit_type_name(struct flow_visit_ctx * ctx, struct type_name * p_type_name)
{
    flow_visit_specifier_qualifier_list(ctx, p_type_name->specifier_qualifier_list, &p_type_name->type);
    flow_visit_declarator(ctx, p_type_name->abstract_declarator);
}

static void flow_visit_generic_selection(struct flow_visit_ctx * ctx, struct generic_selection * p_generic_selection)
{
    if (p_generic_selection->expression)
    {
        struct true_false_set  a = {0};
        flow_visit_expression(ctx, p_generic_selection->expression, &a);
        true_false_set_destroy(&a);
    }
    else
    {
        if (p_generic_selection->type_name)
        {
            flow_visit_type_name(ctx, p_generic_selection->type_name);
        }
    }
}

struct param_list *type_get_func_or_func_ptr_params(struct type * p_type);
void diagnostic_remove(struct diagnostic * d, int   w);
void checked_read_object(struct flow_visit_ctx * ctx, struct type * p_type, unsigned char   is_nullable, struct flow_object * p_object, struct token * position_token, struct marker * p_marker, unsigned char   check_pointed_object);
unsigned char  type_is_pointer(struct type * p_type);
void flow_object_set_unknown(struct type * p_type, unsigned char   t_is_nullable, struct flow_object * p_object, unsigned char   nullable_enabled);

static void compare_function_arguments3(struct flow_visit_ctx * ctx, struct type * p_type, struct argument_expression_list * p_argument_expression_list)
{
    if (1)
    {
        unsigned char   nullable_enabled = ctx->ctx->options.null_checks_enabled;
        struct param_list * p_param_list = type_get_func_or_func_ptr_params(p_type);
        if (p_param_list == ((void *)0))
        {
            goto _catch_label_1;
        }
        struct param * p_current_parameter_type = p_param_list->head;
        struct argument_expression * p_current_argument = p_argument_expression_list->head;
        while (p_current_argument && p_current_parameter_type)
        {
            struct true_false_set  a = {0};
            struct diagnostic  temp = ctx->ctx->options.diagnostic_stack.stack[ctx->ctx->options.diagnostic_stack.top_index];
            diagnostic_remove(&ctx->ctx->options.diagnostic_stack.stack[ctx->ctx->options.diagnostic_stack.top_index], 29);
            flow_visit_expression(ctx, p_current_argument->expression, &a);
            ctx->ctx->options.diagnostic_stack.stack[ctx->ctx->options.diagnostic_stack.top_index] = temp;
            true_false_set_destroy(&a);
            struct flow_object * p_argument_object = expression_get_flow_object(ctx, p_current_argument->expression, nullable_enabled);
            if (p_argument_object)
            {
                struct flow_object * parameter_object = make_flow_object(ctx, &p_current_parameter_type->type, ((void *)0), p_current_argument->expression);
                if (parameter_object == ((void *)0))
                {
                    goto _catch_label_1;
                }
                flow_object_set_uninitialized(&p_current_parameter_type->type, parameter_object);
                struct marker  a_marker = {0, 0, 0, 0, 0, 0, 0};
                a_marker.p_token_begin = p_current_argument->expression->first_token;
                a_marker.p_token_end = p_current_argument->expression->last_token;
                struct marker  b_marker = {0, 0, 0, 0, 0, 0, 0};
                b_marker.p_token_begin = p_current_argument->expression->first_token;
                b_marker.p_token_end = p_current_argument->expression->last_token;
                flow_check_assignment(ctx, p_current_argument->expression->first_token, &a_marker, &b_marker, 1, 1, type_is_view(&p_current_parameter_type->type), type_is_nullable(&p_current_parameter_type->type, ctx->ctx->options.null_checks_enabled), &p_current_parameter_type->type, parameter_object, &p_current_argument->expression->type, p_argument_object, &p_current_argument->set_unkown);
            }
            p_current_argument = p_current_argument->next;
            p_current_parameter_type = p_current_parameter_type->next;
        }
        while (p_current_argument)
        {
            struct flow_object * p_argument_object = expression_get_flow_object(ctx, p_current_argument->expression, nullable_enabled);
            if (p_argument_object)
            {
                struct marker  marker = {0, 0, 0, 0, 0, 0, 0};
                marker.p_token_begin = p_current_argument->expression->first_token;
                marker.p_token_end = p_current_argument->expression->last_token;
                checked_read_object(ctx, &p_current_argument->expression->type, type_is_nullable(&p_current_argument->expression->type, ctx->ctx->options.null_checks_enabled), p_argument_object, p_current_argument->expression->first_token, &marker, 0);
            }
            else
            {
            }
            p_current_argument = p_current_argument->next;
        }
        p_current_parameter_type = p_param_list->head;
        p_current_argument = p_argument_expression_list->head;
        while (p_current_argument && p_current_parameter_type)
        {
            if (p_current_argument->set_unkown && type_is_pointer(&p_current_argument->expression->type))
            {
                struct type  pointed_type = type_remove_pointer(&p_current_argument->expression->type);
                struct flow_object * p_argument_object = expression_get_flow_object(ctx, p_current_argument->expression, nullable_enabled);
                if (p_argument_object)
                {
                    unsigned char   argument_type_is_nullable = type_is_nullable(&pointed_type, ctx->ctx->options.null_checks_enabled);
                    if (p_argument_object->current.pointed)
                    {
                        flow_object_set_unknown(&pointed_type, argument_type_is_nullable, p_argument_object->current.pointed, ctx->ctx->options.null_checks_enabled);
                    }
                    else
                    {
                    }
                }
                type_destroy(&pointed_type);
            }
            p_current_argument = p_current_argument->next;
            p_current_parameter_type = p_current_parameter_type->next;
        }
        while (p_current_argument)
        {
            struct flow_object * p_argument_object = expression_get_flow_object(ctx, p_current_argument->expression, nullable_enabled);
            if (p_argument_object)
            {
            }
            else
            {
            }
            p_current_argument = p_current_argument->next;
        }
    }
    else _catch_label_1:
    {
    }
}

unsigned char  type_is_function(struct type * p_type);
unsigned char  type_is_void(struct type * p_type);
unsigned char  type_is_nullptr_t(struct type * p_type);

static void check_uninitialized(struct flow_visit_ctx * ctx, struct expression * p_expression)
{
    if (p_expression->is_assignment_expression)
    {
        return;
    }
    unsigned char   nullable_enabled = ctx->ctx->options.null_checks_enabled;
    if (type_is_function(&p_expression->type) || type_is_void(&p_expression->type) || type_is_nullptr_t(&p_expression->type))
    {
        return;
    }
    struct flow_object * p_object = expression_get_flow_object(ctx, p_expression, nullable_enabled);
    if ( !ctx->expression_is_not_evaluated)
    {
        if (p_object && p_object->current.state == 1)
        {
            if (p_expression->expression_type == 2 && p_expression->declarator && p_expression->declarator->name_opt)
            {
                compiler_diagnostic_message(29, ctx->ctx, p_expression->first_token, ((void *)0), "using a uninitialized object '%s'", p_expression->declarator->name_opt->lexeme);
            }
            else
            {
                compiler_diagnostic_message(29, ctx->ctx, p_expression->first_token, ((void *)0), "using a uninitialized object");
            }
        }
        else
        {
            if (p_object && p_object->current.state & 1)
            {
                if (ctx->ctx->options.ownership_enabled)
                {
                    if (p_expression->declarator && p_expression->declarator->name_opt)
                    {
                        compiler_diagnostic_message(29, ctx->ctx, p_expression->first_token, ((void *)0), "object '%s' can be uninitialized ", p_expression->declarator->name_opt->lexeme);
                    }
                    else
                    {
                        compiler_diagnostic_message(29, ctx->ctx, p_expression->first_token, ((void *)0), "maybe using a uninitialized object");
                    }
                }
            }
        }
    }
}

void flow_object_push_states_from(struct flow_object * p_object_from, struct flow_object * p_object_to)
{
    struct flow_object_state * it_from = p_object_from->current.next;
    while (it_from)
    {
        it_from = it_from->next;
    }
    {
        int i = 0;
        for (; i < p_object_to->members.size; i++)
        {
            flow_object_push_states_from(p_object_from, p_object_to->members.data[i]);
        }
    }
}

unsigned char  flow_object_is_null(struct flow_object * p);
unsigned char  flow_object_is_not_null(struct flow_object * p);

static void flow_check_pointer_used_as_bool(struct flow_visit_ctx * ctx, struct expression * p_expression)
{
    unsigned char   nullable_enabled = ctx->ctx->options.null_checks_enabled;
    if (type_is_pointer(&p_expression->type))
    {
        struct flow_object * p_object = expression_get_flow_object(ctx, p_expression, nullable_enabled);
        if (p_object)
        {
            struct marker  marker = {0, 0, 0, 0, 0, 0, 0};
            marker.p_token_begin = p_expression->first_token;
            marker.p_token_end = p_expression->last_token;
            if (flow_object_is_null(p_object))
            {
                compiler_diagnostic_message(27, ctx->ctx, ((void *)0), &marker, "pointer is always null");
            }
            else
            {
                if (flow_object_is_not_null(p_object))
                {
                    compiler_diagnostic_message(27, ctx->ctx, ((void *)0), &marker, "pointer is always not-null");
                }
            }
        }
    }
}

void flow_object_update_current(struct flow_object * p);

static void arena_broadcast_change(struct flow_visit_ctx * ctx, struct flow_object * p)
{
    {
        int i = 0;
        for (; i < ctx->arena.size; i++)
        {
            struct flow_object * p_obj = ctx->arena.data[i];
            if (p_obj)
            {
                {
                    int j = 0;
                    for (; j < p_obj->current.alternatives.size; j++)
                    {
                        if (p_obj->current.alternatives.data[j] == p)
                        {
                            flow_object_update_current(p_obj);
                            break;
                        }
                    }
                }
            }
        }
    }
}

unsigned char  flow_object_can_be_null(struct flow_object * p);
unsigned char  flow_object_can_be_uninitialized(struct flow_object * p);
unsigned char  flow_object_can_have_its_lifetime_ended(struct flow_object * p);
unsigned char  flow_object_is_zero(struct flow_object * p);
static void flow_visit_compound_statement(struct flow_visit_ctx * ctx, struct compound_statement * p_compound_statement);
void flow_object_swap(struct flow_object * a, struct flow_object * b);
unsigned char  flow_object_can_be_zero(struct flow_object * p);
unsigned char  object_has_constant_value(struct object * a);
signed long long object_to_signed_long_long(struct object * a);
unsigned char  flow_object_is_not_zero(struct flow_object * p);

static void flow_visit_expression(struct flow_visit_ctx * ctx, struct expression * p_expression, struct true_false_set * expr_true_false_set)
{
    true_false_set_clear(expr_true_false_set);
    unsigned char   nullable_enabled = ctx->ctx->options.null_checks_enabled;
    switch (p_expression->expression_type)
    {
        case 0 :
        ;
        break;
        case 4 :
        break;
        case 1 :
        break;
        case 2 :
        {
            struct true_false_set_item  item = {0};
            item.p_expression = p_expression;
            item.true_branch_state = 1;
            item.false_branch_state = 2;
            true_false_set_push_back(expr_true_false_set, &item);
            check_uninitialized(ctx, p_expression);
        }
        break;
        case 9 :
        ;
        flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
        break;
        case 3 :
        case 5 :
        case 8 :
        case 6 :
        break;
        case 7 :
        ;
        flow_visit_generic_selection(ctx, p_expression->generic_selection);
        break;
        case 14 :
        {
            ;
            struct true_false_set  left_set = {0};
            flow_visit_expression(ctx, p_expression->left, &left_set);
            true_false_set_destroy(&left_set);
            struct true_false_set_item  item;
            item.p_expression = p_expression;
            item.true_branch_state = 1;
            item.false_branch_state = 2;
            true_false_set_push_back(expr_true_false_set, &item);
        }
        break;
        case 15 :
        {
            ;
            struct true_false_set  left_set = {0};
            flow_visit_expression(ctx, p_expression->left, &left_set);
            true_false_set_destroy(&left_set);
            struct flow_object * p_object = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
            if (p_object != ((void *)0))
            {
                if (flow_object_can_be_null(p_object))
                {
                    if (ctx->expression_is_not_evaluated)
                    {
                    }
                    else
                    {
                        struct marker  marker = {0};
                        marker.p_token_begin = p_expression->left->first_token;
                        marker.p_token_end = p_expression->left->last_token;
                        compiler_diagnostic_message(32, ctx->ctx, ((void *)0), &marker, "pointer may be null");
                    }
                }
                else
                {
                    if (flow_object_can_be_uninitialized(p_object))
                    {
                        if (ctx->expression_is_not_evaluated)
                        {
                        }
                        else
                        {
                            compiler_diagnostic_message(32, ctx->ctx, p_expression->left->first_token, ((void *)0), "object is possibly uninitialized");
                        }
                    }
                    else
                    {
                        if (flow_object_can_have_its_lifetime_ended(p_object))
                        {
                            if (ctx->expression_is_not_evaluated)
                            {
                            }
                            else
                            {
                                compiler_diagnostic_message(30, ctx->ctx, p_expression->left->first_token, ((void *)0), "object lifetime ended");
                            }
                        }
                    }
                }
            }
            if ( !ctx->expression_is_not_evaluated)
            {
                struct flow_object * p_object2 = expression_get_flow_object(ctx, p_expression, nullable_enabled);
                if (p_object2 && flow_object_can_have_its_lifetime_ended(p_object2))
                {
                    struct marker  marker = {0, 0, 0, 0, 0, 0, 0};
                    marker.p_token_begin = p_expression->first_token;
                    marker.p_token_end = p_expression->last_token;
                    compiler_diagnostic_message(30, ctx->ctx, ((void *)0), &marker, "object lifetime ended");
                }
            }
            struct true_false_set_item  item;
            item.p_expression = p_expression;
            item.true_branch_state = 1;
            item.false_branch_state = 2;
            true_false_set_push_back(expr_true_false_set, &item);
        }
        break;
        case 16 :
        case 17 :
        {
            ;
            struct flow_object * p_object = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
            if (p_object)
            {
                if (flow_object_is_null(p_object))
                {
                    p_object->current.state = 4;
                }
                else
                {
                    if (flow_object_is_zero(p_object))
                    {
                        p_object->current.state = 64;
                    }
                }
            }
            flow_visit_expression(ctx, p_expression->left, expr_true_false_set);
        }
        break;
        case 13 :
        {
            ;
            ;
            flow_visit_expression(ctx, p_expression->left, expr_true_false_set);
            flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
            true_false_set_clear(expr_true_false_set);
            struct true_false_set_item  item;
            item.p_expression = p_expression;
            item.true_branch_state = 1;
            item.false_branch_state = 2;
            true_false_set_push_back(expr_true_false_set, &item);
        }
        break;
        case 12 :
        {
            ;
            struct true_false_set  left_local = {0};
            flow_visit_expression(ctx, p_expression->left, &left_local);
            compare_function_arguments3(ctx, &p_expression->left->type, &p_expression->argument_expression_list);
            true_false_set_destroy(&left_local);
        }
        break;
        case 10 :
        ;
        flow_visit_compound_statement(ctx, p_expression->compound_statement);
        break;
        case 11 :
        ;
        ;
        ;
        ;
        flow_visit_type_name(ctx, p_expression->type_name);
        flow_visit_bracket_initializer_list(ctx, p_expression->braced_initializer);
        struct flow_object * temp2 = make_flow_object(ctx, &p_expression->type, ((void *)0), p_expression);
        if (temp2 == ((void *)0))
        {
            return;
        }
        if (p_expression->type_name->abstract_declarator->p_object)
        {
            flow_object_swap(temp2, p_expression->type_name->abstract_declarator->p_object);
            flow_object_set_zero(&p_expression->type, p_expression->type_name->abstract_declarator->p_object);
        }
        break;
        case 24 :
        if (p_expression->right)
        {
            flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
        }
        if (p_expression->type_name)
        {
            flow_visit_type_name(ctx, p_expression->type_name);
        }
        break;
        case 25 :
        if (p_expression->right)
        {
            struct true_false_set  true_false_set4 = {0};
            unsigned char   old = ctx->inside_assert;
            ctx->inside_assert = 1;
            flow_visit_expression(ctx, p_expression->right, &true_false_set4);
            ctx->inside_assert = old;
            true_false_set_set_objects_to_true_branch(ctx, &true_false_set4, nullable_enabled);
            true_false_set_destroy(&true_false_set4);
        }
        break;
        case 18 :
        if (p_expression->right)
        {
            unsigned char   t2 = ctx->expression_is_not_evaluated;
            ctx->expression_is_not_evaluated = 1;
            flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
            ctx->expression_is_not_evaluated = t2;
        }
        if (p_expression->type_name)
        {
            flow_visit_type_name(ctx, p_expression->type_name);
        }
        break;
        case 30 :
        case 31 :
        ;
        flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
        break;
        case 28 :
        ;
        flow_check_pointer_used_as_bool(ctx, p_expression->right);
        flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
        true_false_set_invert(expr_true_false_set);
        break;
        case 19 :
        case 20 :
        case 26 :
        case 27 :
        case 29 :
        case 33 :
        {
            if (p_expression->type_name)
            {
                flow_visit_type_name(ctx, p_expression->type_name);
            }
        }
        break;
        case 32 :
        {
            ;
            struct flow_object * p_object0 = expression_get_flow_object(ctx, p_expression->right, nullable_enabled);
            if (p_object0 && p_object0->current.state == 1)
            {
                if ( !ctx->expression_is_not_evaluated)
                {
                    struct marker  marker = {0};
                    marker.p_token_begin = p_expression->right->first_token;
                    marker.p_token_end = p_expression->right->last_token;
                    compiler_diagnostic_message(29, ctx->ctx, ((void *)0), &marker, "using a uninitialized object");
                }
            }
            else
            {
                if (p_object0 && flow_object_can_be_null(p_object0))
                {
                    if ( !ctx->expression_is_not_evaluated)
                    {
                        compiler_diagnostic_message(32, ctx->ctx, p_expression->right->first_token, ((void *)0), "dereference a NULL object");
                    }
                }
            }
            struct true_false_set  local_true_false = {0};
            flow_visit_expression(ctx, p_expression->right, &local_true_false);
            true_false_set_destroy(&local_true_false);
        }
        break;
        case 53 :
        {
            ;
            ;
            struct true_false_set  left_set = {0};
            flow_visit_expression(ctx, p_expression->left, &left_set);
            true_false_set_swap(expr_true_false_set, &left_set);
            true_false_set_destroy(&left_set);
            struct true_false_set  right_set = {0};
            flow_visit_expression(ctx, p_expression->right, &right_set);
            true_false_set_destroy(&right_set);
            struct flow_object * p_right_object = expression_get_flow_object(ctx, p_expression->right, nullable_enabled);
            struct flow_object * p_dest_object = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
            if (p_dest_object == ((void *)0) || p_right_object == ((void *)0))
            {
                return;
            }
            struct marker  a_marker = {0, 0, 0, 0, 0, 0, 0};
            a_marker.p_token_begin = p_expression->left->first_token;
            a_marker.p_token_end = p_expression->left->last_token;
            struct marker  b_marker = {0, 0, 0, 0, 0, 0, 0};
            b_marker.p_token_begin = p_expression->right->first_token;
            b_marker.p_token_end = p_expression->right->last_token;
            flow_check_assignment(ctx, p_expression->left->first_token, &a_marker, &b_marker, 2, 1, type_is_view(&p_expression->left->type), type_is_nullable(&p_expression->left->type, ctx->ctx->options.null_checks_enabled), &p_expression->left->type, p_dest_object, &p_expression->right->type, p_right_object, ((void *)0));
            arena_broadcast_change(ctx, p_dest_object);
            if (expression_is_malloc(p_expression->right))
            {
                struct type  t = type_remove_pointer(&p_expression->left->type);
                struct flow_object * po = make_flow_object(ctx, &t, ((void *)0), p_expression->left);
                if (po == ((void *)0))
                {
                    type_destroy(&t);
                    return;
                }
                object_set_pointer(p_dest_object, po);
                type_destroy(&t);
                p_dest_object->current.state = 4 | 2;
            }
            else
            {
                if (expression_is_calloc(p_expression->right))
                {
                    struct type  t = type_remove_pointer(&p_expression->left->type);
                    struct flow_object * po = make_flow_object(ctx, &t, ((void *)0), p_expression->left);
                    if (po == ((void *)0))
                    {
                        type_destroy(&t);
                        return;
                    }
                    flow_object_set_zero(&t, po);
                    object_set_pointer(p_dest_object, po);
                    type_destroy(&t);
                    p_dest_object->current.state = 4 | 2;
                }
            }
        }
        break;
        case 36 :
        {
            ;
            ;
            struct true_false_set  left_set = {0};
            struct true_false_set  right_set = {0};
            flow_visit_expression(ctx, p_expression->left, &left_set);
            struct flow_object * p_object = expression_get_flow_object(ctx, p_expression->right, ctx->ctx->options.null_checks_enabled);
            if (p_object)
            {
                if (flow_object_can_be_zero(p_object))
                {
                    compiler_diagnostic_message(35, ctx->ctx, p_expression->right->first_token, ((void *)0), "possible division by zero");
                }
            }
            flow_visit_expression(ctx, p_expression->right, &right_set);
            true_false_set_merge(expr_true_false_set, &left_set, &right_set, 1 | 4, 1 | 4);
            true_false_set_destroy(&left_set);
            true_false_set_destroy(&right_set);
        }
        break;
        case 34 :
        case 35 :
        case 37 :
        case 38 :
        case 39 :
        case 40 :
        case 41 :
        {
            if (p_expression->left)
            {
                struct true_false_set  left_set = {0};
                flow_visit_expression(ctx, p_expression->left, &left_set);
                true_false_set_destroy(&left_set);
            }
            if (p_expression->right)
            {
                struct true_false_set  right_set = {0};
                flow_visit_expression(ctx, p_expression->right, &right_set);
                true_false_set_destroy(&right_set);
            }
        }
        break;
        case 44 :
        case 45 :
        case 42 :
        case 43 :
        {
            ;
            ;
            unsigned char   left_is_constant = object_has_constant_value(&p_expression->left->object);
            unsigned char   right_is_constant = object_has_constant_value(&p_expression->right->object);
            if (left_is_constant)
            {
                long long left_value = object_to_signed_long_long(&p_expression->left->object);
                struct true_false_set  true_false_set_right = {0};
                flow_visit_expression(ctx, p_expression->right, &true_false_set_right);
                if (left_value == 0)
                {
                    true_false_set_swap(expr_true_false_set, &true_false_set_right);
                    {
                        int i = 0;
                        for (; i < expr_true_false_set->size; i++)
                        {
                            struct true_false_set_item * item = &expr_true_false_set->data[i];
                            item->false_branch_state = item->true_branch_state;
                        }
                    }
                    if (p_expression->expression_type == 44 || p_expression->expression_type == 45)
                    {
                        true_false_set_invert(expr_true_false_set);
                    }
                }
                true_false_set_destroy(&true_false_set_right);
            }
            else
            {
                if (right_is_constant)
                {
                    long long right_value = object_to_signed_long_long(&p_expression->right->object);
                    struct true_false_set  true_false_set_left3 = {0};
                    flow_visit_expression(ctx, p_expression->left, &true_false_set_left3);
                    if (right_value == 0)
                    {
                        true_false_set_swap(expr_true_false_set, &true_false_set_left3);
                        {
                            int i = 0;
                            for (; i < expr_true_false_set->size; i++)
                            {
                                struct true_false_set_item * item = &expr_true_false_set->data[i];
                                item->false_branch_state = item->true_branch_state;
                            }
                        }
                        if (p_expression->expression_type == 44 || p_expression->expression_type == 45)
                        {
                            true_false_set_invert(expr_true_false_set);
                        }
                    }
                    true_false_set_destroy(&true_false_set_left3);
                }
                else
                {
                    struct true_false_set  true_false_set = {0};
                    flow_visit_expression(ctx, p_expression->left, &true_false_set);
                    flow_visit_expression(ctx, p_expression->right, &true_false_set);
                    true_false_set_destroy(&true_false_set);
                }
            }
        }
        break;
        case 47 :
        case 46 :
        {
            ;
            ;
            long long value = 0;
            struct expression * p_comp_expression = ((void *)0);
            if (object_has_constant_value(&p_expression->left->object) &&  !object_has_constant_value(&p_expression->right->object))
            {
                value = object_to_signed_long_long(&p_expression->left->object);
                p_comp_expression = p_expression->right;
            }
            else
            {
                if (object_has_constant_value(&p_expression->right->object) &&  !object_has_constant_value(&p_expression->left->object))
                {
                    value = object_to_signed_long_long(&p_expression->right->object);
                    p_comp_expression = p_expression->left;
                }
            }
            if (p_comp_expression)
            {
                struct true_false_set  true_false_set = {0};
                flow_visit_expression(ctx, p_comp_expression, &true_false_set);
                struct flow_object * p_object = expression_get_flow_object(ctx, p_comp_expression, nullable_enabled);
                if (p_object)
                {
                    struct marker  marker = {0, 0, 0, 0, 0, 0, 0};
                    marker.p_token_begin = p_comp_expression->first_token;
                    marker.p_token_end = p_comp_expression->last_token;
                    if ((flow_object_is_null(p_object) || flow_object_is_zero(p_object)) && value == 0)
                    {
                        if (p_expression->expression_type == 46)
                        {
                            if (ctx->inside_assert)
                            {
                            }
                            else
                            {
                                if (type_is_pointer(&p_comp_expression->type))
                                {
                                    compiler_diagnostic_message(27, ctx->ctx, ((void *)0), &marker, "pointer is always null");
                                }
                                else
                                {
                                    compiler_diagnostic_message(27, ctx->ctx, ((void *)0), &marker, "value is always zero");
                                }
                            }
                        }
                        else
                        {
                            if (p_expression->expression_type == 47)
                            {
                                if (type_is_pointer(&p_comp_expression->type))
                                {
                                    compiler_diagnostic_message(27, ctx->ctx, ((void *)0), &marker, "pointer is always null");
                                }
                                else
                                {
                                    compiler_diagnostic_message(27, ctx->ctx, ((void *)0), &marker, "value is always zero");
                                }
                            }
                        }
                    }
                    else
                    {
                        if ((flow_object_is_not_null(p_object) || flow_object_is_not_zero(p_object)) && value == 0)
                        {
                            if (p_expression->expression_type == 46)
                            {
                                if (type_is_pointer(&p_comp_expression->type))
                                {
                                    compiler_diagnostic_message(27, ctx->ctx, ((void *)0), &marker, "pointer is always non-null");
                                }
                                else
                                {
                                    compiler_diagnostic_message(27, ctx->ctx, ((void *)0), &marker, "value is always non-zero");
                                }
                            }
                            else
                            {
                                if (p_expression->expression_type == 47)
                                {
                                    if (ctx->inside_assert)
                                    {
                                    }
                                    else
                                    {
                                        if (type_is_pointer(&p_comp_expression->type))
                                        {
                                            compiler_diagnostic_message(27, ctx->ctx, ((void *)0), &marker, "pointer is always non-null");
                                        }
                                        else
                                        {
                                            compiler_diagnostic_message(27, ctx->ctx, ((void *)0), &marker, "value is always non-zero");
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                true_false_set_swap(expr_true_false_set, &true_false_set);
                if (p_expression->expression_type == 46 && value == 0)
                {
                    true_false_set_invert(expr_true_false_set);
                }
                else
                {
                    if (p_expression->expression_type == 47 && value != 0)
                    {
                        true_false_set_invert(expr_true_false_set);
                    }
                }
                true_false_set_destroy(&true_false_set);
            }
            else
            {
                struct true_false_set  true_false_set = {0};
                flow_visit_expression(ctx, p_expression->left, &true_false_set);
                flow_visit_expression(ctx, p_expression->right, &true_false_set);
                true_false_set_destroy(&true_false_set);
            }
        }
        break;
        case 51 :
        {
            ;
            ;
            flow_check_pointer_used_as_bool(ctx, p_expression->left);
            flow_check_pointer_used_as_bool(ctx, p_expression->right);
            int original_state_number = arena_add_copy_of_current_state(ctx, "original");
            struct true_false_set  left_set = {0};
            flow_visit_expression(ctx, p_expression->left, &left_set);
            true_false_set_set_objects_to_false_branch(ctx, &left_set, nullable_enabled);
            struct true_false_set  right_set = {0};
            flow_visit_expression(ctx, p_expression->right, &right_set);
            {
                int i = 0;
                for (; i < left_set.size; i++)
                {
                    struct true_false_set_item  item5;
                    item5.p_expression = left_set.data[i].p_expression;
                    item5.true_branch_state = (left_set.data[i].true_branch_state | left_set.data[i].false_branch_state);
                    item5.false_branch_state = left_set.data[i].false_branch_state;
                    true_false_set_push_back(expr_true_false_set, &item5);
                }
            }
            {
                int k = 0;
                for (; k < right_set.size; k++)
                {
                    int index = find_item_index_by_expression(expr_true_false_set, right_set.data[k].p_expression);
                    if (index ==  -1)
                    {
                        index = expr_true_false_set->size;
                        struct true_false_set_item  item4 = {0};
                        true_false_set_push_back(expr_true_false_set, &item4);
                    }
                    expr_true_false_set->data[index].p_expression = right_set.data[k].p_expression;
                    expr_true_false_set->data[index].false_branch_state = right_set.data[k].false_branch_state;
                    expr_true_false_set->data[index].true_branch_state = (1 | 2);
                }
            }
            arena_restore_current_state_from(ctx, original_state_number);
            arena_remove_state(ctx, original_state_number);
            true_false_set_destroy(&left_set);
            true_false_set_destroy(&right_set);
        }
        break;
        case 52 :
        {
            ;
            ;
            flow_check_pointer_used_as_bool(ctx, p_expression->left);
            flow_check_pointer_used_as_bool(ctx, p_expression->right);
            int original_state_number = arena_add_copy_of_current_state(ctx, "original");
            struct true_false_set  left_set = {0};
            flow_visit_expression(ctx, p_expression->left, &left_set);
            true_false_set_set_objects_to_true_branch(ctx, &left_set, nullable_enabled);
            struct true_false_set  right_set = {0};
            flow_visit_expression(ctx, p_expression->right, &right_set);
            {
                int i = 0;
                for (; i < left_set.size; i++)
                {
                    struct true_false_set_item * p_item_left = &left_set.data[i];
                    struct true_false_set_item  left_and_right = {0};
                    left_and_right.p_expression = p_item_left->p_expression;
                    left_and_right.true_branch_state = p_item_left->true_branch_state;
                    left_and_right.false_branch_state = p_item_left->true_branch_state | p_item_left->false_branch_state;
                    true_false_set_push_back(expr_true_false_set, &left_and_right);
                }
            }
            {
                int k = 0;
                for (; k < right_set.size; k++)
                {
                    struct true_false_set_item * p_item_right = &right_set.data[k];
                    int index = find_item_index_by_expression(expr_true_false_set, p_item_right->p_expression);
                    if (index ==  -1)
                    {
                        index = expr_true_false_set->size;
                        struct true_false_set_item  item2 = {0};
                        true_false_set_push_back(expr_true_false_set, &item2);
                    }
                    expr_true_false_set->data[index].p_expression = p_item_right->p_expression;
                    expr_true_false_set->data[index].true_branch_state = p_item_right->true_branch_state;
                    expr_true_false_set->data[index].false_branch_state = p_item_right->false_branch_state;
                    expr_true_false_set->data[index].false_branch_state = (1 | 2);
                }
            }
            arena_restore_current_state_from(ctx, original_state_number);
            arena_remove_state(ctx, original_state_number);
            true_false_set_destroy(&left_set);
            true_false_set_destroy(&right_set);
        }
        break;
        case 50 :
        {
            ;
            ;
            struct true_false_set  true_false_set = {0};
            flow_visit_expression(ctx, p_expression->left, &true_false_set);
            flow_visit_expression(ctx, p_expression->right, &true_false_set);
            true_false_set_destroy(&true_false_set);
        }
        break;
        case 48 :
        case 49 :
        ;
        ;
        flow_visit_expression(ctx, p_expression->left, expr_true_false_set);
        flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
        break;
        case 21 :
        {
        }
        break;
        case 22 :
        break;
        case 23 :
        break;
        case 55 :
        {
            ;
            ;
            ;
            struct true_false_set  true_false_set = {0};
            flow_check_pointer_used_as_bool(ctx, p_expression->condition_expr);
            flow_visit_expression(ctx, p_expression->condition_expr, &true_false_set);
            int before_if_state_number = arena_add_copy_of_current_state(ctx, "before-if");
            true_false_set_set_objects_to_true_branch(ctx, &true_false_set, nullable_enabled);
            struct true_false_set  set = {0};
            flow_visit_expression(ctx, p_expression->left, &set);
            true_false_set_destroy(&set);
            int left_true_branch_state_number = arena_add_copy_of_current_state(ctx, "left-true-branch");
            arena_restore_current_state_from(ctx, before_if_state_number);
            true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);
            struct true_false_set  set2 = {0};
            flow_visit_expression(ctx, p_expression->right, &set2);
            true_false_set_destroy(&set2);
            arena_merge_current_state_with_state_number(ctx, left_true_branch_state_number);
            arena_restore_current_state_from(ctx, left_true_branch_state_number);
            arena_remove_state(ctx, before_if_state_number);
            arena_remove_state(ctx, left_true_branch_state_number);
            true_false_set_destroy(&true_false_set);
        }
        break;
    }
}

static void flow_visit_expression_statement(struct flow_visit_ctx * ctx, struct expression_statement * p_expression_statement)
{
    struct true_false_set  d = {0};
    if (p_expression_statement->expression_opt)
    {
        flow_visit_expression(ctx, p_expression_statement->expression_opt, &d);
    }
    true_false_set_destroy(&d);
}

static void flow_visit_block_item_list(struct flow_visit_ctx * ctx, struct block_item_list * p_block_item_list);

static void flow_visit_compound_statement(struct flow_visit_ctx * ctx, struct compound_statement * p_compound_statement)
{
    flow_visit_block_item_list(ctx, &p_compound_statement->block_item_list);
    flow_exit_block_visit_defer_list(ctx, &p_compound_statement->defer_list, p_compound_statement->last_token);
    flow_defer_list_set_end_of_lifetime(ctx, &p_compound_statement->defer_list, p_compound_statement->last_token);
}

static void flow_visit_do_while_statement(struct flow_visit_ctx * ctx, struct iteration_statement * p_iteration_statement)
{
    ;
    unsigned char   nullable_enabled = ctx->ctx->options.null_checks_enabled;
    struct true_false_set  true_false_set = {0};
    if (1)
    {
        if (p_iteration_statement->expression1)
        {
            flow_visit_expression(ctx, p_iteration_statement->expression1, &true_false_set);
        }
        flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
        flow_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);
        flow_defer_list_set_end_of_lifetime(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);
        unsigned char   was_last_statement_inside_true_branch_return = secondary_block_ends_with_jump(p_iteration_statement->secondary_block);
        if (was_last_statement_inside_true_branch_return)
        {
        }
        else
        {
            true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);
        }
    }
    else _catch_label_1:
    {
    }
    true_false_set_destroy(&true_false_set);
}

int diagnostic_stack_push_empty(struct diagnostic_stack * diagnostic_stack);
void diagnostic_stack_pop(struct diagnostic_stack * diagnostic_stack);

static void flow_visit_while_statement(struct flow_visit_ctx * ctx, struct iteration_statement * p_iteration_statement)
{
    ;
    if (p_iteration_statement->expression1 == ((void *)0))
    {
        return;
    }
    unsigned char   nullable_enabled = ctx->ctx->options.null_checks_enabled;
    int old_initial_state = ctx->initial_state;
    int old_break_join_state = ctx->break_join_state;
    ctx->initial_state = arena_add_copy_of_current_state(ctx, "original");
    ctx->break_join_state = arena_add_empty_state(ctx, "break join");
    struct true_false_set  true_false_set = {0};
    diagnostic_stack_push_empty(&ctx->ctx->options.diagnostic_stack);
    flow_visit_expression(ctx, p_iteration_statement->expression1, &true_false_set);
    true_false_set_set_objects_to_true_branch(ctx, &true_false_set, nullable_enabled);
    flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
    diagnostic_stack_pop(&ctx->ctx->options.diagnostic_stack);
    struct true_false_set  true_false_set2 = {0};
    flow_visit_expression(ctx, p_iteration_statement->expression1, &true_false_set2);
    true_false_set_destroy(&true_false_set2);
    true_false_set_set_objects_to_true_branch(ctx, &true_false_set, nullable_enabled);
    flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
    flow_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);
    unsigned char   was_last_statement_inside_true_branch_return = secondary_block_ends_with_jump(p_iteration_statement->secondary_block);
    if (was_last_statement_inside_true_branch_return)
    {
        arena_restore_current_state_from(ctx, ctx->initial_state);
        true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);
    }
    else
    {
        true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);
        arena_merge_current_state_with_state_number(ctx, ctx->break_join_state);
        arena_restore_current_state_from(ctx, ctx->break_join_state);
    }
    flow_defer_list_set_end_of_lifetime(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);
    arena_remove_state(ctx, ctx->initial_state);
    arena_remove_state(ctx, ctx->break_join_state);
    ctx->initial_state = old_initial_state;
    ctx->break_join_state = old_break_join_state;
    true_false_set_destroy(&true_false_set);
}

static void flow_visit_for_statement(struct flow_visit_ctx * ctx, struct iteration_statement * p_iteration_statement)
{
    ;
    struct true_false_set  d = {0};
    if (1)
    {
        if (p_iteration_statement->declaration && p_iteration_statement->declaration->init_declarator_list.head)
        {
            flow_visit_init_declarator_list(ctx, &p_iteration_statement->declaration->init_declarator_list);
        }
        if (p_iteration_statement->expression0)
        {
            flow_visit_expression(ctx, p_iteration_statement->expression0, &d);
        }
        if (p_iteration_statement->expression1)
        {
            flow_check_pointer_used_as_bool(ctx, p_iteration_statement->expression1);
            flow_visit_expression(ctx, p_iteration_statement->expression1, &d);
        }
        diagnostic_stack_push_empty(&ctx->ctx->options.diagnostic_stack);
        flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
        diagnostic_stack_pop(&ctx->ctx->options.diagnostic_stack);
        if (p_iteration_statement->expression2)
        {
            flow_visit_expression(ctx, p_iteration_statement->expression2, &d);
        }
        unsigned char   b_secondary_block_ends_with_jump = secondary_block_ends_with_jump(p_iteration_statement->secondary_block);
        if ( !b_secondary_block_ends_with_jump)
        {
            flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
            flow_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);
            flow_defer_list_set_end_of_lifetime(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);
        }
    }
    else _catch_label_1:
    {
    }
    true_false_set_destroy(&d);
}

static void flow_visit_iteration_statement(struct flow_visit_ctx * ctx, struct iteration_statement * p_iteration_statement)
{
    switch (p_iteration_statement->first_token->type)
    {
        case 9042 :
        flow_visit_while_statement(ctx, p_iteration_statement);
        break;
        case 9008 :
        flow_visit_do_while_statement(ctx, p_iteration_statement);
        break;
        case 9015 :
        flow_visit_for_statement(ctx, p_iteration_statement);
        break;
        default:
        ;
        break;
    }
}

static void flow_visit_jump_statement(struct flow_visit_ctx * ctx, struct jump_statement * p_jump_statement)
{
    unsigned char   nullable_enabled = ctx->ctx->options.null_checks_enabled;
    if (1)
    {
        if (p_jump_statement->first_token->type == 9037)
        {
            arena_merge_current_state_with_state_number(ctx, ctx->throw_join_state);
            flow_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
        }
        else
        {
            if (p_jump_statement->first_token->type == 9027)
            {
                if (ctx->p_return_type == ((void *)0))
                {
                    goto _catch_label_1;
                }
                if (p_jump_statement->expression_opt)
                {
                    struct true_false_set  d = {0};
                    flow_visit_expression(ctx, p_jump_statement->expression_opt, &d);
                    true_false_set_destroy(&d);
                }
                if (p_jump_statement->expression_opt)
                {
                    struct flow_object * p_object = expression_get_flow_object(ctx, p_jump_statement->expression_opt, nullable_enabled);
                    if (p_object)
                    {
                        ;
                        struct flow_object * p_dest_object = make_flow_object(ctx, ctx->p_return_type, ((void *)0), p_jump_statement->expression_opt);
                        if (p_dest_object == ((void *)0))
                        {
                            goto _catch_label_1;
                        }
                        ;
                        flow_object_set_zero(ctx->p_return_type, p_dest_object);
                        struct marker  a_marker = {0, 0, 0, 0, 0, 0, 0};
                        a_marker.p_token_begin = p_jump_statement->expression_opt->first_token;
                        a_marker.p_token_end = p_jump_statement->expression_opt->last_token;
                        struct marker  b_marker = {0, 0, 0, 0, 0, 0, 0};
                        b_marker.p_token_begin = p_jump_statement->expression_opt->first_token;
                        b_marker.p_token_end = p_jump_statement->expression_opt->last_token;
                        ;
                        flow_check_assignment(ctx, p_jump_statement->expression_opt->first_token, &a_marker, &b_marker, 0, 1, type_is_view(ctx->p_return_type), type_is_nullable(ctx->p_return_type, ctx->ctx->options.null_checks_enabled), ctx->p_return_type, p_dest_object, &p_jump_statement->expression_opt->type, p_object, ((void *)0));
                        int state_before_return = arena_add_copy_of_current_state(ctx, "before-return");
                        flow_defer_list_set_end_of_lifetime(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
                        checked_read_object(ctx, ctx->p_return_type, type_is_nullable(ctx->p_return_type, ctx->ctx->options.null_checks_enabled), p_dest_object, ((void *)0), &a_marker, 1);
                        arena_restore_current_state_from(ctx, state_before_return);
                    }
                    if (p_object && p_object->is_temporary)
                    {
                        p_object->current.state = 128;
                    }
                }
                flow_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
            }
            else
            {
                if (p_jump_statement->first_token->type == 9005)
                {
                    flow_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
                }
                else
                {
                    if (p_jump_statement->first_token->type == 9000)
                    {
                        arena_merge_current_state_with_state_number(ctx, ctx->break_join_state);
                        flow_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
                    }
                    else
                    {
                        if (p_jump_statement->first_token->type == 9016)
                        {
                            ;
                            int label_state_number =  -1;
                            {
                                int i = 0;
                                for (; i < ctx->labels_size; i++)
                                {
                                    if (ctx->labels[i].state_number == label_state_number)
                                    {
                                        break;
                                    }
                                }
                            }
                            if (label_state_number ==  -1)
                            {
                                label_state_number = arena_add_empty_state(ctx, p_jump_statement->label->lexeme);
                                ctx->labels[ctx->labels_size].state_number = label_state_number;
                                ctx->labels[ctx->labels_size].label_name = p_jump_statement->label->lexeme;
                                ctx->labels_size++;
                            }
                            arena_merge_current_state_with_state_number(ctx, label_state_number);
                            flow_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
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
    else _catch_label_1:
    {
    }
}

static void flow_visit_label(struct flow_visit_ctx * ctx, struct label * p_label);

static void flow_visit_labeled_statement(struct flow_visit_ctx * ctx, struct labeled_statement * p_labeled_statement)
{
    flow_visit_label(ctx, p_labeled_statement->label);
    flow_visit_statement(ctx, p_labeled_statement->statement);
}

static void flow_visit_primary_block(struct flow_visit_ctx * ctx, struct primary_block * p_primary_block)
{
    if (p_primary_block->defer_statement)
    {
        flow_visit_defer_statement(ctx, p_primary_block->defer_statement);
    }
    else
    {
        if (p_primary_block->compound_statement)
        {
            flow_visit_compound_statement(ctx, p_primary_block->compound_statement);
        }
        else
        {
            if (p_primary_block->iteration_statement)
            {
                flow_visit_iteration_statement(ctx, p_primary_block->iteration_statement);
            }
            else
            {
                if (p_primary_block->selection_statement)
                {
                    flow_visit_selection_statement(ctx, p_primary_block->selection_statement);
                }
                else
                {
                    if (p_primary_block->try_statement)
                    {
                        flow_visit_try_statement(ctx, p_primary_block->try_statement);
                    }
                }
            }
        }
    }
}

static void flow_visit_unlabeled_statement(struct flow_visit_ctx * ctx, struct unlabeled_statement * p_unlabeled_statement)
{
    if (p_unlabeled_statement->primary_block)
    {
        flow_visit_primary_block(ctx, p_unlabeled_statement->primary_block);
    }
    else
    {
        if (p_unlabeled_statement->expression_statement)
        {
            flow_visit_expression_statement(ctx, p_unlabeled_statement->expression_statement);
        }
        else
        {
            if (p_unlabeled_statement->jump_statement)
            {
                flow_visit_jump_statement(ctx, p_unlabeled_statement->jump_statement);
            }
            else
            {
                ;
            }
        }
    }
}

static void flow_visit_statement(struct flow_visit_ctx * ctx, struct statement * p_statement)
{
    if (p_statement->labeled_statement)
    {
        flow_visit_labeled_statement(ctx, p_statement->labeled_statement);
    }
    else
    {
        if (p_statement->unlabeled_statement)
        {
            flow_visit_unlabeled_statement(ctx, p_statement->unlabeled_statement);
        }
    }
}

int strcmp(char * _Str1, char * _Str2);

static void flow_visit_label(struct flow_visit_ctx * ctx, struct label * p_label)
{
    if (p_label->p_identifier_opt)
    {
        {
            int i = 0;
            for (; i < ctx->labels_size; i++)
            {
                if (strcmp(ctx->labels[i].label_name, p_label->p_identifier_opt->lexeme) == 0)
                {
                    arena_restore_current_state_from(ctx, ctx->labels[i].state_number);
                    break;
                }
            }
        }
    }
    else
    {
        arena_restore_current_state_from(ctx, ctx->initial_state);
    }
}

void flow_visit_declaration(struct flow_visit_ctx * ctx, struct declaration * p_declaration);

static void flow_visit_block_item(struct flow_visit_ctx * ctx, struct block_item * p_block_item)
{
    if (p_block_item->declaration)
    {
        flow_visit_declaration(ctx, p_block_item->declaration);
    }
    else
    {
        if (p_block_item->unlabeled_statement)
        {
            flow_visit_unlabeled_statement(ctx, p_block_item->unlabeled_statement);
        }
        else
        {
            if (p_block_item->label)
            {
                flow_visit_label(ctx, p_block_item->label);
            }
        }
    }
}

static void flow_visit_block_item_list(struct flow_visit_ctx * ctx, struct block_item_list * p_block_item_list)
{
    struct block_item * p_block_item = p_block_item_list->head;
    while (p_block_item)
    {
        flow_visit_block_item(ctx, p_block_item);
        p_block_item = p_block_item->next;
    }
}

int isalpha(int _C);
int strncmp(char * _Str1, char * _Str2, unsigned int _MaxCount);

static int  parse_string_state(char * s, unsigned char  * invalid)
{
     *invalid = 0;
    int   e = 0;
    char * p = s;
    while ( *p)
    {
        if (isalpha( *p))
        {
            int sz = 0;
            char * start = p;
            while (isalpha( *p) ||  *p == 45)
            {
                sz++;
                p++;
            }
            if (strncmp(start, "moved", sz) == 0)
            {
                e = 8;
            }
            else
            {
                if (strncmp(start, "null", sz) == 0)
                {
                    e = 2;
                }
                else
                {
                    if (strncmp(start, "not-null", sz) == 0)
                    {
                        e = 4;
                    }
                    else
                    {
                        if (strncmp(start, "maybe-null", sz) == 0)
                        {
                            e = (4 | 2);
                        }
                        else
                        {
                            if (strncmp(start, "uninitialized", sz) == 0)
                            {
                                e = 1;
                            }
                            else
                            {
                                if (strncmp(start, "zero", sz) == 0)
                                {
                                    e = 32;
                                }
                                else
                                {
                                    if (strncmp(start, "not-zero", sz) == 0)
                                    {
                                        e = 64;
                                    }
                                    else
                                    {
                                        if (strncmp(start, "any", sz) == 0)
                                        {
                                            e = (64 | 32);
                                        }
                                        else
                                        {
                                             *invalid = 1;
                                            return 0;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            p++;
        }
    }
    return e;
}

void execute_pragma(struct parser_ctx * ctx, struct pragma_declaration * p_pragma, unsigned char   on_flow_analysis);

static void flow_visit_pragma_declaration(struct flow_visit_ctx * ctx, struct pragma_declaration * p_pragma_declaration)
{
    execute_pragma(ctx->ctx, p_pragma_declaration, 1);
}

void print_flow_object(struct type * p_type, struct flow_object * p_object, unsigned char   short_version);
void flow_object_print_state(struct flow_object * p);

static void flow_visit_static_assert_declaration(struct flow_visit_ctx * ctx, struct static_assert_declaration * p_static_assert_declaration)
{
    unsigned char   t2 = ctx->expression_is_not_evaluated;
    ctx->expression_is_not_evaluated = 1;
    unsigned char   nullable_enabled = ctx->ctx->options.null_checks_enabled;
    struct true_false_set  a = {0};
    flow_visit_expression(ctx, p_static_assert_declaration->constant_expression, &a);
    ctx->expression_is_not_evaluated = t2;
    if (p_static_assert_declaration->first_token->type == 9069 || p_static_assert_declaration->first_token->type == 9070)
    {
        unsigned char   ex = p_static_assert_declaration->first_token->type == 9070;
        compiler_diagnostic_message(62, ctx->ctx, p_static_assert_declaration->first_token, ((void *)0), "static_debug");
        struct flow_object * p_obj = expression_get_flow_object(ctx, p_static_assert_declaration->constant_expression, nullable_enabled);
        if (p_obj)
        {
            print_flow_object(&p_static_assert_declaration->constant_expression->type, p_obj,  !ex);
            if (p_obj->is_temporary)
            {
                p_obj->current.state = 128;
            }
        }
        if (ex)
        {
            print_arena(ctx);
        }
    }
    else
    {
        if (p_static_assert_declaration->first_token->type == 9071)
        {
            unsigned char   is_invalid = 0;
            int   e = 0;
            if (p_static_assert_declaration->string_literal_opt)
            {
                e = parse_string_state(p_static_assert_declaration->string_literal_opt->lexeme, &is_invalid);
            }
            if (is_invalid)
            {
                compiler_diagnostic_message(1080, ctx->ctx, p_static_assert_declaration->first_token, ((void *)0), "invalid parameter %s", p_static_assert_declaration->string_literal_opt->lexeme);
            }
            else
            {
                struct flow_object * p_obj = expression_get_flow_object(ctx, p_static_assert_declaration->constant_expression, nullable_enabled);
                if (p_obj)
                {
                    if (e != p_obj->current.state)
                    {
                        compiler_diagnostic_message(1080, ctx->ctx, p_static_assert_declaration->first_token, ((void *)0), "static_state failed");
                        if (p_static_assert_declaration->string_literal_opt)
                        {
                            printf("expected :%s\n", p_static_assert_declaration->string_literal_opt->lexeme);
                        }
                        printf("current  :");
                        flow_object_print_state(p_obj);
                        printf("\n");
                    }
                }
                else
                {
                    if (e != 0)
                    {
                        compiler_diagnostic_message(1080, ctx->ctx, p_static_assert_declaration->first_token, ((void *)0), "static_state failed");
                    }
                }
                if (p_obj && p_obj->is_temporary)
                {
                    p_obj->current.state = 128;
                }
            }
        }
        else
        {
            if (p_static_assert_declaration->first_token->type == 9072)
            {
                struct flow_object * p_obj = expression_get_flow_object(ctx, p_static_assert_declaration->constant_expression, nullable_enabled);
                if (p_obj)
                {
                    if (p_static_assert_declaration->string_literal_opt)
                    {
                        char * lexeme = p_static_assert_declaration->string_literal_opt->lexeme;
                        if (strcmp(lexeme, "\"zero\"") == 0)
                        {
                            flow_object_set_zero(&p_static_assert_declaration->constant_expression->type, p_obj);
                        }
                        else
                        {
                            unsigned char   is_invalid = 0;
                            int   e = parse_string_state(p_static_assert_declaration->string_literal_opt->lexeme, &is_invalid);
                            if ( !is_invalid)
                            {
                                if (p_obj->members.size > 0)
                                {
                                    compiler_diagnostic_message(1070, ctx->ctx, p_static_assert_declaration->first_token, ((void *)0), "use only for non agregates");
                                }
                                p_obj->current.state = e;
                            }
                            else
                            {
                                compiler_diagnostic_message(1070, ctx->ctx, p_static_assert_declaration->first_token, ((void *)0), "invalid parameter %s", p_static_assert_declaration->string_literal_opt->lexeme);
                            }
                        }
                    }
                }
                if (p_obj && p_obj->is_temporary)
                {
                    p_obj->current.state = 128;
                }
            }
        }
    }
    true_false_set_destroy(&a);
}

static void flow_visit_attribute_specifier_sequence(struct flow_visit_ctx * ctx, struct attribute_specifier_sequence * p_visit_attribute_specifier_sequence);

static void flow_visit_direct_declarator(struct flow_visit_ctx * ctx, struct direct_declarator * p_direct_declarator)
{
    if (p_direct_declarator->function_declarator)
    {
        struct parameter_declaration * parameter = ((void *)0);
        if (p_direct_declarator->function_declarator->parameter_type_list_opt && p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list)
        {
            parameter = p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head;
        }
        ctx->parameter_list++;
        while (parameter)
        {
            if (parameter->attribute_specifier_sequence_opt)
            {
                flow_visit_attribute_specifier_sequence(ctx, parameter->attribute_specifier_sequence_opt);
            }
            flow_visit_declaration_specifiers(ctx, parameter->declaration_specifiers, &parameter->declarator->type);
            if (parameter->declarator)
            {
                flow_visit_declarator(ctx, parameter->declarator);
            }
            parameter = parameter->next;
        }
        ctx->parameter_list--;
    }
    else
    {
        if (p_direct_declarator->array_declarator)
        {
            if (p_direct_declarator->array_declarator->assignment_expression)
            {
                struct true_false_set  a = {0};
                flow_visit_expression(ctx, p_direct_declarator->array_declarator->assignment_expression, &a);
                true_false_set_destroy(&a);
            }
        }
    }
}

unsigned char  type_is_pointer_to_out(struct type * p_type);
unsigned char  type_is_any_owner(struct type * p_type);
unsigned char  type_is_struct_or_union(struct type * p_type);
unsigned char  type_is_array(struct type * p_type);

static void flow_visit_declarator(struct flow_visit_ctx * ctx, struct declarator * p_declarator)
{
    unsigned char   nullable_enabled = ctx->ctx->options.null_checks_enabled;
    if (1)
    {
        if (p_declarator->type.category != 1)
        {
            if (ctx->parameter_list > 1)
            {
                return;
            }
            p_declarator->p_object = make_flow_object(ctx, &p_declarator->type, p_declarator, ((void *)0));
            if (p_declarator->p_object == ((void *)0))
            {
                goto _catch_label_1;
            }
            flow_object_set_uninitialized(&p_declarator->type, p_declarator->p_object);
            if (p_declarator->declaration_specifiers && p_declarator->declaration_specifiers->storage_class_specifier_flags & 2048)
            {
                if (type_is_pointer(&p_declarator->type))
                {
                    if (type_is_nullable(&p_declarator->type, ctx->ctx->options.null_checks_enabled))
                    {
                        p_declarator->p_object->current.state = 4 | 2;
                    }
                    else
                    {
                        p_declarator->p_object->current.state = 4;
                    }
                    if (type_is_pointer_to_out(&p_declarator->type))
                    {
                        struct type  t = type_remove_pointer(&p_declarator->type);
                        struct flow_object * po = make_flow_object(ctx, &t, p_declarator, ((void *)0));
                        if (po == ((void *)0))
                        {
                            type_destroy(&t);
                            goto _catch_label_1;
                        }
                        flow_object_set_uninitialized(&t, po);
                        object_set_pointer(p_declarator->p_object, po);
                        type_destroy(&t);
                    }
                    else
                    {
                        if (type_is_any_owner(&p_declarator->type))
                        {
                            struct type  t = type_remove_pointer(&p_declarator->type);
                            struct flow_object * po = make_flow_object(ctx, &t, p_declarator, ((void *)0));
                            if (po == ((void *)0))
                            {
                                type_destroy(&t);
                                goto _catch_label_1;
                            }
                            unsigned char   t_is_nullable = type_is_nullable(&t, ctx->ctx->options.null_checks_enabled);
                            flow_object_set_unknown(&t, t_is_nullable, po, nullable_enabled);
                            object_set_pointer(p_declarator->p_object, po);
                            type_destroy(&t);
                        }
                    }
                }
                else
                {
                    if (type_is_struct_or_union(&p_declarator->type))
                    {
                        unsigned char   is_nullable = type_is_nullable(&p_declarator->type, nullable_enabled);
                        flow_object_set_unknown(&p_declarator->type, is_nullable, p_declarator->p_object, nullable_enabled);
                    }
                    else
                    {
                        if (type_is_array(&p_declarator->type))
                        {
                            p_declarator->p_object->current.state = 64;
                        }
                        else
                        {
                            p_declarator->p_object->current.state = 32 | 64;
                        }
                    }
                }
            }
        }
        if (p_declarator->direct_declarator)
        {
            flow_visit_direct_declarator(ctx, p_declarator->direct_declarator);
        }
    }
    else _catch_label_1:
    {
    }
}

static void flow_visit_init_declarator_list(struct flow_visit_ctx * ctx, struct init_declarator_list * p_init_declarator_list)
{
    struct init_declarator * p_init_declarator = p_init_declarator_list->head;
    while (p_init_declarator)
    {
        flow_visit_init_declarator(ctx, p_init_declarator);
        p_init_declarator = p_init_declarator->next;
    }
}

static void flow_visit_member_declarator(struct flow_visit_ctx * ctx, struct member_declarator * p_member_declarator)
{
    if (p_member_declarator->declarator)
    {
        flow_visit_declarator(ctx, p_member_declarator->declarator);
    }
}

static void flow_visit_member_declarator_list(struct flow_visit_ctx * ctx, struct member_declarator_list * p_member_declarator_list)
{
    struct member_declarator * p_member_declarator = p_member_declarator_list->head;
    while (p_member_declarator)
    {
        flow_visit_member_declarator(ctx, p_member_declarator);
        p_member_declarator = p_member_declarator->next;
    }
}

static void flow_visit_member_declaration(struct flow_visit_ctx * ctx, struct member_declaration * p_member_declaration)
{
    if (p_member_declaration->member_declarator_list_opt)
    {
        flow_visit_specifier_qualifier_list(ctx, p_member_declaration->specifier_qualifier_list, &p_member_declaration->member_declarator_list_opt->head->declarator->type);
    }
    if (p_member_declaration->member_declarator_list_opt)
    {
        flow_visit_member_declarator_list(ctx, p_member_declaration->member_declarator_list_opt);
    }
}

static void flow_visit_member_declaration_list(struct flow_visit_ctx * ctx, struct member_declaration_list * p_member_declaration_list)
{
    struct member_declaration * p_member_declaration = p_member_declaration_list->head;
    while (p_member_declaration)
    {
        flow_visit_member_declaration(ctx, p_member_declaration);
        p_member_declaration = p_member_declaration->next;
    }
}

static void flow_visit_attribute_specifier(struct flow_visit_ctx * ctx, struct attribute_specifier * p_attribute_specifier)
{
}

static void flow_visit_attribute_specifier_sequence(struct flow_visit_ctx * ctx, struct attribute_specifier_sequence * p_visit_attribute_specifier_sequence)
{
    struct attribute_specifier * current = p_visit_attribute_specifier_sequence->head;
    while (current)
    {
        flow_visit_attribute_specifier(ctx, current);
        current = current->next;
    }
}

static void flow_visit_struct_or_union_specifier(struct flow_visit_ctx * ctx, struct struct_or_union_specifier * p_struct_or_union_specifier)
{
    if (p_struct_or_union_specifier->attribute_specifier_sequence_opt)
    {
        flow_visit_attribute_specifier_sequence(ctx, p_struct_or_union_specifier->attribute_specifier_sequence_opt);
    }
    flow_visit_member_declaration_list(ctx, &p_struct_or_union_specifier->member_declaration_list);
}

static void flow_visit_enumerator(struct flow_visit_ctx * ctx, struct enumerator * p_enumerator)
{
    struct true_false_set  a = {0};
    if (p_enumerator->constant_expression_opt)
    {
        flow_visit_expression(ctx, p_enumerator->constant_expression_opt, &a);
    }
    true_false_set_destroy(&a);
}

static void flow_visit_enumerator_list(struct flow_visit_ctx * ctx, struct enumerator_list * p_enumerator_list)
{
    struct enumerator * current = p_enumerator_list->head;
    while (current)
    {
        flow_visit_enumerator(ctx, current);
        current = current->next;
    }
}

static void flow_visit_enum_specifier(struct flow_visit_ctx * ctx, struct enum_specifier * p_enum_specifier)
{
    if (p_enum_specifier->attribute_specifier_sequence_opt)
    {
        flow_visit_attribute_specifier_sequence(ctx, p_enum_specifier->attribute_specifier_sequence_opt);
    }
    flow_visit_enumerator_list(ctx, &p_enum_specifier->enumerator_list);
}

static void flow_visit_typeof_specifier(struct flow_visit_ctx * ctx, struct typeof_specifier * p_typeof_specifier)
{
}

static void flow_visit_type_specifier(struct flow_visit_ctx * ctx, struct type_specifier * p_type_specifier)
{
    if (p_type_specifier->typeof_specifier)
    {
        flow_visit_typeof_specifier(ctx, p_type_specifier->typeof_specifier);
    }
    if (p_type_specifier->struct_or_union_specifier)
    {
        flow_visit_struct_or_union_specifier(ctx, p_type_specifier->struct_or_union_specifier);
    }
    if (p_type_specifier->enum_specifier)
    {
        flow_visit_enum_specifier(ctx, p_type_specifier->enum_specifier);
    }
    if (p_type_specifier->atomic_type_specifier)
    {
    }
}

static void flow_visit_type_specifier_qualifier(struct flow_visit_ctx * ctx, struct type_specifier_qualifier * p_type_specifier_qualifier)
{
    if (p_type_specifier_qualifier->type_qualifier)
    {
    }
    else
    {
        if (p_type_specifier_qualifier->type_specifier)
        {
            flow_visit_type_specifier(ctx, p_type_specifier_qualifier->type_specifier);
        }
        else
        {
            if (p_type_specifier_qualifier->alignment_specifier)
            {
            }
        }
    }
}

static void flow_visit_storage_class_specifier(struct flow_visit_ctx * ctx, struct storage_class_specifier * p_storage_class_specifier)
{
    if (p_storage_class_specifier->flags & 16)
    {
    }
}

static void flow_visit_declaration_specifier(struct flow_visit_ctx * ctx, struct declaration_specifier * p_declaration_specifier)
{
    if (p_declaration_specifier->function_specifier)
    {
    }
    if (p_declaration_specifier->storage_class_specifier)
    {
        flow_visit_storage_class_specifier(ctx, p_declaration_specifier->storage_class_specifier);
    }
    if (p_declaration_specifier->type_specifier_qualifier)
    {
        flow_visit_type_specifier_qualifier(ctx, p_declaration_specifier->type_specifier_qualifier);
    }
}

static void flow_visit_declaration_specifiers(struct flow_visit_ctx * ctx, struct declaration_specifiers * p_declaration_specifiers, struct type * p_type_opt)
{
    struct declaration_specifier * p_declaration_specifier = p_declaration_specifiers->head;
    while (p_declaration_specifier)
    {
        flow_visit_declaration_specifier(ctx, p_declaration_specifier);
        p_declaration_specifier = p_declaration_specifier->next;
    }
}

static unsigned char  flow_is_last_item_return(struct compound_statement * p_compound_statement)
{
    if (p_compound_statement && p_compound_statement->block_item_list.tail && p_compound_statement->block_item_list.tail->unlabeled_statement && p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement && p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement->first_token && p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement->first_token->type == 9027)
    {
        return 1;
    }
    return 0;
}

struct type get_function_return_type(struct type * p_type);

void flow_visit_declaration(struct flow_visit_ctx * ctx, struct declaration * p_declaration)
{
    if (p_declaration->static_assert_declaration)
    {
        flow_visit_static_assert_declaration(ctx, p_declaration->static_assert_declaration);
    }
    if (p_declaration->pragma_declaration)
    {
        flow_visit_pragma_declaration(ctx, p_declaration->pragma_declaration);
    }
    if (p_declaration->p_attribute_specifier_sequence_opt)
    {
        flow_visit_attribute_specifier_sequence(ctx, p_declaration->p_attribute_specifier_sequence_opt);
    }
    if (p_declaration->declaration_specifiers)
    {
        if (p_declaration->init_declarator_list.head)
        {
            flow_visit_declaration_specifiers(ctx, p_declaration->declaration_specifiers, &p_declaration->init_declarator_list.head->p_declarator->type);
        }
        else
        {
            flow_visit_declaration_specifiers(ctx, p_declaration->declaration_specifiers, ((void *)0));
        }
    }
    if (p_declaration->p_attribute_specifier_sequence_opt)
    {
    }
    if (p_declaration->init_declarator_list.head)
    {
        flow_visit_init_declarator_list(ctx, &p_declaration->init_declarator_list);
    }
    if (p_declaration->function_body)
    {
        ;
        struct type  type = get_function_return_type(&p_declaration->init_declarator_list.head->p_declarator->type);
        ctx->p_return_type = &type;
        flow_visit_compound_statement(ctx, p_declaration->function_body);
        type_destroy(&type);
        ctx->p_return_type = ((void *)0);
    }
}

void flow_objects_clear(struct flow_objects * p);

void flow_start_visit_declaration(struct flow_visit_ctx * ctx, struct declaration * p_declaration)
{
    ctx->labels_size = 0;
    flow_objects_clear(&ctx->arena);
    ctx->state_number_generator = 1;
    if (p_declaration->function_body)
    {
        flow_visit_declaration(ctx, p_declaration);
        ;
        if ( !flow_is_last_item_return(p_declaration->function_body))
        {
            flow_exit_block_visit_defer_list(ctx, &p_declaration->defer_list, p_declaration->function_body->last_token);
        }
    }
    else
    {
        flow_visit_declaration(ctx, p_declaration);
    }
    flow_objects_clear(&ctx->arena);
}

int flow_objects_push_back(struct flow_objects * p, struct flow_object * p_object);

struct flow_object *arena_new_object(struct flow_visit_ctx * ctx)
{
    struct flow_object * p = calloc(1, sizeof  *p);
    if (p != ((void *)0))
    {
        p->current.dbg_name = "current";
        p->id = ctx->arena.size + 1;
        if (flow_objects_push_back(&ctx->arena, p) != 0)
        {
            p = ((void *)0);
        }
    }
    return (struct flow_object *)p;
}

void flow_objects_destroy(struct flow_objects * p);

void flow_visit_ctx_destroy(struct flow_visit_ctx * p)
{
    flow_objects_destroy(&p->arena);
}

void flow_analysis_visit(struct flow_visit_ctx * ctx)
{
    struct declaration * p_declaration = ctx->ast.declaration_list.head;
    while (p_declaration)
    {
        flow_visit_declaration(ctx, p_declaration);
        p_declaration = p_declaration->next;
    }
}

