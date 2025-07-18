struct generic_assoc_list {
    struct generic_association * head;
    struct generic_association * tail;
};

struct object_name_list {
    char * name;
    struct object_name_list * previous;
};

struct parameter_type_list {
    unsigned char   is_var_args;
    unsigned char   is_void;
    struct parameter_list * parameter_list;
};

struct member_declaration_list {
    struct token * first_token;
    struct token * last_token;
    struct member_declaration * head;
    struct member_declaration * tail;
};

struct attribute {
    int attributes_flags;
    struct attribute_token * attribute_token;
    struct attribute_argument_clause * attribute_argument_clause;
    struct attribute * next;
};

struct try_statement {
    struct secondary_block * secondary_block;
    struct secondary_block * catch_secondary_block_opt;
    struct token * first_token;
    struct token * last_token;
    struct token * catch_token_opt;
    int catch_label_id;
};

struct label_list_item {
    struct token * p_last_usage;
    struct token * p_defined;
    struct label_list_item * next;
};

struct designator_list {
    struct designator * head;
    struct designator * tail;
};

struct scope_list {
    struct scope * head;
    struct scope * tail;
};

struct param_list {
    unsigned char   is_var_args;
    unsigned char   is_void;
    struct param * head;
    struct param * tail;
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

struct designation {
    struct designator_list * designator_list;
    struct token * token;
};

struct statement {
    struct labeled_statement * labeled_statement;
    struct unlabeled_statement * unlabeled_statement;
};

struct diagnostic {
    unsigned long long errors;
    unsigned long long warnings;
    unsigned long long notes;
};

struct label_list {
    struct label_list_item * head;
    struct label_list_item * tail;
};

struct member_declarator {
    struct declarator * declarator;
    struct expression * constant_expression;
    struct member_declarator * next;
};

struct argument_expression {
    struct expression * expression;
    struct argument_expression * next;
    unsigned char   set_unkown;
};

struct label_state {
    char * label_name;
    int state_number;
};

struct token_list {
    struct token * head;
    struct token * tail;
};

struct static_assert_declaration {
    struct token * first_token;
    struct token * last_token;
    struct expression * constant_expression;
    struct token * string_literal_opt;
};

struct initializer_list {
    struct token * first_token;
    struct initializer_list_item * head;
    struct initializer_list_item * tail;
    int size;
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
    int type;
    union _struct_tag_0  data;
};

struct initializer_list_item {
    struct designation * designation;
    struct initializer * initializer;
    struct initializer_list_item * next;
};

struct storage_class_specifier {
    int flags;
    struct token * token;
};

struct block_item_list {
    struct block_item * head;
    struct block_item * tail;
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

struct direct_declarator {
    struct token * name_opt;
    struct token * p_calling_convention;
    struct declarator * declarator;
    struct array_declarator * array_declarator;
    struct function_declarator * function_declarator;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
};

struct type {
    int category;
    int attributes_flags;
    int type_specifier_flags;
    int type_qualifier_flags;
    int storage_class_specifier_flags;
    char * name_opt;
    struct struct_or_union_specifier * struct_or_union_specifier;
    struct enum_specifier * enum_specifier;
    struct expression * array_num_elements_expression;
    unsigned int num_of_elements;
    unsigned char   has_static_array_size;
    unsigned char   address_of;
    struct param_list  params;
    struct type * next;
};

struct type_name {
    struct token * first_token;
    struct token * last_token;
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct declarator * abstract_declarator;
    struct type  type;
};

struct hash_map {
    struct map_entry ** table;
    int capacity;
    int size;
};

struct object {
    int state;
    int value_type;
    struct type  type;
    char * debug_name;
    union _struct_tag_11  value;
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

struct pointer {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * calling_convention;
    struct pointer * pointer;
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

struct attribute_specifier_sequence {
    struct token * first_token;
    struct token * last_token;
    int attributes_flags;
    struct attribute_specifier * head;
    struct attribute_specifier * tail;
};

struct flow_objects {
    struct flow_object ** data;
    int size;
    int capacity;
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

struct enumerator_list {
    struct enumerator * head;
    struct enumerator * tail;
};

struct expression_statement {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct expression * expression_opt;
};

struct attribute_specifier {
    struct token * first_token;
    struct token * last_token;
    struct attribute_list * attribute_list;
    struct attribute_specifier * next;
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

struct argument_expression_list {
    struct argument_expression * head;
    struct argument_expression * tail;
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
    struct declarator * declarator;
    struct init_declarator * p_init_declarator;
    int member_index;
    struct argument_expression_list  argument_expression_list;
    struct expression * condition_expr;
    struct expression * left;
    struct expression * right;
    unsigned char   is_assignment_expression;
};

struct balanced_token_sequence {
    struct balanced_token * head;
    struct balanced_token * tail;
};

struct parameter_list {
    struct parameter_declaration * head;
    struct parameter_declaration * tail;
};

struct type_qualifier {
    int flags;
    struct token * token;
    struct type_qualifier * next;
};

struct declaration_list {
    struct declaration * head;
    struct declaration * tail;
};

struct ast {
    struct token_list  token_list;
    struct declaration_list  declaration_list;
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

struct typeof_specifier_argument {
    struct expression * expression;
    struct type_name * type_name;
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

struct balanced_token {
    struct token * token;
    struct balanced_token * next;
};

struct block_item {
    struct token * first_token;
    struct declaration * declaration;
    struct unlabeled_statement * unlabeled_statement;
    struct label * label;
    struct block_item * next;
};

struct init_declarator_list {
    struct init_declarator * head;
    struct init_declarator * tail;
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

struct true_false_set_item {
    struct expression * p_expression;
    int true_branch_state;
    int false_branch_state;
};

struct secondary_block {
    struct token * first_token;
    struct token * last_token;
    struct statement * statement;
};

struct condition {
    struct expression * expression;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator * p_init_declarator;
    struct token * first_token;
    struct token * last_token;
};

struct atomic_type_specifier {
    struct token * token;
    struct type_name * type_name;
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

struct declaration_specifier {
    struct storage_class_specifier * storage_class_specifier;
    struct type_specifier_qualifier * type_specifier_qualifier;
    struct function_specifier * function_specifier;
    struct declaration_specifier * next;
};

struct labeled_statement {
    struct label * label;
    struct statement * statement;
};

struct attribute_argument_clause {
    struct balanced_token_sequence * p_balanced_token_sequence;
    struct token * token;
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

struct initializer {
    struct token * first_token;
    struct braced_initializer * braced_initializer;
    struct expression * assignment_expression;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
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

struct array_declarator {
    struct direct_declarator * direct_declarator;
    struct expression * assignment_expression;
    struct expression * expression;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * token;
    struct token * static_token_opt;
};

struct generic_association {
    struct type  type;
    struct type_name * p_type_name;
    struct expression * expression;
    struct token * first_token;
    struct token * last_token;
    struct generic_association * next;
};

struct generic_selection {
    struct expression * expression;
    struct type_name * type_name;
    struct expression * p_view_selected_expression;
    struct generic_assoc_list  generic_assoc_list;
    struct token * first_token;
    struct token * last_token;
};

struct true_false_set {
    struct true_false_set_item * data;
    int size;
    int capacity;
};

struct unlabeled_statement {
    struct expression_statement * expression_statement;
    struct primary_block * primary_block;
    struct jump_statement * jump_statement;
};

struct osstream {
    char * c_str;
    int size;
    int capacity;
};

struct function_specifier {
    int flags;
    struct token * token;
};

struct defer_list_item {
    struct declarator * declarator;
    struct defer_statement * defer_statement;
    struct defer_list_item * next;
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

struct param {
    struct type  type;
    struct param * next;
};

struct attribute_list {
    int attributes_flags;
    struct attribute * head;
    struct attribute * tail;
};

struct diagnostic_stack {
    int top_index;
    struct diagnostic stack[10];
};

struct enum_specifier {
    unsigned char   has_shared_ownership;
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct specifier_qualifier_list * specifier_qualifier_list;
    char tag_name[200];
    struct enumerator_list  enumerator_list;
    struct token * tag_token;
    struct token * first_token;
    struct enum_specifier * p_complete_enum_specifier;
};

struct declaration_specifiers {
    int attributes_flags;
    int type_specifier_flags;
    int type_qualifier_flags;
    int storage_class_specifier_flags;
    int function_specifier_flags;
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

struct simple_declaration {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator_list  init_declarator_list;
    struct token * first_token;
    struct token * last_token;
};

struct specifier_qualifier_list {
    int type_specifier_flags;
    int type_qualifier_flags;
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

struct init_declarator {
    unsigned char   has_shared_ownership;
    struct declarator * p_declarator;
    struct initializer * initializer;
    struct init_declarator * next;
};

struct type_qualifier_list {
    int flags;
    struct type_qualifier * head;
    struct type_qualifier * tail;
};

struct type_specifier_qualifier {
    struct type_specifier * type_specifier;
    struct type_qualifier * type_qualifier;
    struct alignment_specifier * alignment_specifier;
    struct type_specifier_qualifier * next;
};

struct braced_initializer {
    struct token * first_token;
    struct token * last_token;
    struct initializer_list * initializer_list;
};

struct object_visitor {
    int member_index;
    struct type * p_type;
    struct flow_object * p_object;
};

struct options {
    int input;
    int target;
    struct diagnostic_stack  diagnostic_stack;
    int style;
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

struct parameter_declaration {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct declaration_specifiers * declaration_specifiers;
    struct declarator * declarator;
    struct parameter_declaration * next;
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
    int last_diagnostics_ids[2];
    int fatal_error_expected;
    unsigned char   ignore_this_report;
};

struct defer_statement {
    struct token * first_token;
    struct token * last_token;
    struct secondary_block * secondary_block;
};

struct designator {
    struct expression * constant_expression_opt;
    struct token * token;
    struct designator * next;
};

struct pragma_declaration {
    struct token * first_token;
    struct token * last_token;
};

struct alignment_specifier {
    struct type_name * type_name;
    struct expression * constant_expression;
    struct token * token;
};

struct init_statement {
    struct expression_statement * p_expression_statement;
    struct simple_declaration * p_simple_declaration;
};

struct attribute_token {
    int attributes_flags;
    struct token * token;
};

struct primary_block {
    struct compound_statement * compound_statement;
    struct selection_statement * selection_statement;
    struct iteration_statement * iteration_statement;
    struct defer_statement * defer_statement;
    struct try_statement * try_statement;
};

struct jump_statement {
    struct token * label;
    struct token * first_token;
    struct token * last_token;
    struct expression * expression_opt;
    int label_id;
    struct defer_list  defer_list;
};

struct typeof_specifier {
    struct token * first_token;
    struct token * last_token;
    struct typeof_specifier_argument * typeof_specifier_argument;
    struct type  type;
};

struct member_declaration {
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct member_declarator_list * member_declarator_list_opt;
    struct static_assert_declaration * static_assert_declaration;
    struct pragma_declaration * pragma_declaration;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct member_declaration * next;
};

struct _iobuf {
    void * _Placeholder;
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

struct label {
    struct expression * constant_expression;
    struct expression * constant_expression_end;
    struct token * p_identifier_opt;
    struct token * p_first_token;
    struct label * next;
    int label_id;
};

struct member_declarator_list {
    struct member_declarator * head;
    struct member_declarator * tail;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

int _Avx2WmemEnabledWeakValue = 0;
unsigned int s_visit_number = 1;
unsigned char  flow_object_is_not_null(struct flow_object * p)
{
    int e;

    e = p->current.state;
    return !!((!(e & 2) && ((e & 4) || (e & 8))));
}

unsigned char  flow_object_can_be_not_null_or_moved(struct flow_object * p)
{
    int e;

    e = p->current.state;
    return !!((e & 4) || (e & 8));
}

unsigned char  flow_object_is_null(struct flow_object * p)
{
    int e;

    e = p->current.state;
    return !!(((e & 2) && !(e & 4) && !(e & 8)));
}

unsigned char  flow_object_is_zero(struct flow_object * p)
{
    int e;

    e = p->current.state;
    return !!(((e & 32) && !(e & 64)));
}

unsigned char  flow_object_is_not_zero(struct flow_object * p)
{
    int e;

    e = p->current.state;
    return !!((!(e & 32) && (e & 64)));
}

unsigned char  flow_object_can_be_zero(struct flow_object * p)
{
    int e;

    e = p->current.state;
    return !!((e & 32));
}

unsigned char  flow_object_can_be_moved(struct flow_object * p)
{
    int e;

    e = p->current.state;
    return !!((e & 8));
}

unsigned char  flow_object_can_be_null(struct flow_object * p)
{
    int e;

    e = p->current.state;
    return !!((e & 2));
}

unsigned char  flow_object_is_uninitialized(struct flow_object * p)
{
    int e;

    e = p->current.state;
    return !!(e == 1);
}

unsigned char  flow_object_can_be_uninitialized(struct flow_object * p)
{
    int e;

    e = p->current.state;
    return !!((e & 1));
}

unsigned char  flow_object_can_have_its_lifetime_ended(struct flow_object * p)
{
    int e;

    e = p->current.state;
    return !!((e & 128));
}



int __cdecl __stdio_common_vfprintf(unsigned __int64 _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);

static unsigned __int64 __ck__OptionsStorage1;
inline unsigned __int64 *__cdecl __local_stdio_printf_options(void)
{
    return &__ck__OptionsStorage1;
}
inline int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
inline int __cdecl printf(char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4U));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0U, _ArgList);
    ((void)(_ArgList = 0U));
    return _Result;
}

static void object_state_to_string_core(int e)
{
    unsigned char   first;

    first = 1;
    printf("\"");
    if (e & 1)
    {
        if (first)
        {
            first = 0;
        }
        else
        {
            printf(" | ");
        }
        printf("uninitialized");
    }
    if (e & 4)
    {
        if (first)
        {
            first = 0;
        }
        else
        {
            printf(" | ");
        }
        printf("not-null");
    }
    if (e & 2)
    {
        if (first)
        {
            first = 0;
        }
        else
        {
            printf(" | ");
        }
        printf("null");
    }
    if (e & 32)
    {
        if (first)
        {
            first = 0;
        }
        else
        {
            printf(" | ");
        }
        printf("zero");
    }
    if (e & 64)
    {
        if (first)
        {
            first = 0;
        }
        else
        {
            printf(" | ");
        }
        printf("not-zero");
    }
    if (e & 128)
    {
        if (first)
        {
            first = 0;
        }
        else
        {
            printf(" | ");
        }
        printf("lifetime-ended");
    }
    if (e & 8)
    {
        if (first)
        {
            first = 0;
        }
        else
        {
            printf(" | ");
        }
        printf("moved");
    }
    printf("\"");
}

void objects_view_destroy(struct flow_objects_view * p);
void free(void * ptr);

void flow_object_state_delete(struct flow_object_state * p)
{
    if (p)
    {
        objects_view_destroy(&p->alternatives);
        ;
        free(p);
    }
}

void objects_view_copy(struct flow_objects_view * dest, struct flow_objects_view * source);

void flow_object_state_copy(struct flow_object_state * to, struct flow_object_state * from)
{
    to->state = from->state;
    to->pointed = from->pointed;
    objects_view_copy(&to->alternatives, &from->alternatives);
}

void flow_object_print_state(struct flow_object * p)
{
    object_state_to_string_core(p->current.state);
}

void object_state_set_item_print(struct flow_object_state * item)
{
    object_state_to_string_core(item->state);
}

unsigned char  flow_object_is_expansible(struct flow_object * p_object)
{
    return !!((p_object && p_object->current.state != 2 && p_object->current.state != 1 && p_object->current.pointed == 0));
}

struct type type_remove_pointer(struct type * p_type);
unsigned char  type_is_void(struct type * p_type);
struct flow_object *make_flow_object(struct flow_visit_ctx * ctx, struct type * p_type, struct declarator * p_declarator_opt, struct expression * p_expression_origin);
unsigned char  type_is_opt(struct type * p_type, unsigned char   nullable_enabled);
void flow_object_set_unknown(struct type * p_type, unsigned char   t_is_nullable, struct flow_object * p_object, unsigned char   nullable_enabled);
void object_set_pointer(struct flow_object * p_object, struct flow_object * p_object2);
void flow_object_push_states_from(struct flow_object * p_object_from, struct flow_object * p_object_to);
void type_destroy(struct type * p_type);

void flow_object_expand_pointer(struct flow_visit_ctx * ctx, struct type * p_type, struct flow_object * p_object)
{
    ;
    if (flow_object_is_expansible(p_object))
    {
        unsigned char   nullable_enabled;
        struct type  t2;

        nullable_enabled = ctx->ctx->options.null_checks_enabled;
        t2 = type_remove_pointer(p_type);
        if (!type_is_void(&t2))
        {
            struct flow_object * p_object2;

            p_object2 = make_flow_object(ctx, &t2, p_object->p_declarator_origin, p_object->p_expression_origin);
            if (p_object2)
            {
                unsigned char   is_nullable;

                is_nullable = type_is_opt(&t2, nullable_enabled);
                flow_object_set_unknown(&t2, is_nullable, p_object2, nullable_enabled);
                object_set_pointer(p_object, p_object2);
                flow_object_push_states_from(p_object, p_object2);
            }
            else
            {
            }
        }
        type_destroy(&t2);
    }
}

void flow_object_swap(struct flow_object * a, struct flow_object * b)
{
    struct flow_object  temp;

    temp = *a;
    *a = *b;
    *b = temp;
}

void flow_object_destroy(struct flow_object * p);

void flow_object_delete(struct flow_object * p)
{
    if (p)
    {
        flow_object_destroy(p);
        free(p);
    }
}

void object_set_pointer(struct flow_object * p_object, struct flow_object * p_object2)
{
    p_object->current.pointed = p_object2;
}

void flow_object_destroy(struct flow_object * p)
{
    struct flow_object_state * p_flow_object_state;

    objects_view_destroy(&p->members);
    p_flow_object_state = p->current.next;
    while (p_flow_object_state)
    {
        struct flow_object_state * temp;

        temp = p_flow_object_state->next;
        p_flow_object_state->next = 0U;
        flow_object_state_delete(p_flow_object_state);
        p_flow_object_state = temp;
    }
    objects_view_destroy(&p->current.alternatives);
}

void flow_object_update_current(struct flow_object * p)
{
    if (p->current.alternatives.size > 0)
    {
        p->current.state = 0;
        {
            int i;
            i = 0;
            for (; i < p->current.alternatives.size; i++)
            {
                flow_object_update_current(p->current.alternatives.data[i]);
                p->current.state |= p->current.alternatives.data[i]->current.state;
            }
        }
    }
}

void flow_object_set_current_state_to_can_be_null(struct flow_object * p)
{
    p->current.state |= 2;
}

void flow_object_set_current_state_to_is_null(struct flow_object * p)
{
    p->current.state = 2;
    if (p->current.alternatives.size > 0)
    {
        {
            int i;
            i = 0;
            for (; i < p->current.alternatives.size; i++)
            {
                flow_object_set_current_state_to_can_be_null(p->current.alternatives.data[i]);
            }
        }
    }
}

int flow_object_add_state(struct flow_object * p, struct flow_object_state * pnew)
{
    ;
    pnew->next = p->current.next;
    p->current.next = pnew;
    return 0;
}

void objects_view_destroy(struct flow_objects_view * p)
{
    free(p->data);
}

void *realloc(void * ptr, unsigned int size);

int objects_view_reserve(struct flow_objects_view * p, int n)
{
    if (n > p->capacity)
    {
        void * pnew;

        if ((unsigned int)n > 1073741823U)
        {
            return 132;
        }
        pnew = realloc(p->data, n * 4U);
        if (pnew == 0U)
        {
            return 12;
        }
        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}

int objects_view_push_back(struct flow_objects_view * p, struct flow_object * p_object)
{
    if (p->size == 2147483647)
    {
        return 132;
    }
    if (p->size + 1 > p->capacity)
    {
        int new_capacity;
        int error;

        new_capacity = 0;
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
        error = objects_view_reserve(p, new_capacity);
        if (error != 0)
        {
            return error;
        }
    }
    p->data[p->size] = p_object;
    p->size++;
    return 0;
}

void objects_view_clear(struct flow_objects_view * p)
{
    p->size = 0;
}

unsigned char  objects_view_find(struct flow_objects_view * p, struct flow_object * p_object);

void objects_view_merge(struct flow_objects_view * dest, struct flow_objects_view * source)
{
    {
        int i;
        i = 0;
        for (; i < source->size; i++)
        {
            if (!objects_view_find(dest, source->data[i]))
            {
                objects_view_push_back(dest, source->data[i]);
            }
        }
    }
}

void objects_view_copy(struct flow_objects_view * dest, struct flow_objects_view * source)
{
    objects_view_reserve(dest, source->size);
    {
        int i;
        i = 0;
        for (; i < source->size; i++)
        {
            dest->data[i] = source->data[i];
        }
    }
    dest->size = source->size;
}

unsigned char  objects_view_find(struct flow_objects_view * p, struct flow_object * p_object)
{
    {
        int i;
        i = 0;
        for (; i < p->size; i++)
        {
            if (p->data[i] == p_object)
            {
                return 1;
            }
        }
    }
    return 0;
}

void flow_objects_destroy(struct flow_objects * p)
{
    {
        int i;
        i = 0;
        for (; i < p->size; i++)
        {
            flow_object_delete(p->data[i]);
        }
    }
    free(p->data);
}

void flow_objects_clear(struct flow_objects * p)
{
    {
        int i;
        i = 0;
        for (; i < p->size; i++)
        {
            flow_object_delete(p->data[i]);
        }
    }
    p->size = 0;
}

int objects_reserve(struct flow_objects * p, int n)
{
    if (n > p->capacity)
    {
        void * pnew;

        if ((unsigned int)n > 1073741823U)
        {
            return 132;
        }
        pnew = realloc(p->data, n * 4U);
        if (pnew == 0U)
        {
            return 12;
        }
        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}

struct flow_object *flow_objects_find(struct flow_objects * p_objects, struct flow_object * object)
{
    {
        int i;
        i = 0;
        for (; i < p_objects->size; i++)
        {
            if (p_objects->data[i] == object)
            {
                return object;
            }
        }
    }
    return 0U;
}

int flow_objects_push_back(struct flow_objects * p, struct flow_object * p_object)
{
    if (p->size == 2147483647)
    {
        flow_object_delete(p_object);
        return 132;
    }
    if (p->size + 1 > p->capacity)
    {
        int new_capacity;
        int error;

        new_capacity = 0;
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
        error = objects_reserve(p, new_capacity);
        if (error != 0)
        {
            flow_object_delete(p_object);
            return error;
        }
        ;
    }
    p->data[p->size] = p_object;
    p->size++;
    return 0;
}

int __cdecl strcmp(char * _Str1, char * _Str2);

unsigned char  has_name(char * name, struct object_name_list * list)
{
    struct object_name_list * p;

    p = list;
    while (p)
    {
        if (strcmp(p->name, name) == 0)
        {
            return 1;
        }
        p = p->previous;
    }
    return 0;
}

struct flow_object *arena_new_object(struct flow_visit_ctx * ctx);
struct struct_or_union_specifier *get_complete_struct_or_union_specifier(struct struct_or_union_specifier * p_struct_or_union_specifier);
unsigned char  type_is_array(struct type * p_type);

struct flow_object *make_object_core(struct flow_visit_ctx * ctx, struct type * p_type, struct object_name_list * list, struct declarator * p_declarator_opt, struct expression * p_expression_origin)
{
    struct flow_object * p_object;

    if (p_declarator_opt == 0U)
    {
    }
    if (p_expression_origin == 0U)
    {
    }
    p_object = arena_new_object(ctx);
    /*try*/ if (1)
    {
        if (p_object == 0U)
        {
            /*throw*/ goto _CKL2;
        }
        p_object->p_expression_origin = p_expression_origin;
        p_object->p_declarator_origin = p_declarator_opt;
        if (p_type->struct_or_union_specifier)
        {
            struct struct_or_union_specifier * p_struct_or_union_specifier;

            p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
            if (p_struct_or_union_specifier)
            {
                struct member_declaration * p_member_declaration;
                struct object_name_list  l;

                p_object->current.state = 0;
                p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;
                l.name = p_struct_or_union_specifier->tag_name;
                l.previous = list;
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
                                char * tag;

                                tag = 0U;
                                if (p_member_declarator->declarator->type.struct_or_union_specifier)
                                {
                                    tag = p_member_declarator->declarator->type.struct_or_union_specifier->tag_name;
                                }
                                else
                                {
                                    if (p_member_declarator->declarator->type.next && p_member_declarator->declarator->type.next->struct_or_union_specifier)
                                    {
                                        tag = p_member_declarator->declarator->type.next->struct_or_union_specifier->tag_name;
                                    }
                                }
                                if (tag && has_name(tag, &l))
                                {
                                    struct flow_object * member_obj;

                                    member_obj = arena_new_object(ctx);
                                    if (member_obj == 0U)
                                    {
                                        /*throw*/ goto _CKL2;
                                    }
                                    member_obj->parent = p_object;
                                    member_obj->p_expression_origin = p_expression_origin;
                                    member_obj->p_declarator_origin = p_declarator_opt;
                                    member_obj->current.state = 0;
                                    objects_view_push_back(&p_object->members, member_obj);
                                }
                                else
                                {
                                    struct flow_object * p_member_obj;

                                    p_member_obj = make_object_core(ctx, &p_member_declarator->declarator->type, &l, p_declarator_opt, p_expression_origin);
                                    if (p_member_obj == 0U)
                                    {
                                        /*throw*/ goto _CKL2;
                                    }
                                    p_member_obj->parent = p_object;
                                    objects_view_push_back(&p_object->members, p_member_obj);
                                }
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
                                struct flow_object * member_obj;

                                _cake_zmem(&t, 60);
                                t.category = 0;
                                t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                                t.type_specifier_flags = 32768;
                                member_obj = make_object_core(ctx, &t, &l, p_declarator_opt, p_expression_origin);
                                if (member_obj == 0U)
                                {
                                    type_destroy(&t);
                                    /*throw*/ goto _CKL2;
                                }
                                {
                                    int k;
                                    k = 0;
                                    for (; k < member_obj->members.size; k++)
                                    {
                                        objects_view_push_back(&p_object->members, member_obj->members.data[k]);
                                        member_obj->members.data[k] = 0U;
                                    }
                                }
                                type_destroy(&t);
                            }
                        }
                    }
                    p_member_declaration = p_member_declaration->next;
                }
                return p_object;
            }
            return p_object;
        }
        else
        {
            if (type_is_array(p_type))
            {
                return p_object;
            }
        }
        p_object->current.state = 1;
    }
    /*catch*/ else _CKL2:
    {
        p_object = 0U;
    }
    return p_object;
}

struct flow_object *make_flow_object(struct flow_visit_ctx * ctx, struct type * p_type, struct declarator * p_declarator_opt, struct expression * p_expression_origin)
{
    struct object_name_list  list;
    struct flow_object * p_object;

    list.name = "";
    list.previous = 0;
    p_object = make_object_core(ctx, p_type, &list, p_declarator_opt, p_expression_origin);
    return p_object;
}

struct token *flow_object_get_token(struct flow_object * object)
{
    if (object->p_declarator_origin)
    {
        return object->p_declarator_origin->name_opt;
    }
    if (object->p_expression_origin)
    {
        return object->p_expression_origin->first_token;
    }
    return 0U;
}

void *calloc(unsigned int nmemb, unsigned int size);

void flow_object_add_new_state_as_a_copy_of_current_state(struct flow_object * object, char * name, int state_number)
{
    struct flow_object_state * pnew;

    pnew = calloc(1, 32U);
    if (pnew == 0U)
    {
        return;
    }
    pnew->dbg_name = name;
    pnew->state_number = state_number;
    pnew->next = object->current.next;
    object->current.next = pnew;
    flow_object_state_copy(pnew, &object->current);
}

void flow_object_remove_state(struct flow_object * object, int state_number)
{
    struct flow_object_state * previous;
    struct flow_object_state * it;

    previous = &object->current;
    it = object->current.next;
    while (it)
    {
        if (it->state_number == state_number)
        {
            struct flow_object_state * p_it_next;

            p_it_next = it->next;
            it->next = 0U;
            flow_object_state_delete(previous->next);
            previous->next = p_it_next;
            break;
        }
        previous = it;
        it = it->next;
    }
}

int snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...);
unsigned char  type_is_pointer(struct type * p_type);

void print_object_core(int ident, struct object_visitor * p_visitor, char * previous_names, unsigned char   is_pointer, unsigned char   short_version, unsigned int visit_number)
{
    if (p_visitor->p_object->visit_number == visit_number)
    {
        return;
    }
    p_visitor->p_object->visit_number = visit_number;
    if (p_visitor->p_type->struct_or_union_specifier)
    {
        struct struct_or_union_specifier * p_struct_or_union_specifier;

        ;
        p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);
        if (p_struct_or_union_specifier)
        {
            struct member_declaration * p_member_declaration;

            printf("%*c", ident + 1, 32);
            printf("#%02d {\n", p_visitor->p_object->id);
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
                            char * name;
                            char buffer[200];
                            struct object_visitor  visitor;

                            name = p_member_declarator->declarator->name_opt ? p_member_declarator->declarator->name_opt->lexeme : "";
                            _cake_zmem(&buffer, 200);
                            if (is_pointer)
                            {
                                snprintf(buffer, 200U, "%s->%s", previous_names, name);
                            }
                            else
                            {
                                snprintf(buffer, 200U, "%s.%s", previous_names, name);
                            }
                            _cake_zmem(&visitor, 12);
                            visitor.p_type = &p_member_declarator->declarator->type;
                            visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];
                            print_object_core(ident + 2, &visitor, buffer, type_is_pointer(&p_member_declarator->declarator->type), short_version, visit_number);
                            p_visitor->member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                else
                {
                    if (p_member_declaration->specifier_qualifier_list != 0U)
                    {
                        struct type  t;
                        struct type * temp;
                        int visit_number0;

                        _cake_zmem(&t, 60);
                        t.category = 0;
                        t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                        t.type_specifier_flags = 32768;
                        temp = p_visitor->p_type;
                        p_visitor->p_type = &t;
                        visit_number0 = p_visitor->p_object->visit_number;
                        p_visitor->p_object->visit_number = 0;
                        print_object_core(ident + 1, p_visitor, previous_names, 0, short_version, visit_number);
                        p_visitor->p_type = temp;
                        p_visitor->p_object->visit_number = visit_number0;
                        type_destroy(&t);
                    }
                }
                p_member_declaration = p_member_declaration->next;
            }
            printf("%*c", ident + 1, 32);
            printf("}\n");
        }
    }
    else
    {
        if (type_is_pointer(p_visitor->p_type))
        {
            struct type  t2;

            t2 = type_remove_pointer(p_visitor->p_type);
            printf("%*c", ident, 32);
            if (short_version)
            {
                printf("#%02d %s == ", p_visitor->p_object->id, previous_names);
                flow_object_print_state(p_visitor->p_object);
                if (flow_object_is_null(p_visitor->p_object))
                {
                }
                else
                {
                    if (flow_object_can_be_uninitialized(p_visitor->p_object))
                    {
                    }
                    else
                    {
                        printf(" -> ");
                        if (p_visitor->p_object->current.pointed != 0U)
                        {
                            printf(" #%02d", p_visitor->p_object->current.pointed->id);
                        }
                        else
                        {
                            printf("{...}");
                        }
                    }
                }
            }
            else
            {
                struct flow_object_state * it;

                printf("%p:%s == ", p_visitor->p_object, previous_names);
                printf("{");
                it = p_visitor->p_object->current.next;
                while (it)
                {
                    printf("\x1b[36;1m");
                    printf("(#%02d %s)", it->state_number, it->dbg_name);
                    object_state_set_item_print(it);
                    printf("\x1b[0m");
                    printf(",");
                    it = it->next;
                }
                printf("\x1b[95m");
                printf("(current)");
                flow_object_print_state(p_visitor->p_object);
                printf("\x1b[0m");
                printf("}");
            }
            printf("\n");
            type_destroy(&t2);
        }
        else
        {
            printf("%*c", ident, 32);
            if (short_version)
            {
                printf("#%02d %s == ", p_visitor->p_object->id, previous_names);
                flow_object_print_state(p_visitor->p_object);
            }
            else
            {
                struct flow_object_state * it;

                printf("%p:%s == ", p_visitor->p_object, previous_names);
                printf("{");
                it = p_visitor->p_object->current.next;
                while (it)
                {
                    printf("(#%02d %s)", it->state_number, it->dbg_name);
                    object_state_set_item_print(it);
                    printf(",");
                    it = it->next;
                }
                flow_object_print_state(p_visitor->p_object);
                printf("}");
            }
            printf("\n");
        }
    }
}

void flow_object_merge_state(struct flow_object * pdest, struct flow_object * object1, struct flow_object * object2)
{
    pdest->current.state = object1->current.state | object2->current.state;
    if (pdest->members.size == object1->members.size && object1->members.size == object2->members.size)
    {
        {
            int i;
            i = 0;
            for (; i < object1->members.size; i++)
            {
                struct flow_object * m1;
                struct flow_object * m2;

                m1 = object1->members.data[i];
                m2 = object2->members.data[i];
                flow_object_merge_state(pdest->members.data[i], m1, m2);
            }
        }
    }
}

int object_restore_current_state_from_core(struct flow_object * object, int state_number, unsigned int visit_number)
{
    struct flow_object_state * it;

    if (object->visit_number == visit_number)
    {
        return 1;
    }
    object->visit_number = visit_number;
    it = object->current.next;
    while (it)
    {
        if (it->state_number == state_number)
        {
            object->current.state = it->state;
            break;
        }
        it = it->next;
    }
    {
        int i;
        i = 0;
        for (; i < object->members.size; i++)
        {
            object_restore_current_state_from_core(object->members.data[i], state_number, visit_number);
        }
    }
    return 1;
}

extern unsigned int s_visit_number;

int flow_object_restore_current_state_from(struct flow_object * object, int state_number)
{
    return object_restore_current_state_from_core(object, state_number, s_visit_number++);
}

int object_merge_current_state_with_state_number_core(struct flow_object * object, int state_number, unsigned int visit_number)
{
    struct flow_object_state * it;

    if (object->visit_number == visit_number)
    {
        return 1;
    }
    object->visit_number = visit_number;
    it = object->current.next;
    while (it)
    {
        if (it->state_number == state_number)
        {
            it->state |= object->current.state;
            break;
        }
        it = it->next;
    }
    {
        int i;
        i = 0;
        for (; i < object->members.size; i++)
        {
            object_merge_current_state_with_state_number_core(object->members.data[i], state_number, visit_number);
        }
    }
    if (object->current.state == 2 || object->current.state == 1)
    {
    }
    else
    {
    }
    return 1;
}

void object_get_name(struct type * p_type, struct flow_object * p_object, char * outname, int out_size);

void print_flow_object(struct type * p_type, struct flow_object * p_object, unsigned char   short_version)
{
    char name[100];
    struct object_visitor  visitor;

    _cake_zmem(&name, 100);
    object_get_name(p_type, p_object, name, 100U);
    _cake_zmem(&visitor, 12);
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    print_object_core(0, &visitor, name, type_is_pointer(p_type), short_version, s_visit_number++);
}

void object_set_uninitialized_core(struct object_visitor * p_visitor)
{
    if (p_visitor->p_type->struct_or_union_specifier)
    {
        struct struct_or_union_specifier * p_struct_or_union_specifier;

        p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);
        if (p_struct_or_union_specifier)
        {
            struct member_declaration * p_member_declaration;

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
                            if (p_visitor->member_index < p_visitor->p_object->members.size)
                            {
                                struct object_visitor  visitor;

                                _cake_zmem(&visitor, 12);
                                visitor.p_type = &p_member_declarator->declarator->type;
                                visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];
                                object_set_uninitialized_core(&visitor);
                            }
                            p_visitor->member_index++;
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
                            struct type * type_temp;

                            _cake_zmem(&t, 60);
                            t.category = 0;
                            t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                            t.type_specifier_flags = 32768;
                            type_temp = p_visitor->p_type;
                            p_visitor->p_type = &t;
                            object_set_uninitialized_core(p_visitor);
                            p_visitor->p_type = type_temp;
                            type_destroy(&t);
                        }
                    }
                }
                p_member_declaration = p_member_declaration->next;
            }
            return;
        }
    }
    if (type_is_pointer(p_visitor->p_type))
    {
        p_visitor->p_object->current.state = 1;
    }
    else
    {
        p_visitor->p_object->current.state = 1;
    }
}

void flow_object_set_uninitialized(struct type * p_type, struct flow_object * p_object)
{
    struct object_visitor  visitor;

    _cake_zmem(&visitor, 12);
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    object_set_uninitialized_core(&visitor);
}

unsigned char  type_is_owner_or_pointer_to_dtor(struct type * p_type);
unsigned char  compiler_diagnostic(int w, struct parser_ctx * ctx, struct token * p_token, struct marker * p_marker, char * fmt, ...);

static void checked_empty_core(struct flow_visit_ctx * ctx, struct type * p_type, struct flow_object * p_object, char * previous_names, struct marker * p_marker)
{
    if (p_type->struct_or_union_specifier && p_object->members.size > 0)
    {
        struct struct_or_union_specifier * p_struct_or_union_specifier;
        struct member_declaration * p_member_declaration;
        int member_index;

        p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
        p_member_declaration = p_struct_or_union_specifier ? p_struct_or_union_specifier->member_declaration_list.head : 0U;
        member_index = 0;
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
                        char * name;
                        char buffer[200];

                        name = p_member_declarator->declarator->name_opt ? p_member_declarator->declarator->name_opt->lexeme : "";
                        _cake_zmem(&buffer, 200);
                        if (type_is_pointer(p_type))
                        {
                            snprintf(buffer, 200U, "%s->%s", previous_names, name);
                        }
                        else
                        {
                            snprintf(buffer, 200U, "%s.%s", previous_names, name);
                        }
                        checked_empty_core(ctx, &p_member_declarator->declarator->type, p_object->members.data[member_index], name, p_marker);
                        member_index++;
                    }
                    p_member_declarator = p_member_declarator->next;
                }
            }
            else
            {
                if (p_member_declaration->specifier_qualifier_list != 0U)
                {
                }
            }
            p_member_declaration = p_member_declaration->next;
        }
        return;
    }
    if (type_is_owner_or_pointer_to_dtor(p_type))
    {
        if (p_object->current.state == 11 || p_object->current.state == 10 || p_object->current.state == 2 || p_object->current.state == 8 || p_object->current.state == 1 || p_object->current.state == 3 || p_object->current.state == 9)
        {
        }
        else
        {
            compiler_diagnostic(28, ctx->ctx, 0U, p_marker, "object '%s' may not be empty", previous_names);
        }
    }
}

void checked_empty(struct flow_visit_ctx * ctx, struct type * p_type, struct flow_object * p_object, struct marker * p_marker)
{
    char name[100];

    _cake_zmem(&name, 100);
    object_get_name(p_type, p_object, name, 100U);
    checked_empty_core(ctx, p_type, p_object, name, p_marker);
}

static void object_set_moved_core(struct object_visitor * p_visitor)
{
    if (p_visitor->p_type->struct_or_union_specifier)
    {
        struct struct_or_union_specifier * p_struct_or_union_specifier;

        p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);
        if (p_struct_or_union_specifier)
        {
            struct member_declaration * p_member_declaration;

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
                            if (p_visitor->member_index < p_visitor->p_object->members.size)
                            {
                                struct object_visitor  visitor;

                                _cake_zmem(&visitor, 12);
                                visitor.p_type = &p_member_declarator->declarator->type;
                                visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];
                                object_set_moved_core(&visitor);
                            }
                            p_visitor->member_index++;
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
                            struct type * type_temp;

                            _cake_zmem(&t, 60);
                            t.category = 0;
                            t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                            t.type_specifier_flags = 32768;
                            type_temp = p_visitor->p_type;
                            p_visitor->p_type = &t;
                            object_set_moved_core(p_visitor);
                            p_visitor->p_type = type_temp;
                            type_destroy(&t);
                        }
                    }
                }
                p_member_declaration = p_member_declaration->next;
            }
            return;
        }
    }
    if (type_is_pointer(p_visitor->p_type))
    {
        p_visitor->p_object->current.state = 8;
    }
    else
    {
        p_visitor->p_object->current.state = 8;
    }
}

void flow_object_set_moved(struct type * p_type, struct flow_object * p_object)
{
    struct object_visitor  visitor;

    _cake_zmem(&visitor, 12);
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    object_set_moved_core(&visitor);
}

unsigned char  type_is_struct_or_union(struct type * p_type);

static void object_set_unknown_core(struct object_visitor * p_visitor, unsigned char   t_is_nullable, unsigned int visit_number, unsigned char   nullable_enabled)
{
    if (p_visitor->p_object->visit_number == visit_number)
    {
        return;
    }
    p_visitor->p_object->visit_number = visit_number;
    if (p_visitor->p_type->struct_or_union_specifier)
    {
        struct struct_or_union_specifier * p_struct_or_union_specifier;

        p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);
        if (p_struct_or_union_specifier)
        {
            struct member_declaration * p_member_declaration;

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
                            if (p_visitor->member_index < p_visitor->p_object->members.size)
                            {
                                struct object_visitor  visitor;

                                _cake_zmem(&visitor, 12);
                                visitor.p_type = &p_member_declarator->declarator->type;
                                visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];
                                object_set_unknown_core(&visitor, t_is_nullable, visit_number, nullable_enabled);
                            }
                            else
                            {
                            }
                            p_visitor->member_index++;
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
                            struct type * temp;
                            int vn;

                            _cake_zmem(&t, 60);
                            t.category = 0;
                            t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                            t.type_specifier_flags = 32768;
                            temp = p_visitor->p_type;
                            p_visitor->p_type = &t;
                            vn = p_visitor->p_object->visit_number;
                            p_visitor->p_object->visit_number = 0;
                            object_set_unknown_core(p_visitor, t_is_nullable, visit_number, nullable_enabled);
                            p_visitor->p_object->visit_number = vn;
                            p_visitor->p_type = temp;
                            type_destroy(&t);
                        }
                    }
                }
                p_member_declaration = p_member_declaration->next;
            }
            return;
        }
        return;
    }
    if (type_is_pointer(p_visitor->p_type))
    {
        struct flow_object * pointed;

        if (t_is_nullable || type_is_opt(p_visitor->p_type, nullable_enabled))
        {
            p_visitor->p_object->current.state = 6;
        }
        else
        {
            p_visitor->p_object->current.state = 4;
        }
        pointed = p_visitor->p_object->current.pointed;
        if (pointed)
        {
            struct type  t2;
            unsigned char   t2_is_nullable;
            struct object_visitor  visitor;

            t2 = type_remove_pointer(p_visitor->p_type);
            t2_is_nullable = type_is_opt(&t2, nullable_enabled);
            _cake_zmem(&visitor, 12);
            visitor.p_type = &t2;
            visitor.p_object = pointed;
            object_set_unknown_core(&visitor, t2_is_nullable, visit_number, nullable_enabled);
            type_destroy(&t2);
        }
    }
    else
    {
        if (!type_is_struct_or_union(p_visitor->p_type))
        {
            p_visitor->p_object->current.state = 96;
        }
    }
}

void flow_object_set_unknown(struct type * p_type, unsigned char   t_is_nullable, struct flow_object * p_object, unsigned char   nullable_enabled)
{
    struct object_visitor  visitor;

    _cake_zmem(&visitor, 12);
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    object_set_unknown_core(&visitor, t_is_nullable, s_visit_number++, nullable_enabled);
}

static void object_set_deleted_core(struct type * p_type, struct flow_object * p_object, unsigned int visit_number)
{
    if (p_object->visit_number == visit_number)
    {
        return;
    }
    p_object->visit_number = visit_number;
    if (p_type->struct_or_union_specifier && p_object->members.size > 0)
    {
        struct struct_or_union_specifier * p_struct_or_union_specifier;

        p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
        if (p_struct_or_union_specifier)
        {
            struct member_declaration * p_member_declaration;
            int member_index;

            p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;
            member_index = 0;
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
                            if (member_index < p_object->members.size)
                            {
                                object_set_deleted_core(&p_member_declarator->declarator->type, p_object->members.data[member_index], visit_number);
                            }
                            else
                            {
                            }
                            member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                else
                {
                    if (p_member_declaration->specifier_qualifier_list != 0U)
                    {
                    }
                }
                p_member_declaration = p_member_declaration->next;
            }
            return;
        }
        return;
    }
    if (type_is_pointer(p_type))
    {
        p_object->current.state = 128;
    }
    else
    {
        if (!type_is_struct_or_union(p_type))
        {
            p_object->current.state = 128;
        }
    }
}

void flow_object_set_can_be_uninitialized(struct flow_object * p_object)
{
    p_object->current.state |= 1;
}

void flow_object_set_is_unitialized(struct flow_object * p_object)
{
    p_object->current.state = 1;
    if (p_object->current.alternatives.size > 0)
    {
        {
            int i;
            i = 0;
            for (; i < p_object->current.alternatives.size; i++)
            {
                flow_object_set_can_be_uninitialized(p_object->current.alternatives.data[i]);
            }
        }
    }
}

void flow_object_set_can_be_moved(struct flow_object * p_object)
{
    if (p_object->current.state & 4)
    {
        p_object->current.state &= -5;
        p_object->current.state |= 8;
        p_object->current.state |= 8;
    }
}

void flow_object_set_is_moved(struct flow_object * p_object)
{
    p_object->current.state = 8;
    if (p_object->current.alternatives.size > 0)
    {
        {
            int i;
            i = 0;
            for (; i < p_object->current.alternatives.size; i++)
            {
                flow_object_set_can_be_moved(p_object->current.alternatives.data[i]);
            }
        }
    }
}

void object_set_deleted(struct type * p_type, struct flow_object * p_object)
{
    object_set_deleted_core(p_type, p_object, s_visit_number++);
}

void object_set_zero_core(struct object_visitor * p_visitor)
{
    if (p_visitor->p_type->struct_or_union_specifier)
    {
        struct struct_or_union_specifier * p_struct_or_union_specifier;

        p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);
        if (p_struct_or_union_specifier)
        {
            struct member_declaration * p_member_declaration;

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
                            if (p_visitor->member_index < p_visitor->p_object->members.size)
                            {
                                struct object_visitor  visitor;

                                _cake_zmem(&visitor, 12);
                                visitor.p_type = &p_member_declarator->declarator->type;
                                visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];
                                object_set_zero_core(&visitor);
                            }
                            p_visitor->member_index++;
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
                            struct type * temp;

                            _cake_zmem(&t, 60);
                            t.category = 0;
                            t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                            t.type_specifier_flags = 32768;
                            temp = p_visitor->p_type;
                            p_visitor->p_type = &t;
                            object_set_zero_core(p_visitor);
                            p_visitor->p_type = temp;
                            type_destroy(&t);
                        }
                    }
                }
                p_member_declaration = p_member_declaration->next;
            }
            return;
        }
        return;
    }
    if (type_is_pointer(p_visitor->p_type))
    {
        p_visitor->p_object->current.state = 2;
    }
    else
    {
        p_visitor->p_object->current.state = 32;
    }
}

void flow_object_set_zero(struct type * p_type, struct flow_object * p_object)
{
    struct object_visitor  visitor;

    _cake_zmem(&visitor, 12);
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    object_set_zero_core(&visitor);
}

void object_set_end_of_lifetime_core(struct object_visitor * p_visitor)
{
    if (p_visitor->p_type->struct_or_union_specifier)
    {
        struct struct_or_union_specifier * p_struct_or_union_specifier;

        p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);
        if (p_struct_or_union_specifier)
        {
            struct member_declaration * p_member_declaration;

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
                            if (p_visitor->member_index < p_visitor->p_object->members.size)
                            {
                                struct object_visitor  visitor;

                                _cake_zmem(&visitor, 12);
                                visitor.p_type = &p_member_declarator->declarator->type;
                                visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];
                                object_set_end_of_lifetime_core(&visitor);
                            }
                            p_visitor->member_index++;
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
                            struct type * temp;

                            _cake_zmem(&t, 60);
                            t.category = 0;
                            t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                            t.type_specifier_flags = 32768;
                            temp = p_visitor->p_type;
                            p_visitor->p_type = &t;
                            object_set_end_of_lifetime_core(p_visitor);
                            p_visitor->p_type = temp;
                            type_destroy(&t);
                        }
                    }
                }
                p_member_declaration = p_member_declaration->next;
            }
            return;
        }
        return;
    }
    p_visitor->p_object->current.state = 128;
}

void flow_object_set_end_of_lifetime(struct type * p_type, struct flow_object * p_object)
{
    struct object_visitor  visitor;

    _cake_zmem(&visitor, 12);
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    object_set_end_of_lifetime_core(&visitor);
}

unsigned char  type_is_owner(struct type * p_type);

unsigned char  object_check(struct type * p_type, struct flow_object * p_object)
{
    /*try*/ if (1)
    {
        if (p_type->type_qualifier_flags & 32)
        {
            return 0;
        }
        if (!type_is_owner_or_pointer_to_dtor(p_type))
        {
            return 0;
        }
        if (p_type->struct_or_union_specifier && p_object->members.size > 0)
        {
            struct struct_or_union_specifier * p_struct_or_union_specifier;
            struct member_declaration * p_member_declaration;
            int possible_need_destroy_count;
            int need_destroy_count;
            int member_index;

            p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
            if (p_struct_or_union_specifier == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;
            possible_need_destroy_count = 0;
            need_destroy_count = 0;
            member_index = 0;
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
                            if (type_is_owner(&p_member_declarator->declarator->type))
                            {
                                possible_need_destroy_count++;
                            }
                            if (object_check(&p_member_declarator->declarator->type, p_object->members.data[member_index]))
                            {
                                need_destroy_count++;
                            }
                            member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                else
                {
                    if (p_member_declaration->specifier_qualifier_list != 0U)
                    {
                    }
                }
                p_member_declaration = p_member_declaration->next;
            }
            return !!(need_destroy_count > 1 && (need_destroy_count == possible_need_destroy_count));
        }
        else
        {
            unsigned char   should_had_been_moved;

            should_had_been_moved = 0;
            if (type_is_pointer(p_type))
            {
                should_had_been_moved = !!((p_object->current.state & 4));
            }
            else
            {
                if (p_object->current.state == 1 || (p_object->current.state & 8) || p_object->current.state == 4 || p_object->current.state == 1)
                {
                }
                else
                {
                    should_had_been_moved = 1;
                }
            }
            return should_had_been_moved;
        }
    }
    /*catch*/ else _CKL0:
    {
    }
    return 0;
}

void object_get_name_core(struct type * p_type, struct flow_object * p_object, struct flow_object * p_object_target, char * previous_names, char * outname, int out_size, unsigned int visit_number)
{
    /*try*/ if (1)
    {
        if (p_object->visit_number == visit_number)
        {
            return;
        }
        ((struct flow_object *)p_object)->visit_number = visit_number;
        if (p_object == p_object_target)
        {
            snprintf(outname, out_size, "%s", previous_names);
            return;
        }
        if (p_type->struct_or_union_specifier && p_object->members.size > 0)
        {
            struct struct_or_union_specifier * p_struct_or_union_specifier;
            struct member_declaration * p_member_declaration;
            int member_index;

            p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
            if (p_struct_or_union_specifier == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;
            member_index = 0;
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
                            char * name;
                            char buffer[200];

                            name = p_member_declarator->declarator->name_opt ? p_member_declarator->declarator->name_opt->lexeme : "";
                            _cake_zmem(&buffer, 200);
                            if (type_is_pointer(p_type))
                            {
                                snprintf(buffer, 200U, "%s->%s", previous_names, name);
                            }
                            else
                            {
                                snprintf(buffer, 200U, "%s.%s", previous_names, name);
                            }
                            object_get_name_core(&p_member_declarator->declarator->type, p_object->members.data[member_index], p_object_target, buffer, outname, out_size, visit_number);
                            member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                else
                {
                    if (p_member_declaration->specifier_qualifier_list != 0U)
                    {
                    }
                }
                p_member_declaration = p_member_declaration->next;
            }
        }
        else
        {
            snprintf(outname, out_size, "%s", previous_names);
        }
    }
    /*catch*/ else _CKL0:
    {
    }
}

void object_get_name(struct type * p_type, struct flow_object * p_object, char * outname, int out_size)
{
    outname[0] = 0;
    if (p_object->p_declarator_origin)
    {
        char * root_name;
        struct flow_object * root;

        root_name = p_object->p_declarator_origin->name_opt ? p_object->p_declarator_origin->name_opt->lexeme : "?";
        root = p_object->p_declarator_origin->p_flow_object;
        object_get_name_core(&p_object->p_declarator_origin->type, root, p_object, root_name, outname, out_size, s_visit_number++);
    }
    else
    {
        if (p_object->p_expression_origin)
        {
            int bytes_written;
            struct token * p;

            bytes_written = 0;
            p = p_object->p_expression_origin->first_token;
            {
                int i;
                i = 0;
                for (; i < 10; i++)
                {
                    char * ps;

                    ps = p->lexeme;
                    while (*ps)
                    {
                        if (bytes_written < (out_size - 1))
                        {
                            outname[bytes_written] = *ps;
                        }
                        bytes_written++;
                        ps++;
                    }
                    if (p == p_object->p_expression_origin->last_token)
                    {
                        break;
                    }
                    p = p->next;
                    ;
                }
            }
            if (bytes_written < (out_size - 1))
            {
                outname[bytes_written] = 0;
            }
            else
            {
                outname[out_size - 1] = 0;
            }
        }
        else
        {
            outname[0] = 63;
            outname[1] = 0;
        }
    }
}

static void checked_read_object_core(struct flow_visit_ctx * ctx, struct object_visitor * p_visitor, unsigned char   is_nullable, struct token * position_token_opt, struct marker * p_marker_opt, unsigned char   check_pointed_object, char * previous_names, unsigned int visit_number)
{
    ;
    if (p_visitor->p_object->visit_number == visit_number)
    {
        return;
    }
    p_visitor->p_object->visit_number = visit_number;
    if (p_visitor->p_type->struct_or_union_specifier && p_visitor->p_object->members.size > 0)
    {
        struct struct_or_union_specifier * p_struct_or_union_specifier;
        struct member_declaration * p_member_declaration;

        p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);
        p_member_declaration = p_struct_or_union_specifier ? p_struct_or_union_specifier->member_declaration_list.head : 0U;
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
                        char * name;
                        char buffer[200];
                        struct object_visitor  visitor;

                        name = p_member_declarator->declarator->name_opt ? p_member_declarator->declarator->name_opt->lexeme : "?";
                        _cake_zmem(&buffer, 200);
                        if (type_is_pointer(p_visitor->p_type))
                        {
                            snprintf(buffer, 200U, "%s->%s", previous_names, name);
                        }
                        else
                        {
                            snprintf(buffer, 200U, "%s.%s", previous_names, name);
                        }
                        _cake_zmem(&visitor, 12);
                        visitor.p_type = &p_member_declarator->declarator->type;
                        visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];
                        checked_read_object_core(ctx, &visitor, is_nullable, position_token_opt, p_marker_opt, check_pointed_object, buffer, visit_number);
                        p_visitor->member_index++;
                    }
                    p_member_declarator = p_member_declarator->next;
                }
            }
            else
            {
                if (p_member_declaration->specifier_qualifier_list)
                {
                    struct type  t;
                    char buffer[200];
                    struct type * temp;
                    int visit_number0;

                    _cake_zmem(&t, 60);
                    t.category = 0;
                    t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                    t.type_specifier_flags = 32768;
                    _cake_zmem(&buffer, 200);
                    if (type_is_pointer(p_visitor->p_type))
                    {
                        snprintf(buffer, 200U, "%s->", previous_names);
                    }
                    else
                    {
                        snprintf(buffer, 200U, "%s.", previous_names);
                    }
                    temp = p_visitor->p_type;
                    p_visitor->p_type = &t;
                    visit_number0 = p_visitor->p_object->visit_number;
                    p_visitor->p_object->visit_number = 0;
                    checked_read_object_core(ctx, p_visitor, is_nullable, position_token_opt, p_marker_opt, check_pointed_object, buffer, visit_number);
                    p_visitor->p_type = temp;
                    p_visitor->p_object->visit_number = visit_number0;
                    type_destroy(&t);
                }
            }
            p_member_declaration = p_member_declaration->next;
        }
        return;
    }
    else
    {
        if (type_is_pointer(p_visitor->p_type) && !is_nullable && !type_is_opt(p_visitor->p_type, ctx->ctx->options.null_checks_enabled) && flow_object_can_be_null(p_visitor->p_object))
        {
            compiler_diagnostic(32, ctx->ctx, 0U, p_marker_opt, "non-nullable pointer '%s' may be null", previous_names);
        }
        if (type_is_pointer(p_visitor->p_type) && check_pointed_object && flow_object_can_be_not_null_or_moved(p_visitor->p_object))
        {
            struct type  t2;

            t2 = type_remove_pointer(p_visitor->p_type);
            if (p_visitor->p_object->current.pointed)
            {
                struct object_visitor  visitor;

                _cake_zmem(&visitor, 12);
                visitor.p_type = &t2;
                visitor.p_object = p_visitor->p_object->current.pointed;
                checked_read_object_core(ctx, &visitor, is_nullable, position_token_opt, p_marker_opt, 1, previous_names, visit_number);
            }
            type_destroy(&t2);
        }
        if (p_visitor->p_object->current.state & 1)
        {
            if (type_is_array(p_visitor->p_type))
            {
            }
            else
            {
                compiler_diagnostic(29, ctx->ctx, position_token_opt, p_marker_opt, "uninitialized object '%s'", previous_names);
            }
        }
        if (p_visitor->p_object->current.state & 128)
        {
        }
    }
}

void checked_read_object(struct flow_visit_ctx * ctx, struct type * p_type, unsigned char   is_nullable, struct flow_object * p_object, struct token * position_token, struct marker * p_marker_opt, unsigned char   check_pointed_object)
{
    char * s;
    char name[200];
    struct object_visitor  visitor;

    s = 0U;
    _cake_zmem(&name, 200);
    object_get_name(p_type, p_object, name, 200U);
    _cake_zmem(&visitor, 12);
    visitor.p_object = p_object;
    visitor.p_type = p_type;
    checked_read_object_core(ctx, &visitor, is_nullable, position_token, p_marker_opt, check_pointed_object, name, s_visit_number++);
    free((void *)s);
}

unsigned char  type_is_view(struct type * p_type);
unsigned char  type_is_pointed_dtor(struct type * p_type);

static void flow_end_of_block_visit_core(struct flow_visit_ctx * ctx, struct object_visitor * p_visitor, unsigned char   b_type_is_view, struct token * position_token, char * previous_names, unsigned int visit_number)
{
    if (p_visitor->p_object->visit_number == visit_number)
    {
        return;
    }
    p_visitor->p_object->visit_number = visit_number;
    if (p_visitor->p_type->struct_or_union_specifier && p_visitor->p_object->members.size > 0)
    {
        struct struct_or_union_specifier * p_struct_or_union_specifier;
        struct member_declaration * p_member_declaration;

        p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);
        if (p_struct_or_union_specifier == 0U)
        {
            return;
        }
        p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;
        if (object_check(p_visitor->p_type, p_visitor->p_object))
        {
            struct token * name;

            name = flow_object_get_token(p_visitor->p_object);
            if (compiler_diagnostic(28, ctx->ctx, name, 0U, "members of '%s' were not released.", previous_names))
            {
                if (p_visitor->p_object->p_declarator_origin)
                {
                    compiler_diagnostic(62, ctx->ctx, position_token, 0U, "end of '%s' scope", previous_names);
                }
            }
        }
        else
        {
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
                            char * name;
                            char buffer[200];
                            unsigned char   member_is_view;
                            struct object_visitor  visitor;

                            name = p_member_declarator->declarator->name_opt ? p_member_declarator->declarator->name_opt->lexeme : "?";
                            _cake_zmem(&buffer, 200);
                            if (type_is_pointer(p_visitor->p_type))
                            {
                                snprintf(buffer, 200U, "%s->%s", previous_names, name);
                            }
                            else
                            {
                                snprintf(buffer, 200U, "%s.%s", previous_names, name);
                            }
                            member_is_view = type_is_view(&p_member_declarator->declarator->type);
                            _cake_zmem(&visitor, 12);
                            visitor.p_type = &p_member_declarator->declarator->type;
                            visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];
                            flow_end_of_block_visit_core(ctx, &visitor, b_type_is_view || member_is_view, position_token, buffer, visit_number);
                            p_visitor->member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                else
                {
                    if (p_member_declaration->specifier_qualifier_list != 0U)
                    {
                        struct type  t;
                        struct type * temp;
                        unsigned char   member_is_view;
                        int visit_number0;

                        _cake_zmem(&t, 60);
                        t.category = 0;
                        t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                        t.type_specifier_flags = 32768;
                        temp = p_visitor->p_type;
                        p_visitor->p_type = &t;
                        member_is_view = type_is_view(&t);
                        visit_number0 = p_visitor->p_object->visit_number;
                        p_visitor->p_object->visit_number = 0;
                        flow_end_of_block_visit_core(ctx, p_visitor, member_is_view, position_token, previous_names, visit_number);
                        p_visitor->p_type = temp;
                        p_visitor->p_object->visit_number = visit_number0;
                        type_destroy(&t);
                    }
                }
                p_member_declaration = p_member_declaration->next;
            }
        }
    }
    else
    {
        char * name;
        struct token * position;

        name = previous_names;
        position = 0U;
        if (p_visitor->p_object->p_declarator_origin)
        {
            position = p_visitor->p_object->p_declarator_origin->name_opt ? p_visitor->p_object->p_declarator_origin->name_opt : p_visitor->p_object->p_declarator_origin->first_token_opt;
        }
        else
        {
            if (p_visitor->p_object->p_expression_origin)
            {
                position = p_visitor->p_object->p_expression_origin->first_token;
            }
            else
            {
                ;
            }
        }
        if (name[0] == 0)
        {
            name = "?";
        }
        if (type_is_pointer(p_visitor->p_type) && !b_type_is_view && type_is_owner(p_visitor->p_type) && p_visitor->p_object->current.state & 4)
        {
            if (compiler_diagnostic(28, ctx->ctx, position, 0U, "object pointed by '%s' was not released.", previous_names))
            {
                compiler_diagnostic(62, ctx->ctx, position_token, 0U, "end of '%s' lifetime", previous_names);
            }
        }
        else
        {
            if (!b_type_is_view && type_is_pointed_dtor(p_visitor->p_type) && type_is_pointer(p_visitor->p_type))
            {
                char buffer[100];
                struct type  t2;

                _cake_zmem(&buffer, 100);
                snprintf(buffer, 100U, "%s", previous_names);
                t2 = type_remove_pointer(p_visitor->p_type);
                if (p_visitor->p_object->current.pointed)
                {
                    struct object_visitor  visitor;

                    _cake_zmem(&visitor, 12);
                    visitor.p_type = &t2;
                    visitor.p_object = p_visitor->p_object->current.pointed;
                    flow_end_of_block_visit_core(ctx, &visitor, b_type_is_view, position, buffer, visit_number);
                }
                type_destroy(&t2);
            }
            else
            {
                if (type_is_owner(p_visitor->p_type) && !type_is_pointer(p_visitor->p_type))
                {
                    if (p_visitor->p_object->current.state == 1 || p_visitor->p_object->current.state == 2 || p_visitor->p_object->current.state == 8)
                    {
                    }
                    else
                    {
                        if (compiler_diagnostic(28, ctx->ctx, position, 0U, "object pointed by '%s' was not released.", previous_names))
                        {
                            compiler_diagnostic(62, ctx->ctx, position_token, 0U, "end of '%s' lifetime", previous_names);
                        }
                    }
                }
                else
                {
                    if (type_is_pointer(p_visitor->p_type))
                    {
                        if (p_visitor->p_type->storage_class_specifier_flags & 2048)
                        {
                            if (type_is_owner_or_pointer_to_dtor(p_visitor->p_type))
                            {
                            }
                            else
                            {
                                struct type  t2;

                                t2 = type_remove_pointer(p_visitor->p_type);
                                if (p_visitor->p_object->current.pointed && p_visitor->p_object->p_declarator_origin)
                                {
                                    struct token * name_token;

                                    name_token = p_visitor->p_object->p_declarator_origin->name_opt ? p_visitor->p_object->p_declarator_origin->name_opt : p_visitor->p_object->p_declarator_origin->first_token_opt;
                                    checked_read_object(ctx, &t2, 0, p_visitor->p_object->current.pointed, name_token, 0U, 1);
                                }
                                type_destroy(&t2);
                            }
                        }
                    }
                    else
                    {
                    }
                }
            }
        }
    }
}

void flow_end_of_block_visit(struct flow_visit_ctx * ctx, struct type * p_type, unsigned char   type_is_view, struct flow_object * p_object, struct token * position_token, char * previous_names)
{
    struct object_visitor  visitor;

    _cake_zmem(&visitor, 12);
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    flow_end_of_block_visit_core(ctx, &visitor, type_is_view, position_token, previous_names, s_visit_number++);
}

unsigned char  flow_object_is_zero_or_null(struct flow_object * p_object)
{
    return !!((p_object->current.state == 2) || (p_object->current.state == 32));
}

struct type get_array_item_type(struct type * p_type);
unsigned char  type_is_ctor(struct type * p_type);
unsigned char  type_is_const(struct type * p_type);
unsigned char  type_is_nullptr_t(struct type * p_type);
unsigned char  type_is_integer(struct type * p_type);
unsigned char  type_is_void_ptr(struct type * p_type);

static void flow_assignment_core(struct flow_visit_ctx * ctx, struct token * error_position, struct marker * p_a_marker, struct marker * p_b_marker, int assigment_type, unsigned char   check_uninitialized_b, unsigned char   a_type_is_view, unsigned char   a_type_is_nullable, struct object_visitor * p_visitor_a, struct object_visitor * p_visitor_b, unsigned char  * set_argument_to_unkown)
{
    if (check_uninitialized_b && flow_object_can_be_uninitialized(p_visitor_b->p_object))
    {
        if (type_is_array(p_visitor_b->p_type))
        {
            if (assigment_type == 1)
            {
                struct type  item_type;
                unsigned char   cannot_be_uninitialized;

                _cake_zmem(&item_type, 60);
                if (type_is_array(p_visitor_a->p_type))
                {
                    item_type = get_array_item_type(p_visitor_a->p_type);
                }
                else
                {
                    item_type = type_remove_pointer(p_visitor_a->p_type);
                }
                cannot_be_uninitialized = !!((ctx->ctx->options.ownership_enabled && !type_is_ctor(&item_type)) || type_is_const(&item_type));
                if (cannot_be_uninitialized)
                {
                    char b_object_name[100];

                    _cake_zmem(&b_object_name, 100);
                    object_get_name(p_visitor_b->p_type, p_visitor_b->p_object, b_object_name, 100U);
                    compiler_diagnostic(29, ctx->ctx, 0U, p_b_marker, "uninitialized object '%s' passed to non-optional parameter", b_object_name);
                }
                type_destroy(&item_type);
            }
        }
        else
        {
            char b_object_name[100];

            _cake_zmem(&b_object_name, 100);
            object_get_name(p_visitor_b->p_type, p_visitor_b->p_object, b_object_name, 100U);
            if (assigment_type == 1)
            {
                compiler_diagnostic(29, ctx->ctx, 0U, p_b_marker, "passing an uninitialized argument '%s' object", b_object_name);
            }
            else
            {
                if (assigment_type == 0)
                {
                    compiler_diagnostic(29, ctx->ctx, 0U, p_b_marker, "returning an uninitialized '%s' object", b_object_name);
                }
                else
                {
                    compiler_diagnostic(29, ctx->ctx, 0U, p_b_marker, "reading an uninitialized '%s' object", b_object_name);
                }
            }
        }
        return;
    }
    if (check_uninitialized_b && flow_object_can_have_its_lifetime_ended(p_visitor_a->p_object))
    {
        char buffer[100];

        _cake_zmem(&buffer, 100);
        object_get_name(p_visitor_a->p_type, p_visitor_a->p_object, buffer, 100U);
        compiler_diagnostic(30, ctx->ctx, 0U, p_a_marker, "The object '%s' may have been deleted or its lifetime have ended.", buffer);
        return;
    }
    if (type_is_pointer(p_visitor_a->p_type) && (!type_is_opt(p_visitor_a->p_type, ctx->ctx->options.null_checks_enabled)) && flow_object_can_be_null(p_visitor_b->p_object))
    {
        if (!a_type_is_nullable)
        {
            char buffer[100];

            _cake_zmem(&buffer, 100);
            object_get_name(p_visitor_b->p_type, p_visitor_b->p_object, buffer, 100U);
            if (assigment_type == 1)
            {
                compiler_diagnostic(34, ctx->ctx, 0U, p_b_marker, "passing a possible null pointer '%s' to non-nullable pointer parameter", buffer);
            }
            else
            {
                if (assigment_type == 0)
                {
                    compiler_diagnostic(34, ctx->ctx, 0U, p_b_marker, "returning a possible null pointer '%s' to non-nullable pointer", buffer);
                }
                else
                {
                    compiler_diagnostic(34, ctx->ctx, 0U, p_b_marker, "assignment of possible null pointer '%s' to non-nullable pointer", buffer);
                }
            }
        }
    }
    if (type_is_pointer(p_visitor_a->p_type))
    {
        if (!a_type_is_view && type_is_owner(p_visitor_a->p_type))
        {
            checked_empty(ctx, p_visitor_a->p_type, p_visitor_a->p_object, p_a_marker);
        }
        if (flow_object_is_zero_or_null(p_visitor_b->p_object))
        {
            if (type_is_array(p_visitor_b->p_type))
            {
                p_visitor_a->p_object->current.state = 4;
                return;
            }
            else
            {
                if (type_is_nullptr_t(p_visitor_b->p_type) || type_is_integer(p_visitor_b->p_type))
                {
                    flow_object_set_current_state_to_is_null(p_visitor_a->p_object);
                    return;
                }
            }
        }
    }
    if (!a_type_is_view && type_is_pointed_dtor(p_visitor_a->p_type) && type_is_pointer(p_visitor_a->p_type))
    {
        checked_empty(ctx, p_visitor_a->p_type, p_visitor_a->p_object, p_a_marker);
        if (flow_object_is_zero_or_null(p_visitor_b->p_object))
        {
            return;
        }
    }
    if (type_is_void_ptr(p_visitor_a->p_type) && type_is_pointer(p_visitor_b->p_type))
    {
        p_visitor_a->p_object->current.state = p_visitor_b->p_object->current.state;
        if (!a_type_is_view && type_is_owner(p_visitor_a->p_type))
        {
            struct type  t;

            if (flow_object_can_be_moved(p_visitor_b->p_object))
            {
                compiler_diagnostic(31, ctx->ctx, 0U, p_b_marker, "object may be already moved");
            }
            t = type_remove_pointer(p_visitor_b->p_type);
            if (p_visitor_b->p_object->current.pointed == 0U)
            {
                if (flow_object_is_expansible(p_visitor_b->p_object) && type_is_owner(&t))
                {
                    compiler_diagnostic(28, ctx->ctx, 0U, p_a_marker, "pointed object may be not empty");
                }
            }
            else
            {
                checked_empty(ctx, &t, p_visitor_b->p_object->current.pointed, p_b_marker);
                object_set_deleted(&t, p_visitor_b->p_object->current.pointed);
            }
            type_destroy(&t);
            if (assigment_type == 1)
            {
                flow_object_set_is_unitialized(p_visitor_b->p_object);
                p_visitor_b->p_object->current.state = 1;
            }
            else
            {
                flow_object_set_is_moved(p_visitor_b->p_object);
            }
        }
        return;
    }
    if (type_is_pointer(p_visitor_a->p_type) && type_is_pointer(p_visitor_b->p_type))
    {
        struct type  t;
        unsigned char   checked_pointed_object_read;
        unsigned char   is_nullable;

        p_visitor_a->p_object->current.state = p_visitor_b->p_object->current.state;
        p_visitor_a->p_object->current.pointed = p_visitor_b->p_object->current.pointed;
        t = type_remove_pointer(p_visitor_a->p_type);
        checked_pointed_object_read = !!(ctx->ctx->options.ownership_enabled && !type_is_ctor(&t));
        is_nullable = !!(a_type_is_nullable || type_is_opt(&t, ctx->ctx->options.null_checks_enabled));
        checked_read_object(ctx, p_visitor_b->p_type, is_nullable, p_visitor_b->p_object, error_position, p_b_marker, checked_pointed_object_read);
        type_destroy(&t);
        if (!a_type_is_view && type_is_owner(p_visitor_a->p_type))
        {
            if (flow_object_can_be_moved(p_visitor_b->p_object))
            {
                compiler_diagnostic(31, ctx->ctx, error_position, 0U, "source object has already been moved");
            }
            if (assigment_type == 1)
            {
                p_visitor_b->p_object->current.state = 1;
                if (p_visitor_b->p_object->current.pointed)
                {
                    struct flow_object * pointed;
                    struct type  t2;

                    pointed = p_visitor_b->p_object->current.pointed;
                    t2 = type_remove_pointer(p_visitor_b->p_type);
                    object_set_deleted(&t2, pointed);
                    type_destroy(&t2);
                }
            }
            else
            {
                p_visitor_a->p_object->current.state = p_visitor_a->p_object->current.state & -9;
                if (p_visitor_b->p_object->current.state & 4)
                {
                    p_visitor_b->p_object->current.state &= -5;
                    p_visitor_b->p_object->current.state |= 8;
                }
            }
        }
        else
        {
            if (!a_type_is_view && type_is_pointed_dtor(p_visitor_a->p_type))
            {
                if (type_is_owner_or_pointer_to_dtor(p_visitor_b->p_type))
                {
                    if (assigment_type == 1)
                    {
                        if (p_visitor_b->p_object->current.pointed)
                        {
                            struct flow_object * pointed;
                            struct type  t2;

                            pointed = p_visitor_b->p_object->current.pointed;
                            t2 = type_remove_pointer(p_visitor_b->p_type);
                            flow_object_set_uninitialized(&t2, pointed);
                            type_destroy(&t2);
                        }
                    }
                    else
                    {
                        flow_object_set_moved(p_visitor_b->p_type, p_visitor_b->p_object);
                    }
                }
                else
                {
                    if (p_visitor_b->p_type->address_of)
                    {
                        if (assigment_type == 1)
                        {
                            p_visitor_b->p_object->current.state = 1;
                            if (p_visitor_b->p_object->current.pointed)
                            {
                                struct flow_object * pointed;
                                struct type  t2;

                                pointed = p_visitor_b->p_object->current.pointed;
                                t2 = type_remove_pointer(p_visitor_b->p_type);
                                flow_object_set_uninitialized(&t2, pointed);
                                type_destroy(&t2);
                            }
                        }
                        else
                        {
                            flow_object_set_moved(p_visitor_b->p_type, p_visitor_b->p_object);
                        }
                    }
                    else
                    {
                    }
                }
            }
            else
            {
                if (a_type_is_view || !type_is_owner(p_visitor_a->p_type))
                {
                    p_visitor_a->p_object->current.state = p_visitor_b->p_object->current.state;
                }
                if (assigment_type == 1)
                {
                    struct type  t3;

                    t3 = type_remove_pointer(p_visitor_a->p_type);
                    if (!type_is_const(&t3))
                    {
                        if (p_visitor_b->p_object->current.pointed)
                        {
                            if (set_argument_to_unkown)
                            {
                                *set_argument_to_unkown = 1;
                            }
                        }
                    }
                    type_destroy(&t3);
                }
            }
        }
        return;
    }
    if (p_visitor_a->p_type->struct_or_union_specifier && p_visitor_a->p_object->members.size > 0)
    {
        struct struct_or_union_specifier * p_a_struct_or_union_specifier;
        struct struct_or_union_specifier * p_b_struct_or_union_specifier;

        p_a_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_visitor_a->p_type->struct_or_union_specifier);
        if (p_visitor_b->p_type->struct_or_union_specifier == 0U)
        {
            return;
        }
        p_b_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_visitor_b->p_type->struct_or_union_specifier);
        if (p_a_struct_or_union_specifier && p_b_struct_or_union_specifier)
        {
            struct member_declaration * p_a_member_declaration;
            struct member_declaration * p_b_member_declaration;

            p_a_member_declaration = p_a_struct_or_union_specifier->member_declaration_list.head;
            p_b_member_declaration = p_b_struct_or_union_specifier->member_declaration_list.head;
            while (p_a_member_declaration && p_b_member_declaration)
            {
                if (p_a_member_declaration->member_declarator_list_opt && p_b_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator * p_a_member_declarator;
                    struct member_declarator * p_b_member_declarator;

                    p_a_member_declarator = p_a_member_declaration->member_declarator_list_opt->head;
                    p_b_member_declarator = p_b_member_declaration->member_declarator_list_opt->head;
                    while (p_a_member_declarator && p_b_member_declarator)
                    {
                        if (p_a_member_declarator->declarator && p_b_member_declarator->declarator)
                        {
                            if (p_visitor_a->member_index < p_visitor_a->p_object->members.size && p_visitor_b->member_index < p_visitor_b->p_object->members.size)
                            {
                                struct object_visitor  visitor_a;
                                struct object_visitor  visitor_b;

                                _cake_zmem(&visitor_a, 12);
                                visitor_a.p_type = &p_a_member_declarator->declarator->type;
                                visitor_a.p_object = p_visitor_a->p_object->members.data[p_visitor_a->member_index];
                                _cake_zmem(&visitor_b, 12);
                                visitor_b.p_type = &p_b_member_declarator->declarator->type;
                                visitor_b.p_object = p_visitor_b->p_object->members.data[p_visitor_b->member_index];
                                flow_assignment_core(ctx, error_position, p_a_marker, p_b_marker, assigment_type, check_uninitialized_b, a_type_is_view, a_type_is_nullable, &visitor_a, &visitor_b, set_argument_to_unkown);
                            }
                            else
                            {
                            }
                            p_visitor_a->member_index++;
                            p_visitor_b->member_index++;
                        }
                        p_a_member_declarator = p_a_member_declarator->next;
                        p_b_member_declarator = p_b_member_declarator->next;
                    }
                }
                else
                {
                    if (p_a_member_declaration->specifier_qualifier_list && p_a_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                    {
                        struct type  t;
                        struct type * temp1;
                        struct type * temp2;

                        _cake_zmem(&t, 60);
                        t.category = 0;
                        t.struct_or_union_specifier = p_a_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                        t.type_specifier_flags = 32768;
                        temp1 = p_visitor_a->p_type;
                        temp2 = p_visitor_b->p_type;
                        p_visitor_a->p_type = &t;
                        p_visitor_b->p_type = &t;
                        flow_assignment_core(ctx, error_position, p_a_marker, p_b_marker, assigment_type, check_uninitialized_b, a_type_is_view, a_type_is_nullable, p_visitor_a, p_visitor_b, set_argument_to_unkown);
                        p_visitor_a->p_type = temp1;
                        p_visitor_b->p_type = temp2;
                        type_destroy(&t);
                    }
                }
                p_a_member_declaration = p_a_member_declaration->next;
                p_b_member_declaration = p_b_member_declaration->next;
            }
            return;
        }
    }
    p_visitor_a->p_object->current.state = p_visitor_b->p_object->current.state;
    if (!a_type_is_view && type_is_owner(p_visitor_a->p_type))
    {
        if (assigment_type == 1)
        {
            flow_object_set_uninitialized(p_visitor_b->p_type, p_visitor_b->p_object);
        }
        else
        {
            flow_object_set_moved(p_visitor_b->p_type, p_visitor_b->p_object);
        }
    }
}

unsigned char  object_has_constant_value(struct object * a);
unsigned char  object_to_bool(struct object * a);

struct flow_object *expression_get_flow_object(struct flow_visit_ctx * ctx, struct expression * p_expression, unsigned char   nullable_enabled)
{
    /*try*/ if (1)
    {
        if (p_expression->expression_type == 2)
        {
            ;
            if (p_expression->declarator->p_alias_of_expression)
            {
                return expression_get_flow_object(ctx, p_expression->declarator->p_alias_of_expression, nullable_enabled);
            }
            else
            {
                ;
                if (p_expression->declarator->declaration_specifiers && !(p_expression->declarator->declaration_specifiers->storage_class_specifier_flags & 4096))
                {
                    ;
                    if (flow_objects_find(&ctx->arena, p_expression->declarator->p_flow_object) == 0U)
                    {
                        p_expression->declarator->p_flow_object = make_flow_object(ctx, &p_expression->declarator->type, p_expression->declarator, 0U);
                        if (p_expression->declarator->p_flow_object == 0U)
                        {
                            /*throw*/ goto _CKL0;
                        }
                        flow_object_set_unknown(&p_expression->declarator->type, type_is_opt(&p_expression->declarator->type, ctx->ctx->options.null_checks_enabled), p_expression->declarator->p_flow_object, ctx->ctx->options.null_checks_enabled);
                    }
                }
                return p_expression->declarator->p_flow_object;
            }
        }
        else
        {
            if (p_expression->expression_type == 33)
            {
                struct flow_object * p_object;
                struct flow_object * p_object_pointed;

                ;
                p_object = make_flow_object(ctx, &p_expression->type, 0U, p_expression);
                if (p_object == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
                p_object_pointed = expression_get_flow_object(ctx, p_expression->right, nullable_enabled);
                if (p_object_pointed)
                {
                    object_set_pointer(p_object, p_object_pointed);
                }
                p_object->current.state = 4;
                p_object->is_temporary = 1;
                return p_object;
            }
            else
            {
                if (p_expression->expression_type == 9)
                {
                    ;
                    return expression_get_flow_object(ctx, p_expression->right, nullable_enabled);
                }
                else
                {
                    if (p_expression->expression_type == 34)
                    {
                        struct flow_object * p;

                        ;
                        p = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
                        if (p)
                        {
                            if (type_is_pointer(&p_expression->type_name->type))
                            {
                                if (p->current.state & 32)
                                {
                                    p->current.state &= -33;
                                    p->current.state |= 2;
                                }
                                if (p->current.state & 64)
                                {
                                    p->current.state &= -65;
                                    p->current.state |= 4;
                                }
                            }
                        }
                        return p;
                    }
                    else
                    {
                        if (p_expression->expression_type == 14)
                        {
                            struct flow_object * p_obj;

                            ;
                            p_obj = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
                            if (p_obj)
                            {
                                if (p_expression->member_index < p_obj->members.size)
                                {
                                    return p_obj->members.data[p_expression->member_index];
                                }
                                else
                                {
                                    return 0U;
                                }
                            }
                        }
                        else
                        {
                            if (p_expression->expression_type == 13)
                            {
                                struct flow_object * p_obj;

                                ;
                                p_obj = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
                                if (p_obj)
                                {
                                    if (p_obj->current.pointed == 0U)
                                    {
                                        flow_object_expand_pointer(ctx, &p_expression->left->type, p_obj);
                                    }
                                    if (p_obj->current.pointed)
                                    {
                                        struct flow_object * pointed;

                                        pointed = p_obj->current.pointed;
                                        return pointed;
                                    }
                                }
                                return 0U;
                            }
                            else
                            {
                                if (p_expression->expression_type == 15)
                                {
                                    struct flow_object * p_obj;

                                    ;
                                    p_obj = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
                                    if (p_obj)
                                    {
                                        struct flow_object * pointed;
                                        struct flow_object * p_obj2;

                                        if (p_obj->current.pointed == 0)
                                        {
                                            flow_object_expand_pointer(ctx, &p_expression->left->type, p_obj);
                                        }
                                        pointed = p_obj->current.pointed;
                                        if (pointed == 0U || p_expression->member_index >= pointed->members.size)
                                        {
                                            return 0U;
                                        }
                                        p_obj2 = pointed->members.data[p_expression->member_index];
                                        p_obj2->p_declarator_origin = 0U;
                                        p_obj2->p_expression_origin = p_expression;
                                        return p_obj2;
                                    }
                                    return 0U;
                                }
                                else
                                {
                                    if (p_expression->expression_type == 32)
                                    {
                                        struct flow_object * p_obj;

                                        ;
                                        p_obj = expression_get_flow_object(ctx, p_expression->right, nullable_enabled);
                                        if (p_obj)
                                        {
                                            if (p_obj->current.pointed == 0U)
                                            {
                                                flow_object_expand_pointer(ctx, &p_expression->right->type, p_obj);
                                            }
                                            if (p_obj->current.pointed != 0U)
                                            {
                                                return p_obj->current.pointed;
                                            }
                                        }
                                        return p_obj;
                                    }
                                    else
                                    {
                                        if (p_expression->expression_type == 12)
                                        {
                                            struct flow_object * p_object;
                                            unsigned char   is_nullable;

                                            p_object = make_flow_object(ctx, &p_expression->type, 0U, p_expression);
                                            if (p_object == 0U)
                                            {
                                                /*throw*/ goto _CKL0;
                                            }
                                            is_nullable = type_is_opt(&p_expression->type, nullable_enabled);
                                            flow_object_set_unknown(&p_expression->type, is_nullable, p_object, nullable_enabled);
                                            p_object->is_temporary = 1;
                                            if (type_is_pointer(&p_expression->type) && flow_object_is_expansible(p_object))
                                            {
                                                flow_object_expand_pointer(ctx, &p_expression->type, p_object);
                                            }
                                            return p_object;
                                        }
                                        else
                                        {
                                            if (p_expression->expression_type == 11)
                                            {
                                                ;
                                                return p_expression->type_name->abstract_declarator->p_flow_object;
                                            }
                                            else
                                            {
                                                if (p_expression->expression_type == 3)
                                                {
                                                    struct flow_object * p_object;

                                                    p_object = make_flow_object(ctx, &p_expression->type, 0U, p_expression);
                                                    if (p_object == 0U)
                                                    {
                                                        /*throw*/ goto _CKL0;
                                                    }
                                                    p_object->current.state = 4;
                                                    return p_object;
                                                }
                                                else
                                                {
                                                    if (p_expression->expression_type == 6)
                                                    {
                                                        struct flow_object * p_object;

                                                        p_object = make_flow_object(ctx, &p_expression->type, 0U, p_expression);
                                                        if (p_object == 0U)
                                                        {
                                                            /*throw*/ goto _CKL0;
                                                        }
                                                        if (p_expression->type.type_specifier_flags == 16777216)
                                                        {
                                                            p_object->current.state = 2;
                                                        }
                                                        else
                                                        {
                                                            if (object_has_constant_value(&p_expression->object))
                                                            {
                                                                unsigned char   not_zero;

                                                                not_zero = object_to_bool(&p_expression->object);
                                                                p_object->current.state = not_zero ? 64 : 32;
                                                            }
                                                        }
                                                        return p_object;
                                                    }
                                                    else
                                                    {
                                                        if (p_expression->expression_type == 53)
                                                        {
                                                            struct flow_object * p_obj;

                                                            ;
                                                            p_obj = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
                                                            return p_obj;
                                                        }
                                                        else
                                                        {
                                                            if (p_expression->expression_type == 65)
                                                            {
                                                                struct flow_object * p_object;
                                                                struct flow_object * p_obj1;
                                                                struct flow_object * p_obj2;

                                                                ;
                                                                ;
                                                                p_object = make_flow_object(ctx, &p_expression->type, 0U, p_expression);
                                                                if (p_object == 0U)
                                                                {
                                                                    /*throw*/ goto _CKL0;
                                                                }
                                                                p_obj1 = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
                                                                p_obj2 = expression_get_flow_object(ctx, p_expression->right, nullable_enabled);
                                                                if (p_obj1 && p_obj2)
                                                                {
                                                                    flow_object_merge_state(p_object, p_obj1, p_obj2);
                                                                }
                                                                return p_object;
                                                            }
                                                            else
                                                            {
                                                                if (p_expression->expression_type == 46 || p_expression->expression_type == 47)
                                                                {
                                                                    struct flow_object * p_object;

                                                                    p_object = make_flow_object(ctx, &p_expression->type, 0U, p_expression);
                                                                    if (p_object == 0U)
                                                                    {
                                                                        /*throw*/ goto _CKL0;
                                                                    }
                                                                    if (object_has_constant_value(&p_expression->object))
                                                                    {
                                                                        unsigned char   not_zero;

                                                                        not_zero = object_to_bool(&p_expression->object);
                                                                        p_object->current.state = not_zero ? 64 : 32;
                                                                    }
                                                                    else
                                                                    {
                                                                        p_object->current.state = 96;
                                                                    }
                                                                    return p_object;
                                                                }
                                                                else
                                                                {
                                                                    if (p_expression->expression_type == 38)
                                                                    {
                                                                        struct flow_object * p_object;

                                                                        p_object = make_flow_object(ctx, &p_expression->type, 0U, p_expression);
                                                                        if (p_object == 0U)
                                                                        {
                                                                            /*throw*/ goto _CKL0;
                                                                        }
                                                                        if (type_is_pointer(&p_expression->type))
                                                                        {
                                                                            p_object->current.state = 4;
                                                                        }
                                                                        else
                                                                        {
                                                                            if (object_has_constant_value(&p_expression->object))
                                                                            {
                                                                                unsigned char   not_zero;

                                                                                not_zero = object_to_bool(&p_expression->object);
                                                                                p_object->current.state = not_zero ? 4 : 2;
                                                                            }
                                                                            else
                                                                            {
                                                                                p_object->current.state = 6;
                                                                            }
                                                                        }
                                                                        return p_object;
                                                                    }
                                                                    else
                                                                    {
                                                                        if (p_expression->expression_type == 30 || p_expression->expression_type == 31)
                                                                        {
                                                                            struct flow_object * p_obj_right;
                                                                            struct flow_object * p_object;

                                                                            ;
                                                                            p_obj_right = expression_get_flow_object(ctx, p_expression->right, nullable_enabled);
                                                                            p_object = make_flow_object(ctx, &p_expression->type, 0U, p_expression);
                                                                            if (p_object == 0U)
                                                                            {
                                                                                /*throw*/ goto _CKL0;
                                                                            }
                                                                            if (p_obj_right)
                                                                            {
                                                                                p_object->current.state = p_obj_right->current.state;
                                                                            }
                                                                            return p_object;
                                                                        }
                                                                        else
                                                                        {
                                                                            struct flow_object * p_object;

                                                                            p_object = make_flow_object(ctx, &p_expression->type, 0U, p_expression);
                                                                            if (p_object == 0U)
                                                                            {
                                                                                /*throw*/ goto _CKL0;
                                                                            }
                                                                            if (type_is_pointer(&p_expression->type))
                                                                            {
                                                                                if (object_has_constant_value(&p_expression->object))
                                                                                {
                                                                                    unsigned char   not_zero;

                                                                                    not_zero = object_to_bool(&p_expression->object);
                                                                                    p_object->current.state = not_zero ? 4 : 2;
                                                                                }
                                                                                else
                                                                                {
                                                                                    p_object->current.state = 6;
                                                                                }
                                                                            }
                                                                            else
                                                                            {
                                                                                if (object_has_constant_value(&p_expression->object))
                                                                                {
                                                                                    unsigned char   not_zero;

                                                                                    not_zero = object_to_bool(&p_expression->object);
                                                                                    p_object->current.state = not_zero ? 64 : 32;
                                                                                }
                                                                                else
                                                                                {
                                                                                    p_object->current.state = 96;
                                                                                }
                                                                            }
                                                                            return p_object;
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
    /*catch*/ else _CKL0:
    {
    }
    return 0U;
}

void flow_check_assignment(struct flow_visit_ctx * ctx, struct token * error_position, struct marker * p_a_marker, struct marker * p_b_marker, int assigment_type, unsigned char   check_uninitialized_b, unsigned char   a_type_is_view, unsigned char   a_type_is_nullable, struct type * p_a_type, struct flow_object * p_a_object, struct type * p_b_type, struct flow_object * p_b_object, unsigned char  * set_argument_to_unkown)
{
    struct object_visitor  visitor_a;
    struct object_visitor  visitor_b;

    if (type_is_pointer(p_b_type) && flow_object_is_expansible(p_b_object))
    {
    }
    visitor_a.member_index = 0;
    visitor_a.p_type = p_a_type;
    visitor_a.p_object = p_a_object;
    visitor_b.member_index = 0;
    visitor_b.p_type = p_b_type;
    visitor_b.p_object = p_b_object;
    flow_assignment_core(ctx, error_position, p_a_marker, p_b_marker, assigment_type, check_uninitialized_b, a_type_is_view, a_type_is_nullable, &visitor_a, &visitor_b, set_argument_to_unkown);
}

int ss_fprintf(struct osstream * stream, char * fmt, ...);
void ss_close(struct osstream * stream);

void print_object_state_to_str(int e, char str[], int sz)
{
    unsigned char   first;
    struct osstream  ss;

    first = 1;
    _cake_zmem(&ss, 12);
    if (e == 0)
    {
        ss_fprintf(&ss, "--");
    }
    if (e & 1)
    {
        if (first)
        {
            first = 0;
        }
        else
        {
            ss_fprintf(&ss, ",");
        }
        ss_fprintf(&ss, "unset");
    }
    if (e & 4)
    {
        if (first)
        {
            first = 0;
        }
        else
        {
            ss_fprintf(&ss, ",");
        }
        ss_fprintf(&ss, "not-null");
    }
    if (e & 2)
    {
        if (first)
        {
            first = 0;
        }
        else
        {
            ss_fprintf(&ss, ",");
        }
        ss_fprintf(&ss, "null");
    }
    if (e & 32)
    {
        if (first)
        {
            first = 0;
        }
        else
        {
            ss_fprintf(&ss, ",");
        }
        ss_fprintf(&ss, "zero");
    }
    if (e & 64)
    {
        if (first)
        {
            first = 0;
        }
        else
        {
            ss_fprintf(&ss, ",");
        }
        ss_fprintf(&ss, "not-zero");
    }
    if (e & 128)
    {
        if (first)
        {
            first = 0;
        }
        else
        {
            ss_fprintf(&ss, ",");
        }
        ss_fprintf(&ss, "lifetime-ended");
    }
    if (e & 8)
    {
        if (first)
        {
            first = 0;
        }
        else
        {
            ss_fprintf(&ss, ",");
        }
        ss_fprintf(&ss, "moved");
    }
    snprintf(str, sz, "%s", ss.c_str);
    ss_close(&ss);
}

void flow_object_state_print(struct flow_object_state * p_state)
{
    struct osstream  ss;
    char temp[200];

    _cake_zmem(&ss, 12);
    _cake_zmem(&temp, 200);
    print_object_state_to_str(p_state->state, temp, 200U);
    ss_fprintf(&ss, "%d %s", p_state->state_number, temp);
    if (p_state->pointed)
    {
        ss_fprintf(&ss, " ->%d", p_state->pointed->id);
    }
    if (p_state->alternatives.size > 0)
    {
        ss_fprintf(&ss, " ");
    }
    {
        int i;
        i = 0;
        for (; i < p_state->alternatives.size; i++)
        {
            if (i != 0)
            {
                ss_fprintf(&ss, ",");
            }
            ss_fprintf(&ss, "%d", p_state->alternatives.data[i]->id);
        }
    }
    printf("%-25s\xe2\x94\x82", ss.c_str);
    ss_close(&ss);
}

void print_object_line(struct flow_object * p_object, int extra_cols)
{
    struct osstream  ss;
    int cols;
    struct flow_object_state * p_state;

    _cake_zmem(&ss, 12);
    if (p_object->parent)
    {
        ss_fprintf(&ss, "\xe2\x86\x91%d", p_object->parent->id);
        printf("\xe2\x94\x82%-2d\xe2\x94\x82", p_object->id);
        printf("%-20s\xe2\x94\x82", ss.c_str);
    }
    else
    {
        int line;
        int col;

        line = 0;
        col = 0;
        if (p_object->p_declarator_origin)
        {
            if (p_object->p_declarator_origin->name_opt)
            {
                line = p_object->p_declarator_origin->name_opt->line;
                col = p_object->p_declarator_origin->name_opt->col;
                ss_fprintf(&ss, "%2d:%2d ", line, col);
                ss_fprintf(&ss, "%s", p_object->p_declarator_origin->name_opt->lexeme);
            }
            else
            {
                ss_fprintf(&ss, "%2d:%2d ", line, col);
                ss_fprintf(&ss, "%s", "?");
            }
        }
        else
        {
            if (p_object->p_expression_origin)
            {
                line = p_object->p_expression_origin->first_token->line;
                col = p_object->p_expression_origin->first_token->col;
                ss_fprintf(&ss, "%2d:%2d ", line, col);
                ss_fprintf(&ss, "%s", p_object->p_expression_origin->first_token->lexeme);
            }
            else
            {
                ss_fprintf(&ss, "&");
            }
        }
        printf("\xe2\x94\x82%-2d\xe2\x94\x82", p_object->id);
        printf("%-18s\xe2\x94\x82", ss.c_str);
    }
    ss_close(&ss);
    cols = 0;
    p_state = &p_object->current;
    while (p_state)
    {
        cols++;
        flow_object_state_print(p_state);
        p_state = p_state->next;
    }
    {
        int i;
        i = 0;
        for (; i <= extra_cols - cols; i++)
        {
            printf("%-25s\xe2\x94\x82", " ");
        }
    }
    printf("\n");
}

static void true_false_set_clear(struct true_false_set * p)
{
    free(p->data);
    p->data = 0U;
    p->size = 0;
    p->capacity = 0;
}

void true_false_set_destroy(struct true_false_set * p)
{
    free(p->data);
}

static int true_false_set_reserve(struct true_false_set * p, int n)
{
    if (n > p->capacity)
    {
        void * pnew;

        if ((unsigned int)n > 357913941U)
        {
            return 132;
        }
        pnew = realloc(p->data, n * 12U);
        if (pnew == 0U)
        {
            return 12;
        }
        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}

static int true_false_set_push_back(struct true_false_set * p, struct true_false_set_item * book)
{
    if (p->size == 2147483647)
    {
        return 132;
    }
    if (p->size + 1 > p->capacity)
    {
        int new_capacity;
        int error;

        new_capacity = 0;
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
        error = true_false_set_reserve(p, new_capacity);
        if (error != 0)
        {
            return error;
        }
    }
    p->data[p->size] = *book;
    p->size++;
    return 0;
}

static void true_false_set_invert(struct true_false_set * true_false_set)
{
    {
        int i;
        i = 0;
        for (; i < true_false_set->size; i++)
        {
            int temp;

            temp = true_false_set->data[i].true_branch_state;
            true_false_set->data[i].true_branch_state = true_false_set->data[i].false_branch_state;
            true_false_set->data[i].false_branch_state = temp;
        }
    }
}

static void true_false_set_swap(struct true_false_set * a, struct true_false_set * b)
{
    struct true_false_set  temp;

    temp = *a;
    *a = *b;
    *b = temp;
}

static int find_item_index_by_expression(struct true_false_set * a, struct expression * p_expression)
{
    {
        int i;
        i = 0;
        for (; i < a->size; i++)
        {
            if (a->data[i].p_expression == p_expression)
            {
                return i;
            }
        }
    }
    return -1;
}

static void true_false_set_merge(struct true_false_set * result, struct true_false_set * a, struct true_false_set * b, int options_true, int options_false)
{
    {
        int i;
        i = 0;
        for (; i < a->size; i++)
        {
            struct true_false_set_item * p_item_a;
            struct true_false_set_item  new_item;

            p_item_a = &a->data[i];
            _cake_zmem(&new_item, 12);
            new_item.p_expression = p_item_a->p_expression;
            if (options_true & 1)
            {
                new_item.true_branch_state |= p_item_a->true_branch_state;
            }
            if (options_true & 2)
            {
                new_item.true_branch_state |= p_item_a->false_branch_state;
            }
            if (options_false & 1)
            {
                new_item.false_branch_state |= p_item_a->true_branch_state;
            }
            if (options_false & 2)
            {
                new_item.true_branch_state |= p_item_a->false_branch_state;
            }
            true_false_set_push_back(result, &new_item);
        }
    }
    {
        int k;
        k = 0;
        for (; k < b->size; k++)
        {
            struct true_false_set_item * p_item_b;
            int index;
            struct true_false_set_item * p_item_result;

            p_item_b = &b->data[k];
            index = find_item_index_by_expression(result, p_item_b->p_expression);
            if (index == -1)
            {
                struct true_false_set_item  item2;

                index = result->size;
                _cake_zmem(&item2, 12);
                item2.p_expression = p_item_b->p_expression;
                true_false_set_push_back(result, &item2);
            }
            p_item_result = &result->data[index];
            if (options_true & 4)
            {
                p_item_result->true_branch_state |= p_item_b->true_branch_state;
            }
            if (options_true & 8)
            {
                p_item_result->true_branch_state |= p_item_b->false_branch_state;
            }
            if (options_false & 4)
            {
                p_item_result->false_branch_state |= p_item_b->true_branch_state;
            }
            if (options_false & 8)
            {
                p_item_result->true_branch_state |= p_item_b->false_branch_state;
            }
        }
    }
}

static void true_false_set_set_objects_to_core_branch(struct flow_visit_ctx * ctx, struct true_false_set * a, unsigned char   nullable_enabled, unsigned char   true_branch)
{
    {
        int i;
        i = 0;
        for (; i < a->size; i++)
        {
            struct flow_object * p_object;

            ;
            p_object = expression_get_flow_object(ctx, a->data[i].p_expression, nullable_enabled);
            if (p_object)
            {
                int flag;

                if (p_object->current.state == 4 || p_object->current.state == 2 || p_object->current.state == 8 || p_object->current.state == 32 || p_object->current.state == 64 || p_object->current.state == 128)
                {
                    continue;
                }
                flag = true_branch ? a->data[i].true_branch_state : a->data[i].false_branch_state;
                if ((flag & 1) && (flag & 2))
                {
                }
                else
                {
                    if (flag & 2)
                    {
                        p_object->current.state &= -5;
                        p_object->current.state &= -9;
                    }
                    else
                    {
                        if (flag & 1)
                        {
                            p_object->current.state &= -3;
                            p_object->current.state &= -33;
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

static void flow_exit_block_visit_defer_item(struct flow_visit_ctx * ctx, struct defer_list_item * p_item, struct token * position_token)
{
    if (p_item->defer_statement)
    {
        int error_count;
        int warnings_count;
        int info_count;

        error_count = ctx->ctx->p_report->error_count;
        warnings_count = ctx->ctx->p_report->warnings_count;
        info_count = ctx->ctx->p_report->info_count;
        flow_visit_secondary_block(ctx, p_item->defer_statement->secondary_block);
        if (error_count != ctx->ctx->p_report->error_count || warnings_count != ctx->ctx->p_report->warnings_count || info_count != ctx->ctx->p_report->info_count)
        {
            compiler_diagnostic(62, ctx->ctx, position_token, 0U, "defer end of scope");
        }
    }
    else
    {
        if (p_item->declarator)
        {
            struct declarator * p_declarator;
            char * name;

            p_declarator = p_item->declarator;
            name = p_declarator->name_opt ? p_declarator->name_opt->lexeme : "?";
            if (p_declarator->p_flow_object)
            {
                flow_end_of_block_visit(ctx, &p_declarator->type, type_is_view(&p_declarator->type), p_declarator->p_flow_object, position_token, name);
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
    struct defer_list_item * p_item;

    p_item = p_defer_list->head;
    while (p_item)
    {
        flow_exit_block_visit_defer_item(ctx, p_item, position_token);
        p_item = p_item->next;
    }
}

static void flow_defer_item_set_end_of_lifetime(struct flow_visit_ctx * ctx, struct defer_list_item * p_item, struct token * position_token)
{
    if (p_item->defer_statement)
    {
    }
    else
    {
        if (p_item->declarator)
        {
            struct declarator * p_declarator;

            p_declarator = p_item->declarator;
            if (p_declarator->p_flow_object)
            {
                flow_object_set_end_of_lifetime(&p_declarator->type, p_declarator->p_flow_object);
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
    struct defer_list_item * p_item;

    p_item = p_defer_list->head;
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

static int arena_add_copy_of_current_state(struct flow_visit_ctx * ctx, char * name)
{
    int state_number;

    state_number = ctx->state_number_generator;
    ctx->state_number_generator++;
    {
        int i;
        i = 0;
        for (; i < ctx->arena.size; i++)
        {
            flow_object_add_new_state_as_a_copy_of_current_state(ctx->arena.data[i], name, state_number);
        }
    }
    return state_number;
}

static int arena_add_empty_state(struct flow_visit_ctx * ctx, char * name)
{
    int state_number;

    state_number = ctx->state_number_generator;
    ctx->state_number_generator++;
    {
        int i;
        i = 0;
        for (; i < ctx->arena.size; i++)
        {
            struct flow_object * p_object;
            struct flow_object_state * p_flow_object_state;

            p_object = ctx->arena.data[i];
            p_flow_object_state = calloc(1, 32U);
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

static void flow_object_set_state_from_current(struct flow_object * object, int state_number)
{
    struct flow_object_state * p_flow_object_state;

    p_flow_object_state = object->current.next;
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
        int i;
        i = 0;
        for (; i < ctx->arena.size; i++)
        {
            struct flow_object * p_object;

            p_object = ctx->arena.data[i];
            flow_object_set_state_from_current(p_object, number_state);
        }
    }
}

static int flow_object_merge_current_with_state(struct flow_visit_ctx * ctx, struct flow_object * object, int state_number)
{
    /*try*/ if (1)
    {
        struct flow_object_state * it;

        it = object->current.next;
        while (it)
        {
            if (it->state_number == state_number)
            {
                it->state |= object->current.state;
                if (object->current.pointed == 0U && it->pointed == 0U)
                {
                }
                else
                {
                    if (object->current.pointed == 0U && it->pointed != 0U)
                    {
                    }
                    else
                    {
                        if (object->current.pointed != 0U && it->pointed == 0U)
                        {
                            it->pointed = object->current.pointed;
                        }
                        else
                        {
                            if (object->current.pointed != it->pointed)
                            {
                                struct flow_object * p_new_object;
                                struct flow_object_state * p_new_state;
                                int n_childs_1;
                                int n_childs_2;

                                ;
                                ;
                                p_new_object = arena_new_object(ctx);
                                if (p_new_object == 0U)
                                {
                                    /*throw*/ goto _CKL0;
                                }
                                p_new_state = calloc(1, 32U);
                                if (p_new_state == 0U)
                                {
                                    /*throw*/ goto _CKL0;
                                }
                                p_new_state->dbg_name = "merged";
                                p_new_state->state_number = state_number;
                                objects_view_push_back(&p_new_state->alternatives, object->current.pointed);
                                objects_view_push_back(&p_new_state->alternatives, it->pointed);
                                flow_object_add_state(p_new_object, p_new_state);
                                n_childs_1 = object->current.pointed->members.size;
                                n_childs_2 = it->pointed->members.size;
                                if (n_childs_1 == n_childs_2)
                                {
                                    {
                                        int j;
                                        j = 0;
                                        for (; j < n_childs_1; j++)
                                        {
                                            struct flow_object * p_new_child;
                                            struct flow_object * child1;
                                            struct flow_object * child2;
                                            struct flow_object_state * p_child_new_state;

                                            p_new_child = arena_new_object(ctx);
                                            if (p_new_child == 0U)
                                            {
                                                /*throw*/ goto _CKL0;
                                            }
                                            p_new_child->parent = p_new_object;
                                            child1 = object->current.pointed->members.data[j];
                                            child2 = it->pointed->members.data[j];
                                            p_child_new_state = calloc(1, 32U);
                                            if (p_child_new_state == 0U)
                                            {
                                                /*throw*/ goto _CKL0;
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
    /*catch*/ else _CKL0:
    {
    }
    return 0;
}

static void arena_merge_current_state_with_state_number(struct flow_visit_ctx * ctx, int number_state)
{
    {
        int i;
        i = 0;
        for (; i < ctx->arena.size; i++)
        {
            flow_object_merge_current_with_state(ctx, ctx->arena.data[i], number_state);
        }
    }
}

static void object_restore_current_state_from(struct flow_visit_ctx * ctx, struct flow_object * object, int state_number)
{
    struct flow_object_state * it;

    it = object->current.next;
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
        int i;
        i = 0;
        for (; i < ctx->arena.size; i++)
        {
            object_restore_current_state_from(ctx, ctx->arena.data[i], number_state);
        }
    }
}

static void arena_remove_state(struct flow_visit_ctx * ctx, int state_number)
{
    {
        int i;
        i = 0;
        for (; i < ctx->arena.size; i++)
        {
            flow_object_remove_state(ctx->arena.data[i], state_number);
        }
    }
}

static void braced_initializer_flow_core(struct flow_visit_ctx * ctx, struct object * obj, struct flow_object * flow_obj)
{
    unsigned char   nullable_enabled;

    nullable_enabled = ctx->ctx->options.null_checks_enabled;
    if (obj->p_init_expression)
    {
        struct flow_object * p_right_object;

        p_right_object = expression_get_flow_object(ctx, obj->p_init_expression, nullable_enabled);
        if (p_right_object)
        {
            struct marker  a_marker;
            struct marker  b_marker;

            _cake_zmem(&a_marker, 28);
            b_marker.file = 0;
            b_marker.line = 0;
            b_marker.start_col = 0;
            b_marker.end_col = 0;
            b_marker.p_token_caret = 0;
            b_marker.p_token_begin = obj->p_init_expression->first_token;
            b_marker.p_token_end = obj->p_init_expression->last_token;
            flow_check_assignment(ctx, obj->p_init_expression->first_token, &a_marker, &b_marker, 2, 0, type_is_view(&obj->type), type_is_opt(&obj->type, ctx->ctx->options.null_checks_enabled), &obj->type, flow_obj, &obj->p_init_expression->type, p_right_object, 0U);
        }
    }
    else
    {
        flow_object_set_zero(&obj->type, flow_obj);
    }
    if (flow_obj->members.size > 0)
    {
        int i;
        struct object * m;

        i = 0;
        m = obj->members;
        while (m)
        {
            braced_initializer_flow_core(ctx, m, flow_obj->members.data[i]);
            m = m->next;
            i++;
        }
    }
}

static void braced_initializer_flow(struct flow_visit_ctx * ctx, struct object * obj, struct flow_object * flow_obj)
{
    /*try*/ if (1)
    {
        unsigned char   is_nullable;
        struct marker  a_marker;

        braced_initializer_flow_core(ctx, obj, flow_obj);
        if (flow_obj->p_declarator_origin == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        is_nullable = type_is_opt(&obj->type, ctx->ctx->options.null_checks_enabled);
        a_marker.file = 0;
        a_marker.line = 0;
        a_marker.start_col = 0;
        a_marker.end_col = 0;
        a_marker.p_token_caret = 0;
        a_marker.p_token_begin = flow_obj->p_declarator_origin->first_token_opt;
        a_marker.p_token_end = flow_obj->p_declarator_origin->last_token_opt;
        checked_read_object(ctx, &obj->type, is_nullable, flow_obj, flow_obj->p_declarator_origin->first_token_opt, &a_marker, 0);
    }
    /*catch*/ else _CKL0:
    {
    }
}

static void flow_visit_declarator(struct flow_visit_ctx * ctx, struct declarator * p_declarator);
static void flow_visit_expression(struct flow_visit_ctx * ctx, struct expression * p_expression, struct true_false_set * a);
static void flow_visit_bracket_initializer_list(struct flow_visit_ctx * ctx, struct braced_initializer * p_bracket_initializer_list);
unsigned char  expression_is_malloc(struct expression * p);
unsigned char  expression_is_calloc(struct expression * p);

static void flow_visit_init_declarator(struct flow_visit_ctx * ctx, struct init_declarator * p_init_declarator)
{
    unsigned char   nullable_enabled;

    nullable_enabled = ctx->ctx->options.null_checks_enabled;
    /*try*/ if (1)
    {
        flow_visit_declarator(ctx, p_init_declarator->p_declarator);
        if (p_init_declarator->initializer)
        {
            if (p_init_declarator->initializer->assignment_expression)
            {
                struct true_false_set  a;

                _cake_zmem(&a, 12);
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
                struct flow_object * p_right_object;

                p_right_object = expression_get_flow_object(ctx, p_init_declarator->initializer->assignment_expression, nullable_enabled);
                if (p_right_object)
                {
                    struct marker  a_marker;
                    struct marker  b_marker;

                    a_marker.file = 0;
                    a_marker.line = 0;
                    a_marker.start_col = 0;
                    a_marker.end_col = 0;
                    a_marker.p_token_caret = 0;
                    a_marker.p_token_begin = p_init_declarator->p_declarator->first_token_opt;
                    a_marker.p_token_end = p_init_declarator->p_declarator->last_token_opt;
                    b_marker.file = 0;
                    b_marker.line = 0;
                    b_marker.start_col = 0;
                    b_marker.end_col = 0;
                    b_marker.p_token_caret = 0;
                    b_marker.p_token_begin = p_init_declarator->initializer->assignment_expression->first_token;
                    b_marker.p_token_end = p_init_declarator->initializer->assignment_expression->last_token;
                    ;
                    flow_check_assignment(ctx, p_init_declarator->initializer->assignment_expression->first_token, &a_marker, &b_marker, 2, 0, type_is_view(&p_init_declarator->p_declarator->type), type_is_opt(&p_init_declarator->p_declarator->type, ctx->ctx->options.null_checks_enabled), &p_init_declarator->p_declarator->type, p_init_declarator->p_declarator->p_flow_object, &p_init_declarator->initializer->assignment_expression->type, p_right_object, 0U);
                }
                if (expression_is_malloc(p_init_declarator->initializer->assignment_expression))
                {
                    struct type  t;
                    struct flow_object * po;

                    ;
                    t = type_remove_pointer(&p_init_declarator->p_declarator->type);
                    po = make_flow_object(ctx, &t, p_init_declarator->p_declarator, 0U);
                    if (po == 0U)
                    {
                        type_destroy(&t);
                        /*throw*/ goto _CKL0;
                    }
                    object_set_pointer(p_init_declarator->p_declarator->p_flow_object, po);
                    type_destroy(&t);
                    p_init_declarator->p_declarator->p_flow_object->current.state = 6;
                }
                else
                {
                    if (expression_is_calloc(p_init_declarator->initializer->assignment_expression))
                    {
                        struct type  t;
                        struct flow_object * pointed_calloc_object;
                        struct marker  a_marker;

                        ;
                        t = type_remove_pointer(&p_init_declarator->p_declarator->type);
                        pointed_calloc_object = make_flow_object(ctx, &t, p_init_declarator->p_declarator, 0U);
                        if (pointed_calloc_object == 0U)
                        {
                            type_destroy(&t);
                            /*throw*/ goto _CKL0;
                        }
                        flow_object_set_zero(&t, pointed_calloc_object);
                        object_set_pointer(p_init_declarator->p_declarator->p_flow_object, pointed_calloc_object);
                        a_marker.file = 0;
                        a_marker.line = 0;
                        a_marker.start_col = 0;
                        a_marker.end_col = 0;
                        a_marker.p_token_caret = 0;
                        a_marker.p_token_begin = p_init_declarator->p_declarator->first_token_opt;
                        a_marker.p_token_end = p_init_declarator->p_declarator->last_token_opt;
                        checked_read_object(ctx, &t, type_is_opt(&t, ctx->ctx->options.null_checks_enabled), pointed_calloc_object, p_init_declarator->p_declarator->first_token_opt, &a_marker, 0);
                        type_destroy(&t);
                        p_init_declarator->p_declarator->p_flow_object->current.state = 6;
                    }
                }
            }
            else
            {
                if (p_init_declarator->initializer && p_init_declarator->initializer->braced_initializer)
                {
                    ;
                    braced_initializer_flow(ctx, &p_init_declarator->p_declarator->object, p_init_declarator->p_declarator->p_flow_object);
                }
                else
                {
                    if (p_init_declarator->p_declarator->declaration_specifiers && ((!(p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & 4096)) || (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & 4)))
                    {
                        ;
                        flow_object_set_zero(&p_init_declarator->p_declarator->type, p_init_declarator->p_declarator->p_flow_object);
                    }
                    else
                    {
                        ;
                        flow_object_set_uninitialized(&p_init_declarator->p_declarator->type, p_init_declarator->p_declarator->p_flow_object);
                    }
                }
            }
        }
    }
    /*catch*/ else _CKL0:
    {
    }
}

static void flow_visit_declaration_specifiers(struct flow_visit_ctx * ctx, struct declaration_specifiers * p_declaration_specifiers, struct type * p_type);
static void flow_visit_init_declarator_list(struct flow_visit_ctx * ctx, struct init_declarator_list * p_init_declarator_list);

static void flow_visit_simple_declaration(struct flow_visit_ctx * ctx, struct simple_declaration * p_simple_declaration)
{
    flow_visit_declaration_specifiers(ctx, p_simple_declaration->p_declaration_specifiers, 0U);
    flow_visit_init_declarator_list(ctx, &p_simple_declaration->init_declarator_list);
}

void print_arena(struct flow_visit_ctx * ctx)
{
    int extra_cols;

    extra_cols = 0;
    {
        int i;
        i = 0;
        for (; i < ctx->arena.size; i++)
        {
            struct flow_object * p_object;
            struct flow_object_state * p_state;
            int count;

            p_object = ctx->arena.data[i];
            p_state = p_object->current.next;
            count = 0;
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
            int i;
            i = 0;
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
        int i;
        i = 0;
        for (; i < ctx->arena.size; i++)
        {
            struct flow_object * p;

            p = ctx->arena.data[i];
            print_object_line(p, extra_cols);
        }
    }
    printf("\xe2\x94\x94\xe2\x94\x80\xe2\x94\x80\xe2\x94\xb4\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\xb4\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80");
    if (extra_cols > 0)
    {
        {
            int i;
            i = 0;
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
    unsigned char   nullable_enabled;
    struct expression  hidden_expression;
    struct true_false_set  true_false_set;
    int before_if_state_number;
    unsigned char   true_branch_ends_with_jump;
    int left_true_branch_state_number;
    unsigned char   else_ends_with_jump;

    nullable_enabled = ctx->ctx->options.null_checks_enabled;
    if (p_selection_statement->p_init_statement && p_selection_statement->p_init_statement->p_expression_statement)
    {
        flow_visit_expression_statement(ctx, p_selection_statement->p_init_statement->p_expression_statement);
    }
    if (p_selection_statement->p_init_statement && p_selection_statement->p_init_statement->p_simple_declaration)
    {
        flow_visit_simple_declaration(ctx, p_selection_statement->p_init_statement->p_simple_declaration);
    }
    _cake_zmem(&hidden_expression, 224);
    _cake_zmem(&true_false_set, 12);
    if (p_selection_statement->condition && p_selection_statement->condition->expression)
    {
        flow_check_pointer_used_as_bool(ctx, p_selection_statement->condition->expression);
        flow_visit_expression(ctx, p_selection_statement->condition->expression, &true_false_set);
    }
    if (p_selection_statement->condition && p_selection_statement->condition->p_init_declarator)
    {
        flow_visit_init_declarator(ctx, p_selection_statement->condition->p_init_declarator);
    }
    if (p_selection_statement->condition && p_selection_statement->condition->expression == 0U && p_selection_statement->condition->p_init_declarator != 0U)
    {
        hidden_expression.expression_type = 2;
        hidden_expression.declarator = p_selection_statement->condition->p_init_declarator->p_declarator;
        ;
        hidden_expression.first_token = p_selection_statement->condition->p_init_declarator->p_declarator->first_token_opt;
        hidden_expression.last_token = hidden_expression.first_token;
        flow_visit_expression(ctx, &hidden_expression, &true_false_set);
    }
    ;
    before_if_state_number = arena_add_copy_of_current_state(ctx, "before-if");
    true_false_set_set_objects_to_true_branch(ctx, &true_false_set, nullable_enabled);
    flow_visit_secondary_block(ctx, p_selection_statement->secondary_block);
    true_branch_ends_with_jump = secondary_block_ends_with_jump(p_selection_statement->secondary_block);
    left_true_branch_state_number = arena_add_copy_of_current_state(ctx, "left-true-branch");
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
    else_ends_with_jump = !!(p_selection_statement->else_secondary_block_opt ? secondary_block_ends_with_jump(p_selection_statement->else_secondary_block_opt) : 0);
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
    /*try*/ if (1)
    {
        int throw_join_state_old;
        struct secondary_block * catch_secondary_block_old;
        int original_state_number;
        unsigned char   try_reached_the_end;
        unsigned char   catch_reached_the_end;

        throw_join_state_old = ctx->throw_join_state;
        catch_secondary_block_old = ctx->catch_secondary_block_opt;
        ctx->catch_secondary_block_opt = p_try_statement->catch_secondary_block_opt;
        ctx->throw_join_state = arena_add_empty_state(ctx, "try");
        original_state_number = arena_add_copy_of_current_state(ctx, "original");
        flow_visit_secondary_block(ctx, p_try_statement->secondary_block);
        arena_set_state_from_current(ctx, original_state_number);
        if (p_try_statement->catch_secondary_block_opt)
        {
            arena_restore_current_state_from(ctx, ctx->throw_join_state);
            flow_visit_secondary_block(ctx, p_try_statement->catch_secondary_block_opt);
        }
        try_reached_the_end = !!(!secondary_block_ends_with_jump(p_try_statement->secondary_block));
        catch_reached_the_end = !!(!secondary_block_ends_with_jump(p_try_statement->catch_secondary_block_opt));
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
    /*catch*/ else _CKL0:
    {
    }
}

static void flow_visit_switch_statement(struct flow_visit_ctx * ctx, struct selection_statement * p_selection_statement)
{
    int old_initial_state;
    int old_break_join_state;
    unsigned char   reached_the_end;

    old_initial_state = ctx->initial_state;
    old_break_join_state = ctx->break_join_state;
    ctx->initial_state = arena_add_copy_of_current_state(ctx, "original");
    ctx->break_join_state = arena_add_empty_state(ctx, "break join");
    flow_visit_secondary_block(ctx, p_selection_statement->secondary_block);
    reached_the_end = !!(!secondary_block_ends_with_jump(p_selection_statement->secondary_block));
    if (!reached_the_end)
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
    if (p_bracket_initializer_list->initializer_list == 0U)
    {
    }
    else
    {
        flow_visit_initializer_list(ctx, p_bracket_initializer_list->initializer_list);
    }
}

static void flow_visit_initializer(struct flow_visit_ctx * ctx, struct initializer * p_initializer);

static void flow_visit_initializer_list_item(struct flow_visit_ctx * ctx, struct initializer_list_item * p_initializer)
{
    ;
    flow_visit_initializer(ctx, p_initializer->initializer);
}

static void flow_visit_initializer(struct flow_visit_ctx * ctx, struct initializer * p_initializer)
{
    if (p_initializer->assignment_expression)
    {
        struct true_false_set  a;

        _cake_zmem(&a, 12);
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
    struct initializer_list_item * p_initializer;

    p_initializer = p_initializer_list->head;
    while (p_initializer)
    {
        flow_visit_initializer_list_item(ctx, p_initializer);
        p_initializer = p_initializer->next;
    }
}

static void flow_visit_generic_selection(struct flow_visit_ctx * ctx, struct generic_selection * p_generic_selection)
{
    if (p_generic_selection->expression)
    {
        struct true_false_set  a;

        _cake_zmem(&a, 12);
        flow_visit_expression(ctx, p_generic_selection->expression, &a);
        true_false_set_destroy(&a);
    }
}

struct param_list *type_get_func_or_func_ptr_params(struct type * p_type);
void diagnostic_remove(struct diagnostic * d, int w);

static void flow_compare_function_arguments(struct flow_visit_ctx * ctx, struct type * p_type, struct argument_expression_list * p_argument_expression_list)
{
    /*try*/ if (1)
    {
        unsigned char   nullable_enabled;
        struct param_list * p_param_list;
        struct param * p_current_parameter_type;
        struct argument_expression * p_current_argument;

        nullable_enabled = ctx->ctx->options.null_checks_enabled;
        p_param_list = type_get_func_or_func_ptr_params(p_type);
        if (p_param_list == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_current_parameter_type = p_param_list->head;
        p_current_argument = p_argument_expression_list->head;
        while (p_current_argument && p_current_parameter_type)
        {
            struct true_false_set  a;
            struct diagnostic  temp;
            struct flow_object * p_argument_object;

            _cake_zmem(&a, 12);
            temp = ctx->ctx->options.diagnostic_stack.stack[ctx->ctx->options.diagnostic_stack.top_index];
            diagnostic_remove(&ctx->ctx->options.diagnostic_stack.stack[ctx->ctx->options.diagnostic_stack.top_index], 29);
            flow_visit_expression(ctx, p_current_argument->expression, &a);
            ctx->ctx->options.diagnostic_stack.stack[ctx->ctx->options.diagnostic_stack.top_index] = temp;
            true_false_set_destroy(&a);
            p_argument_object = expression_get_flow_object(ctx, p_current_argument->expression, nullable_enabled);
            if (p_argument_object)
            {
                struct flow_object * parameter_object;
                struct marker  a_marker;
                struct marker  b_marker;

                parameter_object = make_flow_object(ctx, &p_current_parameter_type->type, 0U, p_current_argument->expression);
                if (parameter_object == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
                flow_object_set_uninitialized(&p_current_parameter_type->type, parameter_object);
                a_marker.file = 0;
                a_marker.line = 0;
                a_marker.start_col = 0;
                a_marker.end_col = 0;
                a_marker.p_token_caret = 0;
                a_marker.p_token_begin = p_current_argument->expression->first_token;
                a_marker.p_token_end = p_current_argument->expression->last_token;
                b_marker.file = 0;
                b_marker.line = 0;
                b_marker.start_col = 0;
                b_marker.end_col = 0;
                b_marker.p_token_caret = 0;
                b_marker.p_token_begin = p_current_argument->expression->first_token;
                b_marker.p_token_end = p_current_argument->expression->last_token;
                flow_check_assignment(ctx, p_current_argument->expression->first_token, &a_marker, &b_marker, 1, 1, type_is_view(&p_current_parameter_type->type), type_is_opt(&p_current_parameter_type->type, ctx->ctx->options.null_checks_enabled), &p_current_parameter_type->type, parameter_object, &p_current_argument->expression->type, p_argument_object, &p_current_argument->set_unkown);
            }
            p_current_argument = p_current_argument->next;
            p_current_parameter_type = p_current_parameter_type->next;
        }
        while (p_current_argument)
        {
            struct flow_object * p_argument_object;

            p_argument_object = expression_get_flow_object(ctx, p_current_argument->expression, nullable_enabled);
            if (p_argument_object)
            {
                struct marker  marker;

                marker.file = 0;
                marker.line = 0;
                marker.start_col = 0;
                marker.end_col = 0;
                marker.p_token_caret = 0;
                marker.p_token_begin = p_current_argument->expression->first_token;
                marker.p_token_end = p_current_argument->expression->last_token;
                checked_read_object(ctx, &p_current_argument->expression->type, type_is_opt(&p_current_argument->expression->type, ctx->ctx->options.null_checks_enabled), p_argument_object, p_current_argument->expression->first_token, &marker, 0);
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
                struct type  pointed_type;
                struct flow_object * p_argument_object;

                pointed_type = type_remove_pointer(&p_current_argument->expression->type);
                p_argument_object = expression_get_flow_object(ctx, p_current_argument->expression, nullable_enabled);
                if (p_argument_object)
                {
                    unsigned char   argument_type_is_nullable;

                    argument_type_is_nullable = type_is_opt(&pointed_type, ctx->ctx->options.null_checks_enabled);
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
            struct flow_object * p_argument_object;

            p_argument_object = expression_get_flow_object(ctx, p_current_argument->expression, nullable_enabled);
            if (p_argument_object)
            {
            }
            else
            {
            }
            p_current_argument = p_current_argument->next;
        }
    }
    /*catch*/ else _CKL0:
    {
    }
}

unsigned char  type_is_function(struct type * p_type);

static void check_uninitialized(struct flow_visit_ctx * ctx, struct expression * p_expression)
{
    unsigned char   nullable_enabled;
    struct flow_object * p_object;

    if (p_expression->is_assignment_expression)
    {
        return;
    }
    nullable_enabled = ctx->ctx->options.null_checks_enabled;
    if (type_is_function(&p_expression->type) || type_is_void(&p_expression->type) || type_is_nullptr_t(&p_expression->type))
    {
        return;
    }
    p_object = expression_get_flow_object(ctx, p_expression, nullable_enabled);
    if (!ctx->expression_is_not_evaluated)
    {
        if (p_object && p_object->current.state == 1)
        {
            if (p_expression->expression_type == 2 && p_expression->declarator && p_expression->declarator->name_opt)
            {
                compiler_diagnostic(29, ctx->ctx, p_expression->first_token, 0U, "using a uninitialized object '%s'", p_expression->declarator->name_opt->lexeme);
            }
            else
            {
                compiler_diagnostic(29, ctx->ctx, p_expression->first_token, 0U, "using a uninitialized object");
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
                        compiler_diagnostic(29, ctx->ctx, p_expression->first_token, 0U, "object '%s' can be uninitialized ", p_expression->declarator->name_opt->lexeme);
                    }
                    else
                    {
                        compiler_diagnostic(29, ctx->ctx, p_expression->first_token, 0U, "maybe using a uninitialized object");
                    }
                }
            }
        }
    }
}

void flow_object_push_states_from(struct flow_object * p_object_from, struct flow_object * p_object_to)
{
    struct flow_object_state * it_from;

    it_from = p_object_from->current.next;
    while (it_from)
    {
        it_from = it_from->next;
    }
    {
        int i;
        i = 0;
        for (; i < p_object_to->members.size; i++)
        {
            flow_object_push_states_from(p_object_from, p_object_to->members.data[i]);
        }
    }
}

static void flow_check_pointer_used_as_bool(struct flow_visit_ctx * ctx, struct expression * p_expression)
{
    unsigned char   nullable_enabled;

    nullable_enabled = ctx->ctx->options.null_checks_enabled;
    if (type_is_pointer(&p_expression->type))
    {
        struct flow_object * p_object;

        p_object = expression_get_flow_object(ctx, p_expression, nullable_enabled);
        if (p_object)
        {
            struct marker  marker;

            marker.file = 0;
            marker.line = 0;
            marker.start_col = 0;
            marker.end_col = 0;
            marker.p_token_caret = 0;
            marker.p_token_begin = p_expression->first_token;
            marker.p_token_end = p_expression->last_token;
            if (!ctx->inside_loop && flow_object_is_null(p_object))
            {
                compiler_diagnostic(27, ctx->ctx, 0U, &marker, "pointer is always null");
            }
            else
            {
                if (!ctx->inside_loop && flow_object_is_not_null(p_object))
                {
                    compiler_diagnostic(27, ctx->ctx, 0U, &marker, "pointer is always not-null");
                }
            }
        }
    }
}

static void arena_broadcast_change(struct flow_visit_ctx * ctx, struct flow_object * p)
{
    {
        int i;
        i = 0;
        for (; i < ctx->arena.size; i++)
        {
            struct flow_object * p_obj;

            p_obj = ctx->arena.data[i];
            {
                int j;
                j = 0;
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

static struct argument_expression *param_list_find_argument_by_name(struct param_list * p_param_list, struct argument_expression_list * list, char * name)
{
    struct param * p_param;
    struct argument_expression * p_argument_expression;

    p_param = p_param_list->head;
    p_argument_expression = list->head;
    while (p_param && p_argument_expression)
    {
        if (p_param->type.name_opt && strcmp(p_param->type.name_opt, name) == 0)
        {
            return p_argument_expression;
        }
        p_argument_expression = p_argument_expression->next;
        p_param = p_param->next;
    }
    return 0U;
}

static void flow_clear_alias(struct expression * p_expression)
{
    struct argument_expression * p;

    if (p_expression->declarator)
    {
        p_expression->declarator->p_alias_of_expression = 0U;
    }
    p = p_expression->argument_expression_list.head;
    while (p)
    {
        flow_clear_alias(p->expression);
        p = p->next;
    }
    if (p_expression->left)
    {
        flow_clear_alias(p_expression->left);
    }
    if (p_expression->right)
    {
        flow_clear_alias(p_expression->right);
    }
}

static void flow_expression_bind(struct flow_visit_ctx * ctx, struct expression * p_expression, struct param_list * p_param_list, struct argument_expression_list * p_argument_expression_list)
{
    struct argument_expression * p;

    if (p_expression->expression_type == 2)
    {
        struct argument_expression * p_argument_expression;

        ;
        ;
        p_argument_expression = param_list_find_argument_by_name(p_param_list, p_argument_expression_list, p_expression->declarator->name_opt->lexeme);
        if (p_argument_expression)
        {
            if (p_argument_expression->expression->declarator && p_argument_expression->expression->declarator->p_alias_of_expression)
            {
                p_expression->declarator->p_alias_of_expression = p_argument_expression->expression->declarator->p_alias_of_expression;
            }
            else
            {
                p_expression->declarator->p_alias_of_expression = p_argument_expression->expression;
            }
            return;
        }
    }
    p = p_expression->argument_expression_list.head;
    while (p)
    {
        flow_expression_bind(ctx, p->expression, p_param_list, p_argument_expression_list);
        p = p->next;
    }
    if (p_expression->left)
    {
        flow_expression_bind(ctx, p_expression->left, p_param_list, p_argument_expression_list);
    }
    if (p_expression->right)
    {
        flow_expression_bind(ctx, p_expression->right, p_param_list, p_argument_expression_list);
    }
}

struct type get_function_return_type(struct type * p_type);
unsigned char  type_is_scalar(struct type * p_type);
static void flow_visit_compound_statement(struct flow_visit_ctx * ctx, struct compound_statement * p_compound_statement);
signed long long object_to_signed_long_long(struct object * a);

static void flow_visit_expression(struct flow_visit_ctx * ctx, struct expression * p_expression, struct true_false_set * expr_true_false_set)
{
    unsigned char   nullable_enabled;

    true_false_set_clear(expr_true_false_set);
    nullable_enabled = ctx->ctx->options.null_checks_enabled;
    /*switch*/
    {
        register int _R0 = p_expression->expression_type;
        if (_R0 == 0) goto _CKL1; /*case 0*/
        if (_R0 == 4) goto _CKL2; /*case 4*/
        if (_R0 == 1) goto _CKL3; /*case 1*/
        if (_R0 == 2) goto _CKL4; /*case 2*/
        if (_R0 == 9) goto _CKL6; /*case 9*/
        if (_R0 == 3) goto _CKL7; /*case 3*/
        if (_R0 == 5) goto _CKL8; /*case 5*/
        if (_R0 == 8) goto _CKL9; /*case 8*/
        if (_R0 == 6) goto _CKL10; /*case 6*/
        if (_R0 == 7) goto _CKL11; /*case 7*/
        if (_R0 == 14) goto _CKL12; /*case 14*/
        if (_R0 == 15) goto _CKL13; /*case 15*/
        if (_R0 == 16) goto _CKL23; /*case 16*/
        if (_R0 == 17) goto _CKL24; /*case 17*/
        if (_R0 == 13) goto _CKL28; /*case 13*/
        if (_R0 == 12) goto _CKL29; /*case 12*/
        if (_R0 == 10) goto _CKL36; /*case 10*/
        if (_R0 == 11) goto _CKL37; /*case 11*/
        if (_R0 == 24) goto _CKL40; /*case 24*/
        if (_R0 == 25) goto _CKL42; /*case 25*/
        if (_R0 == 18) goto _CKL44; /*case 18*/
        if (_R0 == 30) goto _CKL46; /*case 30*/
        if (_R0 == 31) goto _CKL47; /*case 31*/
        if (_R0 == 28) goto _CKL48; /*case 28*/
        if (_R0 == 19) goto _CKL49; /*case 19*/
        if (_R0 == 20) goto _CKL50; /*case 20*/
        if (_R0 == 26) goto _CKL51; /*case 26*/
        if (_R0 == 27) goto _CKL52; /*case 27*/
        if (_R0 == 29) goto _CKL53; /*case 29*/
        if (_R0 == 33) goto _CKL54; /*case 33*/
        if (_R0 == 32) goto _CKL55; /*case 32*/
        if (_R0 == 53) goto _CKL60; /*case 53*/
        if (_R0 == 54) goto _CKL61; /*case 54*/
        if (_R0 == 55) goto _CKL62; /*case 55*/
        if (_R0 == 56) goto _CKL63; /*case 56*/
        if (_R0 == 57) goto _CKL64; /*case 57*/
        if (_R0 == 58) goto _CKL65; /*case 58*/
        if (_R0 == 59) goto _CKL66; /*case 59*/
        if (_R0 == 60) goto _CKL67; /*case 60*/
        if (_R0 == 61) goto _CKL68; /*case 61*/
        if (_R0 == 62) goto _CKL69; /*case 62*/
        if (_R0 == 63) goto _CKL70; /*case 63*/
        if (_R0 == 36) goto _CKL76; /*case 36*/
        if (_R0 == 34) goto _CKL79; /*case 34*/
        if (_R0 == 35) goto _CKL80; /*case 35*/
        if (_R0 == 37) goto _CKL81; /*case 37*/
        if (_R0 == 38) goto _CKL82; /*case 38*/
        if (_R0 == 39) goto _CKL83; /*case 39*/
        if (_R0 == 40) goto _CKL84; /*case 40*/
        if (_R0 == 41) goto _CKL85; /*case 41*/
        if (_R0 == 44) goto _CKL88; /*case 44*/
        if (_R0 == 45) goto _CKL89; /*case 45*/
        if (_R0 == 42) goto _CKL90; /*case 42*/
        if (_R0 == 43) goto _CKL91; /*case 43*/
        if (_R0 == 47) goto _CKL98; /*case 47*/
        if (_R0 == 46) goto _CKL99; /*case 46*/
        if (_R0 == 51) goto _CKL124; /*case 51*/
        if (_R0 == 52) goto _CKL127; /*case 52*/
        if (_R0 == 50) goto _CKL129; /*case 50*/
        if (_R0 == 48) goto _CKL130; /*case 48*/
        if (_R0 == 49) goto _CKL131; /*case 49*/
        if (_R0 == 21) goto _CKL132; /*case 21*/
        if (_R0 == 22) goto _CKL133; /*case 22*/
        if (_R0 == 23) goto _CKL134; /*case 23*/
        if (_R0 == 64) goto _CKL135; /*case 64*/
        if (_R0 == 65) goto _CKL136; /*case 65*/
        goto _CKL0;

        {
            struct flow_object * temp2;

            /*case 0*/ _CKL1:
            ;
            /*break*/ goto _CKL0;

            /*case 4*/ _CKL2:
            /*break*/ goto _CKL0;

            /*case 1*/ _CKL3:
            /*break*/ goto _CKL0;

            /*case 2*/ _CKL4:
            {
                ;
                if (p_expression->declarator->p_alias_of_expression)
                {
                    flow_visit_expression(ctx, p_expression->declarator->p_alias_of_expression, expr_true_false_set);
                }
                else
                {
                    struct true_false_set_item  item;

                    _cake_zmem(&item, 12);
                    item.p_expression = p_expression;
                    item.true_branch_state = 1;
                    item.false_branch_state = 2;
                    true_false_set_push_back(expr_true_false_set, &item);
                    check_uninitialized(ctx, p_expression);
                }
            }
            /*break*/ goto _CKL0;

            /*case 9*/ _CKL6:
            ;
            flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
            /*break*/ goto _CKL0;

            /*case 3*/ _CKL7:
            /*case 5*/ _CKL8:
            /*case 8*/ _CKL9:
            /*case 6*/ _CKL10:
            /*break*/ goto _CKL0;

            /*case 7*/ _CKL11:
            ;
            flow_visit_generic_selection(ctx, p_expression->generic_selection);
            /*break*/ goto _CKL0;

            /*case 14*/ _CKL12:
            {
                struct true_false_set  left_set;
                struct true_false_set_item  item;

                ;
                _cake_zmem(&left_set, 12);
                flow_visit_expression(ctx, p_expression->left, &left_set);
                true_false_set_destroy(&left_set);
                item.p_expression = p_expression;
                item.true_branch_state = 1;
                item.false_branch_state = 2;
                true_false_set_push_back(expr_true_false_set, &item);
            }
            /*break*/ goto _CKL0;

            /*case 15*/ _CKL13:
            {
                struct true_false_set  left_set;
                struct flow_object * p_object;
                struct true_false_set_item  item;

                ;
                _cake_zmem(&left_set, 12);
                flow_visit_expression(ctx, p_expression->left, &left_set);
                true_false_set_destroy(&left_set);
                p_object = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
                if (p_object != 0U)
                {
                    if (flow_object_can_be_null(p_object))
                    {
                        if (ctx->expression_is_not_evaluated)
                        {
                        }
                        else
                        {
                            struct marker  marker;

                            _cake_zmem(&marker, 28);
                            marker.p_token_begin = p_expression->left->first_token;
                            marker.p_token_end = p_expression->left->last_token;
                            compiler_diagnostic(32, ctx->ctx, 0U, &marker, "pointer may be null");
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
                                compiler_diagnostic(32, ctx->ctx, p_expression->left->first_token, 0U, "object is possibly uninitialized");
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
                                    compiler_diagnostic(30, ctx->ctx, p_expression->left->first_token, 0U, "object lifetime ended");
                                }
                            }
                        }
                    }
                }
                if (!ctx->expression_is_not_evaluated)
                {
                    struct flow_object * p_object2;

                    p_object2 = expression_get_flow_object(ctx, p_expression, nullable_enabled);
                    if (p_object2 && flow_object_can_have_its_lifetime_ended(p_object2))
                    {
                        struct marker  marker;

                        marker.file = 0;
                        marker.line = 0;
                        marker.start_col = 0;
                        marker.end_col = 0;
                        marker.p_token_caret = 0;
                        marker.p_token_begin = p_expression->first_token;
                        marker.p_token_end = p_expression->last_token;
                        compiler_diagnostic(30, ctx->ctx, 0U, &marker, "object lifetime ended");
                    }
                }
                item.p_expression = p_expression;
                item.true_branch_state = 1;
                item.false_branch_state = 2;
                true_false_set_push_back(expr_true_false_set, &item);
            }
            /*break*/ goto _CKL0;

            /*case 16*/ _CKL23:
            /*case 17*/ _CKL24:
            {
                struct flow_object * p_object;

                ;
                p_object = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
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
            /*break*/ goto _CKL0;

            /*case 13*/ _CKL28:
            {
                struct true_false_set_item  item;

                ;
                ;
                flow_visit_expression(ctx, p_expression->left, expr_true_false_set);
                flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
                true_false_set_clear(expr_true_false_set);
                item.p_expression = p_expression;
                item.true_branch_state = 1;
                item.false_branch_state = 2;
                true_false_set_push_back(expr_true_false_set, &item);
            }
            /*break*/ goto _CKL0;

            /*case 12*/ _CKL29:
            {
                if (!ctx->inside_contract)
                {
                    struct true_false_set  left_local;

                    ;
                    _cake_zmem(&left_local, 12);
                    flow_visit_expression(ctx, p_expression->left, &left_local);
                    flow_compare_function_arguments(ctx, &p_expression->left->type, &p_expression->argument_expression_list);
                    true_false_set_destroy(&left_local);
                }
                if (p_expression->left->declarator && type_is_function(&p_expression->left->declarator->type))
                {
                    struct type  return_type;

                    return_type = get_function_return_type(&p_expression->left->declarator->type);
                    if (p_expression->left->declarator->p_expression_true)
                    {
                        struct expression * p_expression_true;

                        p_expression_true = p_expression->left->declarator->p_expression_true;
                        flow_clear_alias(p_expression_true);
                        flow_expression_bind(ctx, p_expression_true, &p_expression->left->declarator->type.params, &p_expression->argument_expression_list);
                        if (type_is_scalar(&return_type))
                        {
                            struct true_false_set  local;
                            unsigned char   inside_contract;

                            _cake_zmem(&local, 12);
                            inside_contract = ctx->inside_contract;
                            ctx->inside_contract = 1;
                            flow_visit_expression(ctx, p_expression_true, &local);
                            ctx->inside_contract = inside_contract;
                            {
                                int i;
                                i = 0;
                                for (; i < local.size; i++)
                                {
                                    struct true_false_set_item  item5;

                                    item5.p_expression = local.data[i].p_expression;
                                    item5.true_branch_state = local.data[i].true_branch_state;
                                    item5.false_branch_state = 0;
                                    true_false_set_push_back(expr_true_false_set, &item5);
                                }
                            }
                            true_false_set_destroy(&local);
                        }
                        else
                        {
                            struct true_false_set  true_false_set4;
                            unsigned char   old;
                            unsigned char   inside_contract;

                            _cake_zmem(&true_false_set4, 12);
                            old = ctx->inside_assert;
                            ctx->inside_assert = 1;
                            inside_contract = ctx->inside_contract;
                            ctx->inside_contract = 1;
                            flow_visit_expression(ctx, p_expression->left->declarator->p_expression_true, &true_false_set4);
                            ctx->inside_contract = inside_contract;
                            ctx->inside_assert = old;
                            true_false_set_set_objects_to_true_branch(ctx, &true_false_set4, nullable_enabled);
                            true_false_set_destroy(&true_false_set4);
                        }
                    }
                    if (p_expression->left->declarator->p_expression_false)
                    {
                        struct expression * p_expression_false;
                        struct true_false_set  local;
                        unsigned char   inside_contract;

                        p_expression_false = p_expression->left->declarator->p_expression_false;
                        flow_clear_alias(p_expression_false);
                        flow_expression_bind(ctx, p_expression_false, &p_expression->left->declarator->type.params, &p_expression->argument_expression_list);
                        _cake_zmem(&local, 12);
                        inside_contract = ctx->inside_contract;
                        ctx->inside_contract = 1;
                        flow_visit_expression(ctx, p_expression_false, &local);
                        ctx->inside_contract = inside_contract;
                        {
                            int i;
                            i = 0;
                            for (; i < local.size; i++)
                            {
                                int index;

                                index = find_item_index_by_expression(expr_true_false_set, local.data[i].p_expression);
                                if (index == -1)
                                {
                                    struct true_false_set_item  item5;

                                    item5.p_expression = local.data[i].p_expression;
                                    item5.true_branch_state = 0;
                                    item5.false_branch_state = local.data[i].true_branch_state;
                                    true_false_set_push_back(expr_true_false_set, &item5);
                                }
                                else
                                {
                                    expr_true_false_set->data[index].false_branch_state |= local.data[i].false_branch_state;
                                }
                            }
                        }
                        true_false_set_destroy(&local);
                    }
                    type_destroy(&return_type);
                }
            }
            /*break*/ goto _CKL0;

            /*case 10*/ _CKL36:
            ;
            flow_visit_compound_statement(ctx, p_expression->compound_statement);
            /*break*/ goto _CKL0;

            /*case 11*/ _CKL37:
            ;
            ;
            ;
            ;
            flow_visit_bracket_initializer_list(ctx, p_expression->braced_initializer);
            temp2 = make_flow_object(ctx, &p_expression->type, 0U, p_expression);
            if (temp2 == 0U)
            {
                return;
            }
            if (p_expression->type_name->abstract_declarator->p_flow_object)
            {
                flow_object_swap(temp2, p_expression->type_name->abstract_declarator->p_flow_object);
                flow_object_set_zero(&p_expression->type, p_expression->type_name->abstract_declarator->p_flow_object);
            }
            /*break*/ goto _CKL0;

            /*case 24*/ _CKL40:
            if (p_expression->right)
            {
                flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
            }
            /*break*/ goto _CKL0;

            /*case 25*/ _CKL42:
            if (p_expression->right)
            {
                struct true_false_set  true_false_set4;
                unsigned char   old;

                _cake_zmem(&true_false_set4, 12);
                old = ctx->inside_assert;
                ctx->inside_assert = 1;
                flow_visit_expression(ctx, p_expression->right, &true_false_set4);
                ctx->inside_assert = old;
                true_false_set_set_objects_to_true_branch(ctx, &true_false_set4, nullable_enabled);
                true_false_set_destroy(&true_false_set4);
            }
            /*break*/ goto _CKL0;

            /*case 18*/ _CKL44:
            if (p_expression->right)
            {
                unsigned char   t2;

                t2 = ctx->expression_is_not_evaluated;
                ctx->expression_is_not_evaluated = 1;
                flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
                ctx->expression_is_not_evaluated = t2;
            }
            /*break*/ goto _CKL0;

            /*case 30*/ _CKL46:
            /*case 31*/ _CKL47:
            ;
            flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
            /*break*/ goto _CKL0;

            /*case 28*/ _CKL48:
            ;
            flow_check_pointer_used_as_bool(ctx, p_expression->right);
            flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
            true_false_set_invert(expr_true_false_set);
            /*break*/ goto _CKL0;

            /*case 19*/ _CKL49:
            /*case 20*/ _CKL50:
            /*case 26*/ _CKL51:
            /*case 27*/ _CKL52:
            /*case 29*/ _CKL53:
            /*case 33*/ _CKL54:
            /*break*/ goto _CKL0;

            /*case 32*/ _CKL55:
            {
                struct flow_object * p_object0;
                struct true_false_set  local_true_false;

                ;
                p_object0 = expression_get_flow_object(ctx, p_expression->right, nullable_enabled);
                if (p_object0 && p_object0->current.state == 1)
                {
                    if (!ctx->expression_is_not_evaluated)
                    {
                        struct marker  marker;

                        _cake_zmem(&marker, 28);
                        marker.p_token_begin = p_expression->right->first_token;
                        marker.p_token_end = p_expression->right->last_token;
                        compiler_diagnostic(29, ctx->ctx, 0U, &marker, "using a uninitialized object");
                    }
                }
                else
                {
                    if (p_object0 && flow_object_can_be_null(p_object0))
                    {
                        if (!ctx->expression_is_not_evaluated)
                        {
                            compiler_diagnostic(32, ctx->ctx, p_expression->right->first_token, 0U, "dereference a NULL object");
                        }
                    }
                }
                _cake_zmem(&local_true_false, 12);
                flow_visit_expression(ctx, p_expression->right, &local_true_false);
                true_false_set_destroy(&local_true_false);
            }
            /*break*/ goto _CKL0;

            /*case 53*/ _CKL60:
            /*case 54*/ _CKL61:
            /*case 55*/ _CKL62:
            /*case 56*/ _CKL63:
            /*case 57*/ _CKL64:
            /*case 58*/ _CKL65:
            /*case 59*/ _CKL66:
            /*case 60*/ _CKL67:
            /*case 61*/ _CKL68:
            /*case 62*/ _CKL69:
            /*case 63*/ _CKL70:
            {
                struct true_false_set  left_set;
                struct true_false_set  right_set;
                struct flow_object * p_right_object;
                struct flow_object * p_dest_object;
                struct marker  a_marker;
                struct marker  b_marker;

                ;
                ;
                _cake_zmem(&left_set, 12);
                flow_visit_expression(ctx, p_expression->left, &left_set);
                true_false_set_swap(expr_true_false_set, &left_set);
                true_false_set_destroy(&left_set);
                _cake_zmem(&right_set, 12);
                flow_visit_expression(ctx, p_expression->right, &right_set);
                true_false_set_destroy(&right_set);
                p_right_object = expression_get_flow_object(ctx, p_expression->right, nullable_enabled);
                p_dest_object = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
                if (p_dest_object == 0U || p_right_object == 0U)
                {
                    return;
                }
                a_marker.file = 0;
                a_marker.line = 0;
                a_marker.start_col = 0;
                a_marker.end_col = 0;
                a_marker.p_token_caret = 0;
                a_marker.p_token_begin = p_expression->left->first_token;
                a_marker.p_token_end = p_expression->left->last_token;
                b_marker.file = 0;
                b_marker.line = 0;
                b_marker.start_col = 0;
                b_marker.end_col = 0;
                b_marker.p_token_caret = 0;
                b_marker.p_token_begin = p_expression->right->first_token;
                b_marker.p_token_end = p_expression->right->last_token;
                flow_check_assignment(ctx, p_expression->left->first_token, &a_marker, &b_marker, 2, 1, type_is_view(&p_expression->left->type), type_is_opt(&p_expression->left->type, ctx->ctx->options.null_checks_enabled), &p_expression->left->type, p_dest_object, &p_expression->right->type, p_right_object, 0U);
                arena_broadcast_change(ctx, p_dest_object);
                if (expression_is_malloc(p_expression->right))
                {
                    struct type  t;
                    struct flow_object * po;

                    t = type_remove_pointer(&p_expression->left->type);
                    po = make_flow_object(ctx, &t, 0U, p_expression->left);
                    if (po == 0U)
                    {
                        type_destroy(&t);
                        return;
                    }
                    object_set_pointer(p_dest_object, po);
                    type_destroy(&t);
                    p_dest_object->current.state = 6;
                }
                else
                {
                    if (expression_is_calloc(p_expression->right))
                    {
                        struct type  t;
                        struct flow_object * po;

                        t = type_remove_pointer(&p_expression->left->type);
                        po = make_flow_object(ctx, &t, 0U, p_expression->left);
                        if (po == 0U)
                        {
                            type_destroy(&t);
                            return;
                        }
                        flow_object_set_zero(&t, po);
                        object_set_pointer(p_dest_object, po);
                        type_destroy(&t);
                        p_dest_object->current.state = 6;
                    }
                }
            }
            /*break*/ goto _CKL0;

            /*case 36*/ _CKL76:
            {
                struct true_false_set  left_set;
                struct true_false_set  right_set;
                struct flow_object * p_object;

                ;
                ;
                _cake_zmem(&left_set, 12);
                _cake_zmem(&right_set, 12);
                flow_visit_expression(ctx, p_expression->left, &left_set);
                p_object = expression_get_flow_object(ctx, p_expression->right, ctx->ctx->options.null_checks_enabled);
                if (p_object)
                {
                    if (flow_object_can_be_zero(p_object))
                    {
                        compiler_diagnostic(35, ctx->ctx, p_expression->right->first_token, 0U, "possible division by zero");
                    }
                }
                flow_visit_expression(ctx, p_expression->right, &right_set);
                true_false_set_merge(expr_true_false_set, &left_set, &right_set, 5, 5);
                true_false_set_destroy(&left_set);
                true_false_set_destroy(&right_set);
            }
            /*break*/ goto _CKL0;

            /*case 34*/ _CKL79:
            /*case 35*/ _CKL80:
            /*case 37*/ _CKL81:
            /*case 38*/ _CKL82:
            /*case 39*/ _CKL83:
            /*case 40*/ _CKL84:
            /*case 41*/ _CKL85:
            {
                if (p_expression->left)
                {
                    struct true_false_set  left_set;

                    _cake_zmem(&left_set, 12);
                    flow_visit_expression(ctx, p_expression->left, &left_set);
                    true_false_set_destroy(&left_set);
                }
                if (p_expression->right)
                {
                    struct true_false_set  right_set;

                    _cake_zmem(&right_set, 12);
                    flow_visit_expression(ctx, p_expression->right, &right_set);
                    true_false_set_destroy(&right_set);
                }
            }
            /*break*/ goto _CKL0;

            /*case 44*/ _CKL88:
            /*case 45*/ _CKL89:
            /*case 42*/ _CKL90:
            /*case 43*/ _CKL91:
            {
                unsigned char   left_is_constant;
                unsigned char   right_is_constant;

                ;
                ;
                left_is_constant = object_has_constant_value(&p_expression->left->object);
                right_is_constant = object_has_constant_value(&p_expression->right->object);
                if (left_is_constant)
                {
                    long long left_value;
                    struct true_false_set  true_false_set_right;

                    left_value = object_to_signed_long_long(&p_expression->left->object);
                    _cake_zmem(&true_false_set_right, 12);
                    flow_visit_expression(ctx, p_expression->right, &true_false_set_right);
                    if (left_value == 0)
                    {
                        true_false_set_swap(expr_true_false_set, &true_false_set_right);
                        {
                            int i;
                            i = 0;
                            for (; i < expr_true_false_set->size; i++)
                            {
                                struct true_false_set_item * item;

                                item = &expr_true_false_set->data[i];
                                item->false_branch_state |= item->true_branch_state;
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
                        long long right_value;
                        struct true_false_set  true_false_set_left3;

                        right_value = object_to_signed_long_long(&p_expression->right->object);
                        _cake_zmem(&true_false_set_left3, 12);
                        flow_visit_expression(ctx, p_expression->left, &true_false_set_left3);
                        if (right_value == 0)
                        {
                            true_false_set_swap(expr_true_false_set, &true_false_set_left3);
                            {
                                int i;
                                i = 0;
                                for (; i < expr_true_false_set->size; i++)
                                {
                                    struct true_false_set_item * item;

                                    item = &expr_true_false_set->data[i];
                                    item->false_branch_state |= item->true_branch_state;
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
                        struct true_false_set  true_false_set;

                        _cake_zmem(&true_false_set, 12);
                        flow_visit_expression(ctx, p_expression->left, &true_false_set);
                        flow_visit_expression(ctx, p_expression->right, &true_false_set);
                        true_false_set_destroy(&true_false_set);
                    }
                }
            }
            /*break*/ goto _CKL0;

            /*case 47*/ _CKL98:
            /*case 46*/ _CKL99:
            {
                long long value;
                struct expression * p_comp_expression;

                ;
                ;
                value = 0;
                p_comp_expression = 0U;
                if (object_has_constant_value(&p_expression->left->object) && !object_has_constant_value(&p_expression->right->object))
                {
                    value = object_to_signed_long_long(&p_expression->left->object);
                    p_comp_expression = p_expression->right;
                }
                else
                {
                    if (object_has_constant_value(&p_expression->right->object) && !object_has_constant_value(&p_expression->left->object))
                    {
                        value = object_to_signed_long_long(&p_expression->right->object);
                        p_comp_expression = p_expression->left;
                    }
                }
                if (p_comp_expression)
                {
                    struct true_false_set  true_false_set;
                    struct flow_object * p_object;

                    _cake_zmem(&true_false_set, 12);
                    flow_visit_expression(ctx, p_comp_expression, &true_false_set);
                    p_object = expression_get_flow_object(ctx, p_comp_expression, nullable_enabled);
                    if (p_object)
                    {
                        struct marker  marker;

                        marker.file = 0;
                        marker.line = 0;
                        marker.start_col = 0;
                        marker.end_col = 0;
                        marker.p_token_caret = 0;
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
                                        if (!ctx->inside_loop)
                                        {
                                            compiler_diagnostic(27, ctx->ctx, 0U, &marker, "pointer is always null");
                                        }
                                    }
                                    else
                                    {
                                        if (!ctx->inside_loop)
                                        {
                                            compiler_diagnostic(27, ctx->ctx, 0U, &marker, "value is always zero");
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if (p_expression->expression_type == 47)
                                {
                                    if (type_is_pointer(&p_comp_expression->type))
                                    {
                                        compiler_diagnostic(27, ctx->ctx, 0U, &marker, "pointer is always null");
                                    }
                                    else
                                    {
                                        compiler_diagnostic(27, ctx->ctx, 0U, &marker, "value is always zero");
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
                                        if (!ctx->inside_loop)
                                        {
                                            compiler_diagnostic(27, ctx->ctx, 0U, &marker, "pointer is always non-null");
                                        }
                                    }
                                    else
                                    {
                                        if (!ctx->inside_loop)
                                        {
                                            compiler_diagnostic(27, ctx->ctx, 0U, &marker, "value is always non-zero");
                                        }
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
                                                if (!ctx->inside_loop)
                                                {
                                                    compiler_diagnostic(27, ctx->ctx, 0U, &marker, "pointer is always non-null");
                                                }
                                            }
                                            else
                                            {
                                                if (!ctx->inside_loop)
                                                {
                                                    compiler_diagnostic(27, ctx->ctx, 0U, &marker, "value is always non-zero");
                                                }
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
                    struct true_false_set  true_false_set;

                    _cake_zmem(&true_false_set, 12);
                    flow_visit_expression(ctx, p_expression->left, &true_false_set);
                    flow_visit_expression(ctx, p_expression->right, &true_false_set);
                    true_false_set_destroy(&true_false_set);
                }
            }
            /*break*/ goto _CKL0;

            /*case 51*/ _CKL124:
            {
                struct true_false_set  left_set;

                ;
                ;
                flow_check_pointer_used_as_bool(ctx, p_expression->left);
                flow_check_pointer_used_as_bool(ctx, p_expression->right);
                _cake_zmem(&left_set, 12);
                flow_visit_expression(ctx, p_expression->left, &left_set);
                if (object_has_constant_value(&p_expression->left->object) && object_to_bool(&p_expression->left->object) == 1)
                {
                }
                else
                {
                    int original_state_number;
                    struct true_false_set  right_set;

                    original_state_number = arena_add_copy_of_current_state(ctx, "original");
                    true_false_set_set_objects_to_false_branch(ctx, &left_set, nullable_enabled);
                    _cake_zmem(&right_set, 12);
                    flow_visit_expression(ctx, p_expression->right, &right_set);
                    {
                        int i;
                        i = 0;
                        for (; i < left_set.size; i++)
                        {
                            struct true_false_set_item  item5;

                            item5.p_expression = left_set.data[i].p_expression;
                            item5.true_branch_state |= (left_set.data[i].true_branch_state | left_set.data[i].false_branch_state);
                            item5.false_branch_state |= left_set.data[i].false_branch_state;
                            true_false_set_push_back(expr_true_false_set, &item5);
                        }
                    }
                    {
                        int k;
                        k = 0;
                        for (; k < right_set.size; k++)
                        {
                            int index;

                            index = find_item_index_by_expression(expr_true_false_set, right_set.data[k].p_expression);
                            if (index == -1)
                            {
                                struct true_false_set_item  item4;

                                index = expr_true_false_set->size;
                                item4.p_expression = right_set.data[k].p_expression;
                                item4.true_branch_state = 0;
                                item4.false_branch_state = 0;
                                true_false_set_push_back(expr_true_false_set, &item4);
                            }
                            expr_true_false_set->data[index].p_expression = right_set.data[k].p_expression;
                            expr_true_false_set->data[index].false_branch_state |= right_set.data[k].false_branch_state;
                            expr_true_false_set->data[index].true_branch_state |= 3;
                        }
                    }
                    arena_restore_current_state_from(ctx, original_state_number);
                    arena_remove_state(ctx, original_state_number);
                    true_false_set_destroy(&right_set);
                }
                true_false_set_destroy(&left_set);
            }
            /*break*/ goto _CKL0;

            /*case 52*/ _CKL127:
            {
                struct true_false_set  left_set;
                int original_state_number;
                struct true_false_set  right_set;

                ;
                ;
                flow_check_pointer_used_as_bool(ctx, p_expression->left);
                flow_check_pointer_used_as_bool(ctx, p_expression->right);
                _cake_zmem(&left_set, 12);
                flow_visit_expression(ctx, p_expression->left, &left_set);
                original_state_number = arena_add_copy_of_current_state(ctx, "original");
                true_false_set_set_objects_to_true_branch(ctx, &left_set, nullable_enabled);
                _cake_zmem(&right_set, 12);
                flow_visit_expression(ctx, p_expression->right, &right_set);
                {
                    int i;
                    i = 0;
                    for (; i < left_set.size; i++)
                    {
                        struct true_false_set_item * p_item_left;
                        struct true_false_set_item  left_and_right;

                        p_item_left = &left_set.data[i];
                        _cake_zmem(&left_and_right, 12);
                        left_and_right.p_expression = p_item_left->p_expression;
                        left_and_right.true_branch_state |= p_item_left->true_branch_state;
                        left_and_right.false_branch_state |= p_item_left->true_branch_state | p_item_left->false_branch_state;
                        true_false_set_push_back(expr_true_false_set, &left_and_right);
                    }
                }
                {
                    int k;
                    k = 0;
                    for (; k < right_set.size; k++)
                    {
                        struct true_false_set_item * p_item_right;
                        int index;

                        p_item_right = &right_set.data[k];
                        index = find_item_index_by_expression(expr_true_false_set, p_item_right->p_expression);
                        if (index == -1)
                        {
                            struct true_false_set_item  item2;

                            index = expr_true_false_set->size;
                            item2.p_expression = p_item_right->p_expression;
                            item2.true_branch_state = 0;
                            item2.false_branch_state = 0;
                            true_false_set_push_back(expr_true_false_set, &item2);
                        }
                        expr_true_false_set->data[index].p_expression = p_item_right->p_expression;
                        expr_true_false_set->data[index].true_branch_state |= p_item_right->true_branch_state;
                        expr_true_false_set->data[index].false_branch_state |= p_item_right->false_branch_state;
                        expr_true_false_set->data[index].false_branch_state |= 3;
                    }
                }
                arena_restore_current_state_from(ctx, original_state_number);
                arena_remove_state(ctx, original_state_number);
                true_false_set_destroy(&left_set);
                true_false_set_destroy(&right_set);
            }
            /*break*/ goto _CKL0;

            /*case 50*/ _CKL129:
            {
                struct true_false_set  true_false_set;

                ;
                ;
                _cake_zmem(&true_false_set, 12);
                flow_visit_expression(ctx, p_expression->left, &true_false_set);
                flow_visit_expression(ctx, p_expression->right, &true_false_set);
                true_false_set_destroy(&true_false_set);
            }
            /*break*/ goto _CKL0;

            /*case 48*/ _CKL130:
            /*case 49*/ _CKL131:
            ;
            ;
            flow_visit_expression(ctx, p_expression->left, expr_true_false_set);
            flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
            /*break*/ goto _CKL0;

            /*case 21*/ _CKL132:
            /*break*/ goto _CKL0;

            /*case 22*/ _CKL133:
            /*break*/ goto _CKL0;

            /*case 23*/ _CKL134:
            /*break*/ goto _CKL0;

            /*case 64*/ _CKL135:
            /*break*/ goto _CKL0;

            /*case 65*/ _CKL136:
            {
                struct true_false_set  true_false_set;
                int before_if_state_number;
                struct true_false_set  set;
                int left_true_branch_state_number;
                struct true_false_set  set2;

                ;
                ;
                ;
                _cake_zmem(&true_false_set, 12);
                flow_check_pointer_used_as_bool(ctx, p_expression->condition_expr);
                flow_visit_expression(ctx, p_expression->condition_expr, &true_false_set);
                before_if_state_number = arena_add_copy_of_current_state(ctx, "before-if");
                true_false_set_set_objects_to_true_branch(ctx, &true_false_set, nullable_enabled);
                _cake_zmem(&set, 12);
                flow_visit_expression(ctx, p_expression->left, &set);
                true_false_set_destroy(&set);
                left_true_branch_state_number = arena_add_copy_of_current_state(ctx, "left-true-branch");
                arena_restore_current_state_from(ctx, before_if_state_number);
                true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);
                _cake_zmem(&set2, 12);
                flow_visit_expression(ctx, p_expression->right, &set2);
                true_false_set_destroy(&set2);
                arena_merge_current_state_with_state_number(ctx, left_true_branch_state_number);
                arena_restore_current_state_from(ctx, left_true_branch_state_number);
                arena_remove_state(ctx, before_if_state_number);
                arena_remove_state(ctx, left_true_branch_state_number);
                true_false_set_destroy(&true_false_set);
            }
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
}

static void flow_visit_expression_statement(struct flow_visit_ctx * ctx, struct expression_statement * p_expression_statement)
{
    struct true_false_set  d;

    _cake_zmem(&d, 12);
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
    unsigned char   nullable_enabled;
    struct true_false_set  true_false_set;

    ;
    nullable_enabled = ctx->ctx->options.null_checks_enabled;
    _cake_zmem(&true_false_set, 12);
    /*try*/ if (1)
    {
        unsigned char   was_last_statement_inside_true_branch_return;

        if (p_iteration_statement->expression1)
        {
            flow_visit_expression(ctx, p_iteration_statement->expression1, &true_false_set);
        }
        flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
        flow_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);
        flow_defer_list_set_end_of_lifetime(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);
        was_last_statement_inside_true_branch_return = secondary_block_ends_with_jump(p_iteration_statement->secondary_block);
        if (was_last_statement_inside_true_branch_return)
        {
        }
        else
        {
            true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);
        }
    }
    /*catch*/ else _CKL0:
    {
    }
    true_false_set_destroy(&true_false_set);
}

int diagnostic_stack_push_empty(struct diagnostic_stack * diagnostic_stack);
void diagnostic_stack_pop(struct diagnostic_stack * diagnostic_stack);

static void flow_visit_while_statement(struct flow_visit_ctx * ctx, struct iteration_statement * p_iteration_statement)
{
    unsigned char   nullable_enabled;
    int old_initial_state;
    int old_break_join_state;
    struct true_false_set  true_false_set;
    struct true_false_set  true_false_set2;
    unsigned char   was_last_statement_inside_true_branch_return;

    ;
    if (p_iteration_statement->expression1 == 0U)
    {
        return;
    }
    nullable_enabled = ctx->ctx->options.null_checks_enabled;
    old_initial_state = ctx->initial_state;
    old_break_join_state = ctx->break_join_state;
    ctx->initial_state = arena_add_copy_of_current_state(ctx, "original");
    ctx->break_join_state = arena_add_empty_state(ctx, "break join");
    _cake_zmem(&true_false_set, 12);
    diagnostic_stack_push_empty(&ctx->ctx->options.diagnostic_stack);
    flow_visit_expression(ctx, p_iteration_statement->expression1, &true_false_set);
    true_false_set_set_objects_to_true_branch(ctx, &true_false_set, nullable_enabled);
    flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
    diagnostic_stack_pop(&ctx->ctx->options.diagnostic_stack);
    _cake_zmem(&true_false_set2, 12);
    flow_visit_expression(ctx, p_iteration_statement->expression1, &true_false_set2);
    true_false_set_destroy(&true_false_set2);
    true_false_set_set_objects_to_true_branch(ctx, &true_false_set, nullable_enabled);
    flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
    flow_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);
    was_last_statement_inside_true_branch_return = secondary_block_ends_with_jump(p_iteration_statement->secondary_block);
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
    struct true_false_set  d;

    ;
    _cake_zmem(&d, 12);
    /*try*/ if (1)
    {
        unsigned char   b_secondary_block_ends_with_jump;

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
        b_secondary_block_ends_with_jump = secondary_block_ends_with_jump(p_iteration_statement->secondary_block);
        if (!b_secondary_block_ends_with_jump)
        {
            flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
            flow_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);
            flow_defer_list_set_end_of_lifetime(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);
        }
    }
    /*catch*/ else _CKL0:
    {
    }
    true_false_set_destroy(&d);
}

static void flow_visit_iteration_statement(struct flow_visit_ctx * ctx, struct iteration_statement * p_iteration_statement)
{
    unsigned char   inside_loop;

    inside_loop = ctx->inside_loop;
    ctx->inside_loop = 1;
    /*switch*/
    {
        register int _R1 = p_iteration_statement->first_token->type;
        if (_R1 == 9042) goto _CKL1; /*case 9042*/
        if (_R1 == 9008) goto _CKL2; /*case 9008*/
        if (_R1 == 9015) goto _CKL3; /*case 9015*/
        goto /*default*/ _CKL4;

        {
            /*case 9042*/ _CKL1:
            flow_visit_while_statement(ctx, p_iteration_statement);
            /*break*/ goto _CKL0;

            /*case 9008*/ _CKL2:
            flow_visit_do_while_statement(ctx, p_iteration_statement);
            /*break*/ goto _CKL0;

            /*case 9015*/ _CKL3:
            flow_visit_for_statement(ctx, p_iteration_statement);
            /*break*/ goto _CKL0;

            /*default*/ _CKL4:
            ;
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    ctx->inside_loop = inside_loop;
}

static void flow_visit_jump_statement(struct flow_visit_ctx * ctx, struct jump_statement * p_jump_statement)
{
    unsigned char   nullable_enabled;

    nullable_enabled = ctx->ctx->options.null_checks_enabled;
    /*try*/ if (1)
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
                if (ctx->p_return_type == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
                if (p_jump_statement->expression_opt)
                {
                    struct true_false_set  d;

                    _cake_zmem(&d, 12);
                    flow_visit_expression(ctx, p_jump_statement->expression_opt, &d);
                    true_false_set_destroy(&d);
                }
                if (p_jump_statement->expression_opt)
                {
                    struct flow_object * p_object;

                    p_object = expression_get_flow_object(ctx, p_jump_statement->expression_opt, nullable_enabled);
                    if (p_object)
                    {
                        struct flow_object * p_dest_object;
                        struct marker  a_marker;
                        struct marker  b_marker;
                        int state_before_return;

                        ;
                        p_dest_object = make_flow_object(ctx, ctx->p_return_type, 0U, p_jump_statement->expression_opt);
                        if (p_dest_object == 0U)
                        {
                            /*throw*/ goto _CKL0;
                        }
                        ;
                        flow_object_set_zero(ctx->p_return_type, p_dest_object);
                        a_marker.file = 0;
                        a_marker.line = 0;
                        a_marker.start_col = 0;
                        a_marker.end_col = 0;
                        a_marker.p_token_caret = 0;
                        a_marker.p_token_begin = p_jump_statement->expression_opt->first_token;
                        a_marker.p_token_end = p_jump_statement->expression_opt->last_token;
                        b_marker.file = 0;
                        b_marker.line = 0;
                        b_marker.start_col = 0;
                        b_marker.end_col = 0;
                        b_marker.p_token_caret = 0;
                        b_marker.p_token_begin = p_jump_statement->expression_opt->first_token;
                        b_marker.p_token_end = p_jump_statement->expression_opt->last_token;
                        ;
                        flow_check_assignment(ctx, p_jump_statement->expression_opt->first_token, &a_marker, &b_marker, 0, 1, type_is_view(ctx->p_return_type), type_is_opt(ctx->p_return_type, ctx->ctx->options.null_checks_enabled), ctx->p_return_type, p_dest_object, &p_jump_statement->expression_opt->type, p_object, 0U);
                        state_before_return = arena_add_copy_of_current_state(ctx, "before-return");
                        flow_defer_list_set_end_of_lifetime(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
                        if (ctx->p_return_type == 0U)
                        {
                            /*throw*/ goto _CKL0;
                        }
                        checked_read_object(ctx, ctx->p_return_type, type_is_opt(ctx->p_return_type, ctx->ctx->options.null_checks_enabled), p_dest_object, 0U, &a_marker, 1);
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
                            int label_state_number;

                            ;
                            label_state_number = -1;
                            {
                                int i;
                                i = 0;
                                for (; i < ctx->labels_size; i++)
                                {
                                    if (ctx->labels[i].state_number == label_state_number)
                                    {
                                        break;
                                    }
                                }
                            }
                            if (label_state_number == -1)
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
    /*catch*/ else _CKL0:
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

static void flow_visit_label(struct flow_visit_ctx * ctx, struct label * p_label)
{
    if (p_label->p_identifier_opt)
    {
        {
            int i;
            i = 0;
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
    struct block_item * p_block_item;

    p_block_item = p_block_item_list->head;
    while (p_block_item)
    {
        flow_visit_block_item(ctx, p_block_item);
        p_block_item = p_block_item->next;
    }
}

int __cdecl isalpha(int _C);
int __cdecl strncmp(char * _Str1, char * _Str2, unsigned int _MaxCount);

static int parse_string_state(char * s, unsigned char  * invalid)
{
    int e;
    char * p;

    *invalid = 0;
    e = 0;
    p = s;
    while (*p)
    {
        if (isalpha(*p))
        {
            int sz;
            char * start;

            sz = 0;
            start = p;
            while (isalpha(*p) || *p == 45)
            {
                sz++;
                p++;
            }
            if (strncmp(start, "moved", sz) == 0)
            {
                e |= 8;
            }
            else
            {
                if (strncmp(start, "null", sz) == 0)
                {
                    e |= 2;
                }
                else
                {
                    if (strncmp(start, "not-null", sz) == 0)
                    {
                        e |= 4;
                    }
                    else
                    {
                        if (strncmp(start, "maybe-null", sz) == 0)
                        {
                            e |= 6;
                        }
                        else
                        {
                            if (strncmp(start, "uninitialized", sz) == 0)
                            {
                                e |= 1;
                            }
                            else
                            {
                                if (strncmp(start, "zero", sz) == 0)
                                {
                                    e |= 32;
                                }
                                else
                                {
                                    if (strncmp(start, "not-zero", sz) == 0)
                                    {
                                        e |= 64;
                                    }
                                    else
                                    {
                                        if (strncmp(start, "any", sz) == 0)
                                        {
                                            e |= 96;
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

static void flow_visit_static_assert_declaration(struct flow_visit_ctx * ctx, struct static_assert_declaration * p_static_assert_declaration)
{
    unsigned char   t2;
    unsigned char   nullable_enabled;
    struct true_false_set  a;

    t2 = ctx->expression_is_not_evaluated;
    ctx->expression_is_not_evaluated = 1;
    nullable_enabled = ctx->ctx->options.null_checks_enabled;
    _cake_zmem(&a, 12);
    flow_visit_expression(ctx, p_static_assert_declaration->constant_expression, &a);
    ctx->expression_is_not_evaluated = t2;
    if (p_static_assert_declaration->first_token->type == 9072 || p_static_assert_declaration->first_token->type == 9073)
    {
        unsigned char   ex;
        struct flow_object * p_obj;

        ex = !!(p_static_assert_declaration->first_token->type == 9073);
        compiler_diagnostic(62, ctx->ctx, p_static_assert_declaration->first_token, 0U, "static_debug");
        p_obj = expression_get_flow_object(ctx, p_static_assert_declaration->constant_expression, nullable_enabled);
        if (p_obj)
        {
            print_flow_object(&p_static_assert_declaration->constant_expression->type, p_obj, !ex);
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
        if (p_static_assert_declaration->first_token->type == 9074)
        {
            unsigned char   is_invalid;
            int e;

            is_invalid = 0;
            e = 0;
            if (p_static_assert_declaration->string_literal_opt)
            {
                e = parse_string_state(p_static_assert_declaration->string_literal_opt->lexeme, &is_invalid);
            }
            if (is_invalid)
            {
                compiler_diagnostic(1080, ctx->ctx, p_static_assert_declaration->first_token, 0U, "invalid parameter %s", p_static_assert_declaration->string_literal_opt->lexeme);
            }
            else
            {
                struct flow_object * p_obj;

                p_obj = expression_get_flow_object(ctx, p_static_assert_declaration->constant_expression, nullable_enabled);
                if (p_obj)
                {
                    if (e != p_obj->current.state)
                    {
                        compiler_diagnostic(1080, ctx->ctx, p_static_assert_declaration->first_token, 0U, "static_state failed");
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
                        compiler_diagnostic(1080, ctx->ctx, p_static_assert_declaration->first_token, 0U, "static_state failed");
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
            if (p_static_assert_declaration->first_token->type == 9075)
            {
                struct flow_object * p_obj;

                p_obj = expression_get_flow_object(ctx, p_static_assert_declaration->constant_expression, nullable_enabled);
                if (p_obj)
                {
                    if (p_static_assert_declaration->string_literal_opt)
                    {
                        char * lexeme;

                        lexeme = p_static_assert_declaration->string_literal_opt->lexeme;
                        if (strcmp(lexeme, "\"zero\"") == 0)
                        {
                            flow_object_set_zero(&p_static_assert_declaration->constant_expression->type, p_obj);
                        }
                        else
                        {
                            unsigned char   is_invalid;
                            int e;

                            is_invalid = 0;
                            e = parse_string_state(p_static_assert_declaration->string_literal_opt->lexeme, &is_invalid);
                            if (!is_invalid)
                            {
                                if (p_obj->members.size > 0)
                                {
                                    compiler_diagnostic(1070, ctx->ctx, p_static_assert_declaration->first_token, 0U, "use only for non agregates");
                                }
                                p_obj->current.state = e;
                            }
                            else
                            {
                                compiler_diagnostic(1070, ctx->ctx, p_static_assert_declaration->first_token, 0U, "invalid parameter %s", p_static_assert_declaration->string_literal_opt->lexeme);
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

static void flow_visit_direct_declarator(struct flow_visit_ctx * ctx, struct direct_declarator * p_direct_declarator)
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
                struct true_false_set  a;

                _cake_zmem(&a, 12);
                flow_visit_expression(ctx, p_direct_declarator->array_declarator->assignment_expression, &a);
                true_false_set_destroy(&a);
            }
        }
    }
}

unsigned char  type_is_pointer_to_out(struct type * p_type);

static void flow_visit_declarator(struct flow_visit_ctx * ctx, struct declarator * p_declarator)
{
    unsigned char   nullable_enabled;

    nullable_enabled = ctx->ctx->options.null_checks_enabled;
    /*try*/ if (1)
    {
        if (p_declarator->type.category != 1)
        {
            if (ctx->parameter_list > 1)
            {
                return;
            }
            p_declarator->p_flow_object = make_flow_object(ctx, &p_declarator->type, p_declarator, 0U);
            if (p_declarator->p_flow_object == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            flow_object_set_uninitialized(&p_declarator->type, p_declarator->p_flow_object);
            if (p_declarator->declaration_specifiers && p_declarator->declaration_specifiers->storage_class_specifier_flags & 2048)
            {
                if (type_is_pointer(&p_declarator->type))
                {
                    if (type_is_opt(&p_declarator->type, ctx->ctx->options.null_checks_enabled))
                    {
                        p_declarator->p_flow_object->current.state = 6;
                    }
                    else
                    {
                        p_declarator->p_flow_object->current.state = 4;
                    }
                    if (type_is_pointer_to_out(&p_declarator->type))
                    {
                        struct type  t;
                        struct flow_object * po;

                        t = type_remove_pointer(&p_declarator->type);
                        po = make_flow_object(ctx, &t, p_declarator, 0U);
                        if (po == 0U)
                        {
                            type_destroy(&t);
                            /*throw*/ goto _CKL0;
                        }
                        flow_object_set_uninitialized(&t, po);
                        object_set_pointer(p_declarator->p_flow_object, po);
                        type_destroy(&t);
                    }
                    else
                    {
                        if (type_is_owner_or_pointer_to_dtor(&p_declarator->type))
                        {
                            struct type  t;
                            struct flow_object * po;
                            unsigned char   t_is_nullable;

                            t = type_remove_pointer(&p_declarator->type);
                            po = make_flow_object(ctx, &t, p_declarator, 0U);
                            if (po == 0U)
                            {
                                type_destroy(&t);
                                /*throw*/ goto _CKL0;
                            }
                            t_is_nullable = type_is_opt(&t, ctx->ctx->options.null_checks_enabled);
                            flow_object_set_unknown(&t, t_is_nullable, po, nullable_enabled);
                            object_set_pointer(p_declarator->p_flow_object, po);
                            type_destroy(&t);
                        }
                    }
                }
                else
                {
                    if (type_is_struct_or_union(&p_declarator->type))
                    {
                        unsigned char   is_nullable;

                        is_nullable = type_is_opt(&p_declarator->type, nullable_enabled);
                        flow_object_set_unknown(&p_declarator->type, is_nullable, p_declarator->p_flow_object, nullable_enabled);
                    }
                    else
                    {
                        if (type_is_array(&p_declarator->type))
                        {
                            p_declarator->p_flow_object->current.state = 64;
                        }
                        else
                        {
                            p_declarator->p_flow_object->current.state = 96;
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
    /*catch*/ else _CKL0:
    {
    }
}

static void flow_visit_init_declarator_list(struct flow_visit_ctx * ctx, struct init_declarator_list * p_init_declarator_list)
{
    struct init_declarator * p_init_declarator;

    p_init_declarator = p_init_declarator_list->head;
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
    struct member_declarator * p_member_declarator;

    p_member_declarator = p_member_declarator_list->head;
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
        flow_visit_member_declarator_list(ctx, p_member_declaration->member_declarator_list_opt);
    }
}

static void flow_visit_member_declaration_list(struct flow_visit_ctx * ctx, struct member_declaration_list * p_member_declaration_list)
{
    struct member_declaration * p_member_declaration;

    p_member_declaration = p_member_declaration_list->head;
    while (p_member_declaration)
    {
        flow_visit_member_declaration(ctx, p_member_declaration);
        p_member_declaration = p_member_declaration->next;
    }
}

static void flow_visit_struct_or_union_specifier(struct flow_visit_ctx * ctx, struct struct_or_union_specifier * p_struct_or_union_specifier)
{
    flow_visit_member_declaration_list(ctx, &p_struct_or_union_specifier->member_declaration_list);
}

static void flow_visit_enumerator(struct flow_visit_ctx * ctx, struct enumerator * p_enumerator)
{
    struct true_false_set  a;

    _cake_zmem(&a, 12);
    if (p_enumerator->constant_expression_opt)
    {
        flow_visit_expression(ctx, p_enumerator->constant_expression_opt, &a);
    }
    true_false_set_destroy(&a);
}

static void flow_visit_enumerator_list(struct flow_visit_ctx * ctx, struct enumerator_list * p_enumerator_list)
{
    struct enumerator * current;

    current = p_enumerator_list->head;
    while (current)
    {
        flow_visit_enumerator(ctx, current);
        current = current->next;
    }
}

static void flow_visit_enum_specifier(struct flow_visit_ctx * ctx, struct enum_specifier * p_enum_specifier)
{
    flow_visit_enumerator_list(ctx, &p_enum_specifier->enumerator_list);
}

static void flow_visit_type_specifier(struct flow_visit_ctx * ctx, struct type_specifier * p_type_specifier)
{
    if (p_type_specifier->struct_or_union_specifier)
    {
        flow_visit_struct_or_union_specifier(ctx, p_type_specifier->struct_or_union_specifier);
    }
    if (p_type_specifier->enum_specifier)
    {
        flow_visit_enum_specifier(ctx, p_type_specifier->enum_specifier);
    }
}

static void flow_visit_type_specifier_qualifier(struct flow_visit_ctx * ctx, struct type_specifier_qualifier * p_type_specifier_qualifier)
{
    if (p_type_specifier_qualifier->type_specifier)
    {
        flow_visit_type_specifier(ctx, p_type_specifier_qualifier->type_specifier);
    }
}

static void flow_visit_declaration_specifier(struct flow_visit_ctx * ctx, struct declaration_specifier * p_declaration_specifier)
{
    if (p_declaration_specifier->type_specifier_qualifier)
    {
        flow_visit_type_specifier_qualifier(ctx, p_declaration_specifier->type_specifier_qualifier);
    }
}

static void flow_visit_declaration_specifiers(struct flow_visit_ctx * ctx, struct declaration_specifiers * p_declaration_specifiers, struct type * p_type_opt)
{
    struct declaration_specifier * p_declaration_specifier;

    p_declaration_specifier = p_declaration_specifiers->head;
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
    if (p_declaration->declaration_specifiers)
    {
        if (p_declaration->init_declarator_list.head)
        {
            flow_visit_declaration_specifiers(ctx, p_declaration->declaration_specifiers, &p_declaration->init_declarator_list.head->p_declarator->type);
        }
        else
        {
            flow_visit_declaration_specifiers(ctx, p_declaration->declaration_specifiers, 0U);
        }
    }
    if (p_declaration->init_declarator_list.head)
    {
        flow_visit_init_declarator_list(ctx, &p_declaration->init_declarator_list);
    }
    if (p_declaration->function_body)
    {
        struct type  type;

        ;
        type = get_function_return_type(&p_declaration->init_declarator_list.head->p_declarator->type);
        ctx->p_return_type = &type;
        flow_visit_compound_statement(ctx, p_declaration->function_body);
        type_destroy(&type);
        ctx->p_return_type = 0U;
    }
}

void flow_start_visit_declaration(struct flow_visit_ctx * ctx, struct declaration * p_declaration)
{
    ctx->labels_size = 0;
    flow_objects_clear(&ctx->arena);
    ctx->state_number_generator = 1;
    if (p_declaration->function_body)
    {
        flow_visit_declaration(ctx, p_declaration);
        ;
        if (!flow_is_last_item_return(p_declaration->function_body))
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

struct flow_object *arena_new_object(struct flow_visit_ctx * ctx)
{
    struct flow_object * p;

    p = calloc(1, 68U);
    if (p != 0U)
    {
        p->current.dbg_name = "current";
        p->id = ctx->arena.size + 1;
        if (flow_objects_push_back(&ctx->arena, p) != 0)
        {
            p = 0U;
        }
    }
    return (struct flow_object *)p;
}

void flow_visit_ctx_destroy(struct flow_visit_ctx * p)
{
    flow_objects_destroy(&p->arena);
}

