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

struct macro;
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

struct atomic_type_specifier {
    struct token * token;
    struct type_name * type_name;
};

struct balanced_token {
    struct token * token;
    struct balanced_token * next;
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

struct declaration_specifier {
    struct storage_class_specifier * storage_class_specifier;
    struct type_specifier_qualifier * type_specifier_qualifier;
    struct function_specifier * function_specifier;
    struct declaration_specifier * next;
};

struct secondary_block {
    struct token * first_token;
    struct token * last_token;
    struct statement * statement;
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

struct primary_block {
    struct compound_statement * compound_statement;
    struct selection_statement * selection_statement;
    struct iteration_statement * iteration_statement;
    struct defer_statement * defer_statement;
    struct try_statement * try_statement;
};

struct array_declarator {
    struct direct_declarator * direct_declarator;
    struct expression * assignment_expression;
    struct expression * expression;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * token;
    struct token * static_token_opt;
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

struct pointer {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct pointer * pointer;
};

struct init_declarator_list {
    struct init_declarator * head;
    struct init_declarator * tail;
};

struct initializer_list_item {
    struct designation * designation;
    struct initializer * initializer;
    struct initializer_list_item * next;
};

struct direct_declarator {
    struct token * name_opt;
    struct declarator * declarator;
    struct array_declarator * array_declarator;
    struct function_declarator * function_declarator;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
};

struct designator {
    struct expression * constant_expression_opt;
    struct token * token;
    struct designator * next;
};

struct object_name_list {
    char * name;
    struct object_name_list * previous;
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

struct init_declarator {
    unsigned char   has_shared_ownership;
    struct declarator * p_declarator;
    struct initializer * initializer;
    struct init_declarator * next;
};

struct labeled_statement {
    struct label * label;
    struct statement * statement;
};

struct type_name {
    struct token * first_token;
    struct token * last_token;
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct declarator * abstract_declarator;
    struct type  type;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
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

struct member_declaration {
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct member_declarator_list * member_declarator_list_opt;
    struct static_assert_declaration * static_assert_declaration;
    struct pragma_declaration * pragma_declaration;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct member_declaration * next;
};

struct simple_declaration {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
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

struct label {
    struct expression * constant_expression;
    struct token * p_identifier_opt;
    struct token * p_first_token;
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

struct function_specifier {
    int   flags;
    struct token * token;
};

struct object_visitor {
    int member_index;
    struct type * p_type;
    struct flow_object * p_object;
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

struct scope_list {
    struct scope * head;
    struct scope * tail;
};

struct switch_value {
    long long value;
    struct label * p_label;
    struct switch_value * next;
};

struct designation {
    struct designator_list * designator_list;
    struct token * token;
};

struct attribute {
    int   attributes_flags;
    struct attribute_token * attribute_token;
    struct attribute_argument_clause * attribute_argument_clause;
    struct attribute * next;
};

struct typeof_specifier {
    struct token * first_token;
    struct token * last_token;
    struct typeof_specifier_argument * typeof_specifier_argument;
    struct type  type;
};

struct statement {
    struct labeled_statement * labeled_statement;
    struct unlabeled_statement * unlabeled_statement;
};

struct type_specifier_qualifier {
    struct type_specifier * type_specifier;
    struct type_qualifier * type_qualifier;
    struct alignment_specifier * alignment_specifier;
    struct type_specifier_qualifier * next;
};

struct try_statement {
    struct secondary_block * secondary_block;
    struct secondary_block * catch_secondary_block_opt;
    struct token * first_token;
    struct token * last_token;
    struct token * catch_token_opt;
    int try_catch_block_index;
};

struct static_assert_declaration {
    struct token * first_token;
    struct token * last_token;
    struct expression * constant_expression;
    struct token * string_literal_opt;
};

struct defer_list_item {
    struct declarator * declarator;
    struct defer_statement * defer_statement;
    struct defer_list_item * next;
};

struct __crt_locale_data;
struct alignment_specifier {
    struct type_name * type_name;
    struct expression * constant_expression;
    struct token * token;
};

struct argument_expression_list {
    struct argument_expression * head;
    struct argument_expression * tail;
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

struct attribute_token {
    int   attributes_flags;
    struct token * token;
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

struct param {
    struct type  type;
    struct param * next;
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

struct initializer {
    struct token * first_token;
    struct braced_initializer * braced_initializer;
    struct expression * assignment_expression;
};

struct storage_class_specifier {
    int   flags;
    struct token * token;
};

struct struct_entry;
struct enumerator_list {
    struct enumerator * head;
    struct enumerator * tail;
};

struct initializer_list {
    struct token * first_token;
    struct initializer_list_item * head;
    struct initializer_list_item * tail;
    int size;
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

struct type_specifier {
    int   flags;
    struct token * token;
    struct struct_or_union_specifier * struct_or_union_specifier;
    struct typeof_specifier * typeof_specifier;
    struct enum_specifier * enum_specifier;
    struct declarator * typedef_declarator;
    struct atomic_type_specifier * atomic_type_specifier;
};

struct attribute_specifier_sequence {
    struct token * first_token;
    struct token * last_token;
    int   attributes_flags;
    struct attribute_specifier * head;
    struct attribute_specifier * tail;
};

struct switch_value_list {
    struct switch_value * head;
    struct switch_value * tail;
    struct switch_value * p_default;
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

struct pragma_declaration {
    struct token * first_token;
    struct token * last_token;
};

struct type_qualifier_list {
    int   flags;
    struct type_qualifier * head;
    struct type_qualifier * tail;
};

struct parameter_type_list {
    unsigned char   is_var_args;
    unsigned char   is_void;
    struct parameter_list * parameter_list;
};

struct attribute_specifier {
    struct token * first_token;
    struct token * last_token;
    struct attribute_list * attribute_list;
    struct attribute_specifier * next;
};

struct osstream {
    char * c_str;
    int size;
    int capacity;
};

struct parameter_list {
    struct parameter_declaration * head;
    struct parameter_declaration * tail;
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

struct expression_statement {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct expression * expression_opt;
};

struct unlabeled_statement {
    struct expression_statement * expression_statement;
    struct primary_block * primary_block;
    struct jump_statement * jump_statement;
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

struct type_qualifier {
    int   flags;
    struct token * token;
    struct type_qualifier * next;
};

struct argument_expression {
    struct expression * expression;
    struct argument_expression * next;
    unsigned char   set_unkown;
};

struct member_declarator {
    struct declarator * declarator;
    struct expression * constant_expression;
    struct member_declarator * next;
};

struct member_declarator_list {
    struct member_declarator * head;
    struct member_declarator * tail;
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

struct jump_statement {
    struct token * label;
    struct token * first_token;
    struct token * last_token;
    struct expression * expression_opt;
    int try_catch_block_index;
    struct defer_list  defer_list;
};

struct condition {
    struct expression * expression;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator * p_init_declarator;
    struct token * first_token;
    struct token * last_token;
};

struct __crt_multibyte_data;
struct init_statement {
    struct expression_statement * p_expression_statement;
    struct simple_declaration * p_simple_declaration;
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

struct designator_list {
    struct designator * head;
    struct designator * tail;
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

struct typeof_specifier_argument {
    struct expression * expression;
    struct type_name * type_name;
};

struct parameter_declaration {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct declaration_specifiers * declaration_specifiers;
    struct declarator * declarator;
    struct parameter_declaration * next;
};

struct attribute_list {
    int   attributes_flags;
    struct attribute * head;
    struct attribute * tail;
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

struct defer_statement {
    struct token * first_token;
    struct token * last_token;
    struct secondary_block * secondary_block;
};

struct _iobuf {
    void * _Placeholder;
};


unsigned int s_visit_number = 1;
unsigned char  flow_object_is_not_null(struct flow_object * p)
{
    int   e = p->current.state;
    return ( !(e & 2) && ((e & 4) || (e & 8)));
}

unsigned char  flow_object_can_be_not_null_or_moved(struct flow_object * p)
{
    int   e = p->current.state;
    return (e & 4) || (e & 8);
}

unsigned char  flow_object_is_null(struct flow_object * p)
{
    int   e = p->current.state;
    return ((e & 2) &&  !(e & 4) &&  !(e & 8));
}

unsigned char  flow_object_is_zero(struct flow_object * p)
{
    int   e = p->current.state;
    return ((e & 32) &&  !(e & 64));
}

unsigned char  flow_object_is_not_zero(struct flow_object * p)
{
    int   e = p->current.state;
    return ( !(e & 32) && (e & 64));
}

unsigned char  flow_object_can_be_zero(struct flow_object * p)
{
    int   e = p->current.state;
    return (e & 32);
}

unsigned char  flow_object_can_be_moved(struct flow_object * p)
{
    int   e = p->current.state;
    return (e & 8);
}

unsigned char  flow_object_can_be_null(struct flow_object * p)
{
    int   e = p->current.state;
    return (e & 2);
}

unsigned char  flow_object_is_uninitialized(struct flow_object * p)
{
    int   e = p->current.state;
    return e == 1;
}

unsigned char  flow_object_can_be_uninitialized(struct flow_object * p)
{
    int   e = p->current.state;
    return (e & 1);
}

unsigned char  flow_object_can_have_its_lifetime_ended(struct flow_object * p)
{
    int   e = p->current.state;
    return (e & 128);
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

static void object_state_to_string_core(int   e)
{
    unsigned char   first = 1;
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
    return (p_object && p_object->current.state != 2 && p_object->current.state != 1 && p_object->current.pointed == 0);
}

struct type type_remove_pointer(struct type * p_type);
unsigned char  type_is_void(struct type * p_type);
struct flow_object *make_flow_object(struct flow_visit_ctx * ctx, struct type * p_type, struct declarator * p_declarator_opt, struct expression * p_expression_origin);
unsigned char  type_is_nullable(struct type * p_type, unsigned char   nullable_enabled);
void flow_object_set_unknown(struct type * p_type, unsigned char   t_is_nullable, struct flow_object * p_object, unsigned char   nullable_enabled);
void object_set_pointer(struct flow_object * p_object, struct flow_object * p_object2);
void flow_object_push_states_from(struct flow_object * p_object_from, struct flow_object * p_object_to);
void type_destroy(struct type * p_type);

void flow_object_expand_pointer(struct flow_visit_ctx * ctx, struct type * p_type, struct flow_object * p_object)
{
    ;
    if (flow_object_is_expansible(p_object))
    {
        unsigned char   nullable_enabled = ctx->ctx->options.null_checks_enabled;
        struct type  t2 = type_remove_pointer(p_type);
        if ( !type_is_void(&t2))
        {
            struct flow_object * p_object2 = make_flow_object(ctx, &t2, p_object->p_declarator_origin, p_object->p_expression_origin);
            if (p_object2)
            {
                unsigned char   is_nullable = type_is_nullable(&t2, nullable_enabled);
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
    struct flow_object  temp =  *a;
     *a =  *b;
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
    objects_view_destroy(&p->members);
    struct flow_object_state * p_flow_object_state = p->current.next;
    while (p_flow_object_state)
    {
        struct flow_object_state * temp = p_flow_object_state->next;
        p_flow_object_state->next = ((void *)0);
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
            int i = 0;
            for (; i < p->current.alternatives.size; i++)
            {
                flow_object_update_current(p->current.alternatives.data[i]);
                p->current.state = p->current.alternatives.data[i]->current.state;
            }
        }
    }
}

void flow_object_set_current_state_to_can_be_null(struct flow_object * p)
{
    p->current.state = 2;
}

void flow_object_set_current_state_to_is_null(struct flow_object * p)
{
    p->current.state = 2;
    if (p->current.alternatives.size > 0)
    {
        {
            int i = 0;
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

int objects_view_push_back(struct flow_objects_view * p, struct flow_object * p_object)
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
        int error = objects_view_reserve(p, new_capacity);
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
        int i = 0;
        for (; i < source->size; i++)
        {
            if ( !objects_view_find(dest, source->data[i]))
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
        int i = 0;
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
        int i = 0;
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
        int i = 0;
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
        int i = 0;
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

struct flow_object *flow_objects_find(struct flow_objects * p_objects, struct flow_object * object)
{
    {
        int i = 0;
        for (; i < p_objects->size; i++)
        {
            if (p_objects->data[i] == object)
            {
                return object;
            }
        }
    }
    return ((void *)0);
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
        int error = objects_reserve(p, new_capacity);
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

int strcmp(char * _Str1, char * _Str2);

unsigned char  has_name(char * name, struct object_name_list * list)
{
    struct object_name_list * p = list;
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
    if (p_declarator_opt == ((void *)0))
    {
    }
    if (p_expression_origin == ((void *)0))
    {
    }
    struct flow_object * p_object = arena_new_object(ctx);
    if (1)
    {
        if (p_object == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_object->p_expression_origin = p_expression_origin;
        p_object->p_declarator_origin = p_declarator_opt;
        if (p_type->struct_or_union_specifier)
        {
            struct struct_or_union_specifier * p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
            if (p_struct_or_union_specifier)
            {
                p_object->current.state = 0;
                struct member_declaration * p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;
                struct object_name_list  l = {0};
                l.name = p_struct_or_union_specifier->tag_name;
                l.previous = list;
                while (p_member_declaration)
                {
                    if (p_member_declaration->member_declarator_list_opt)
                    {
                        struct member_declarator * p_member_declarator = p_member_declaration->member_declarator_list_opt->head;
                        while (p_member_declarator)
                        {
                            if (p_member_declarator->declarator)
                            {
                                char * tag = ((void *)0);
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
                                    struct flow_object * member_obj = arena_new_object(ctx);
                                    if (member_obj == ((void *)0))
                                    {
                                        goto _catch_label_1;
                                    }
                                    member_obj->parent = p_object;
                                    member_obj->p_expression_origin = p_expression_origin;
                                    member_obj->p_declarator_origin = p_declarator_opt;
                                    member_obj->current.state = 0;
                                    objects_view_push_back(&p_object->members, member_obj);
                                }
                                else
                                {
                                    struct flow_object * p_member_obj = make_object_core(ctx, &p_member_declarator->declarator->type, &l, p_declarator_opt, p_expression_origin);
                                    if (p_member_obj == ((void *)0))
                                    {
                                        goto _catch_label_1;
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
                        if (p_member_declaration->specifier_qualifier_list != ((void *)0))
                        {
                            if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                            {
                                struct type  t = {0};
                                t.category = 0;
                                t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                                t.type_specifier_flags = 32768;
                                struct flow_object * member_obj = make_object_core(ctx, &t, &l, p_declarator_opt, p_expression_origin);
                                if (member_obj == ((void *)0))
                                {
                                    type_destroy(&t);
                                    goto _catch_label_1;
                                }
                                {
                                    int k = 0;
                                    for (; k < member_obj->members.size; k++)
                                    {
                                        objects_view_push_back(&p_object->members, member_obj->members.data[k]);
                                        member_obj->members.data[k] = ((void *)0);
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
    else _catch_label_1:
    {
        p_object = ((void *)0);
    }
    return p_object;
}

struct flow_object *make_flow_object(struct flow_visit_ctx * ctx, struct type * p_type, struct declarator * p_declarator_opt, struct expression * p_expression_origin)
{
    struct object_name_list  list = {"", 0};
    struct flow_object * p_object = make_object_core(ctx, p_type, &list, p_declarator_opt, p_expression_origin);
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
    return ((void *)0);
}

void *calloc(int nmemb, unsigned int size);

void flow_object_add_new_state_as_a_copy_of_current_state(struct flow_object * object, char * name, int state_number)
{
    struct flow_object_state * pnew = calloc(1, sizeof  *pnew);
    if (pnew == ((void *)0))
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
    struct flow_object_state * previous = &object->current;
    struct flow_object_state * it = object->current.next;
    while (it)
    {
        if (it->state_number == state_number)
        {
            struct flow_object_state * p_it_next = it->next;
            it->next = ((void *)0);
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
        ;
        struct struct_or_union_specifier * p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);
        if (p_struct_or_union_specifier)
        {
            printf("%*c", ident + 1, 32);
            printf("#%02d {\n", p_visitor->p_object->id);
            struct member_declaration * p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;
            while (p_member_declaration)
            {
                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator * p_member_declarator = p_member_declaration->member_declarator_list_opt->head;
                    while (p_member_declarator)
                    {
                        if (p_member_declarator->declarator)
                        {
                            char * name = p_member_declarator->declarator->name_opt ? p_member_declarator->declarator->name_opt->lexeme : "";
                            char buffer[200] = {0};
                            if (is_pointer)
                            {
                                snprintf(buffer, sizeof buffer, "%s->%s", previous_names, name);
                            }
                            else
                            {
                                snprintf(buffer, sizeof buffer, "%s.%s", previous_names, name);
                            }
                            struct object_visitor  visitor = {0};
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
                    if (p_member_declaration->specifier_qualifier_list != ((void *)0))
                    {
                        struct type  t = {0};
                        t.category = 0;
                        t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                        t.type_specifier_flags = 32768;
                        struct type * temp = p_visitor->p_type;
                        p_visitor->p_type = &t;
                        int visit_number0 = p_visitor->p_object->visit_number;
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
            struct type  t2 = type_remove_pointer(p_visitor->p_type);
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
                        if (p_visitor->p_object->current.pointed != ((void *)0))
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
                printf("%p:%s == ", p_visitor->p_object, previous_names);
                printf("{");
                struct flow_object_state * it = p_visitor->p_object->current.next;
                while (it)
                {
                    printf("\x1b[36;1m");
                    printf("(#%02d %s)", it->state_number, it->dbg_name);
                    object_state_set_item_print(it);
                    printf("\x1b");
                    printf(",");
                    it = it->next;
                }
                printf("\x1b[95m");
                printf("(current)");
                flow_object_print_state(p_visitor->p_object);
                printf("\x1b");
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
                printf("%p:%s == ", p_visitor->p_object, previous_names);
                printf("{");
                struct flow_object_state * it = p_visitor->p_object->current.next;
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
            int i = 0;
            for (; i < object1->members.size; i++)
            {
                struct flow_object * m1 = object1->members.data[i];
                struct flow_object * m2 = object2->members.data[i];
                flow_object_merge_state(pdest->members.data[i], m1, m2);
            }
        }
    }
}

int object_restore_current_state_from_core(struct flow_object * object, int state_number, unsigned int visit_number)
{
    if (object->visit_number == visit_number)
    {
        return 1;
    }
    object->visit_number = visit_number;
    struct flow_object_state * it = object->current.next;
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
        int i = 0;
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
    if (object->visit_number == visit_number)
    {
        return 1;
    }
    object->visit_number = visit_number;
    struct flow_object_state * it = object->current.next;
    while (it)
    {
        if (it->state_number == state_number)
        {
            it->state = object->current.state;
            break;
        }
        it = it->next;
    }
    {
        int i = 0;
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
    char name[100] = {0};
    object_get_name(p_type, p_object, name, sizeof name);
    struct object_visitor  visitor = {0};
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    print_object_core(0, &visitor, name, type_is_pointer(p_type), short_version, s_visit_number++);
}

void object_set_uninitialized_core(struct object_visitor * p_visitor)
{
    if (p_visitor->p_type->struct_or_union_specifier)
    {
        struct struct_or_union_specifier * p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);
        if (p_struct_or_union_specifier)
        {
            struct member_declaration * p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;
            while (p_member_declaration)
            {
                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator * p_member_declarator = p_member_declaration->member_declarator_list_opt->head;
                    while (p_member_declarator)
                    {
                        if (p_member_declarator->declarator)
                        {
                            if (p_visitor->member_index < p_visitor->p_object->members.size)
                            {
                                struct object_visitor  visitor = {0};
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
                    if (p_member_declaration->specifier_qualifier_list != ((void *)0))
                    {
                        if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                        {
                            struct type  t = {0};
                            t.category = 0;
                            t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                            t.type_specifier_flags = 32768;
                            struct type * type_temp = p_visitor->p_type;
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
    struct object_visitor  visitor = {0};
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    object_set_uninitialized_core(&visitor);
}

unsigned char  type_is_any_owner(struct type * p_type);
unsigned char  compiler_diagnostic_message(int   w, struct parser_ctx * ctx, struct token * p_token, struct marker * p_marker, char * fmt, ...);

static void checked_empty_core(struct flow_visit_ctx * ctx, struct type * p_type, struct flow_object * p_object, char * previous_names, struct marker * p_marker)
{
    if (p_type->struct_or_union_specifier && p_object->members.size > 0)
    {
        struct struct_or_union_specifier * p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
        struct member_declaration * p_member_declaration = p_struct_or_union_specifier ? p_struct_or_union_specifier->member_declaration_list.head : ((void *)0);
        int member_index = 0;
        while (p_member_declaration)
        {
            if (p_member_declaration->member_declarator_list_opt)
            {
                struct member_declarator * p_member_declarator = p_member_declaration->member_declarator_list_opt->head;
                while (p_member_declarator)
                {
                    if (p_member_declarator->declarator)
                    {
                        char * name = p_member_declarator->declarator->name_opt ? p_member_declarator->declarator->name_opt->lexeme : "";
                        char buffer[200] = {0};
                        if (type_is_pointer(p_type))
                        {
                            snprintf(buffer, sizeof buffer, "%s->%s", previous_names, name);
                        }
                        else
                        {
                            snprintf(buffer, sizeof buffer, "%s.%s", previous_names, name);
                        }
                        checked_empty_core(ctx, &p_member_declarator->declarator->type, p_object->members.data[member_index], name, p_marker);
                        member_index++;
                    }
                    p_member_declarator = p_member_declarator->next;
                }
            }
            else
            {
                if (p_member_declaration->specifier_qualifier_list != ((void *)0))
                {
                }
            }
            p_member_declaration = p_member_declaration->next;
        }
        return;
    }
    if (type_is_any_owner(p_type))
    {
        if (p_object->current.state == (1 | 2 | 8) || p_object->current.state == (2 | 8) || p_object->current.state == 2 || p_object->current.state == 8 || p_object->current.state == 1 || p_object->current.state == (1 | 2) || p_object->current.state == (1 | 8))
        {
        }
        else
        {
            compiler_diagnostic_message(28, ctx->ctx, ((void *)0), p_marker, "object '%s' may not be empty", previous_names);
        }
    }
}

void checked_empty(struct flow_visit_ctx * ctx, struct type * p_type, struct flow_object * p_object, struct marker * p_marker)
{
    char name[100] = {0};
    object_get_name(p_type, p_object, name, sizeof name);
    checked_empty_core(ctx, p_type, p_object, name, p_marker);
}

static void object_set_moved_core(struct object_visitor * p_visitor)
{
    if (p_visitor->p_type->struct_or_union_specifier)
    {
        struct struct_or_union_specifier * p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);
        if (p_struct_or_union_specifier)
        {
            struct member_declaration * p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;
            while (p_member_declaration)
            {
                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator * p_member_declarator = p_member_declaration->member_declarator_list_opt->head;
                    while (p_member_declarator)
                    {
                        if (p_member_declarator->declarator)
                        {
                            if (p_visitor->member_index < p_visitor->p_object->members.size)
                            {
                                struct object_visitor  visitor = {0};
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
                    if (p_member_declaration->specifier_qualifier_list != ((void *)0))
                    {
                        if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                        {
                            struct type  t = {0};
                            t.category = 0;
                            t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                            t.type_specifier_flags = 32768;
                            struct type * type_temp = p_visitor->p_type;
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
    struct object_visitor  visitor = {0};
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
        struct struct_or_union_specifier * p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);
        if (p_struct_or_union_specifier)
        {
            struct member_declaration * p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;
            while (p_member_declaration)
            {
                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator * p_member_declarator = p_member_declaration->member_declarator_list_opt->head;
                    while (p_member_declarator)
                    {
                        if (p_member_declarator->declarator)
                        {
                            if (p_visitor->member_index < p_visitor->p_object->members.size)
                            {
                                struct object_visitor  visitor = {0};
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
                    if (p_member_declaration->specifier_qualifier_list != ((void *)0))
                    {
                        if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                        {
                            struct type  t = {0};
                            t.category = 0;
                            t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                            t.type_specifier_flags = 32768;
                            struct type * temp = p_visitor->p_type;
                            p_visitor->p_type = &t;
                            int vn = p_visitor->p_object->visit_number;
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
        if (t_is_nullable || type_is_nullable(p_visitor->p_type, nullable_enabled))
        {
            p_visitor->p_object->current.state = 2 | 4;
        }
        else
        {
            p_visitor->p_object->current.state = 4;
        }
        struct flow_object * pointed = p_visitor->p_object->current.pointed;
        if (pointed)
        {
            struct type  t2 = type_remove_pointer(p_visitor->p_type);
            unsigned char   t2_is_nullable = type_is_nullable(&t2, nullable_enabled);
            struct object_visitor  visitor = {0};
            visitor.p_type = &t2;
            visitor.p_object = pointed;
            object_set_unknown_core(&visitor, t2_is_nullable, visit_number, nullable_enabled);
            type_destroy(&t2);
        }
    }
    else
    {
        if ( !type_is_struct_or_union(p_visitor->p_type))
        {
            p_visitor->p_object->current.state = 32 | 64;
        }
    }
}

void flow_object_set_unknown(struct type * p_type, unsigned char   t_is_nullable, struct flow_object * p_object, unsigned char   nullable_enabled)
{
    struct object_visitor  visitor = {0};
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
        struct struct_or_union_specifier * p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
        if (p_struct_or_union_specifier)
        {
            struct member_declaration * p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;
            int member_index = 0;
            while (p_member_declaration)
            {
                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator * p_member_declarator = p_member_declaration->member_declarator_list_opt->head;
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
                    if (p_member_declaration->specifier_qualifier_list != ((void *)0))
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
        if ( !type_is_struct_or_union(p_type))
        {
            p_object->current.state = 128;
        }
    }
}

void flow_object_set_can_be_uninitialized(struct flow_object * p_object)
{
    p_object->current.state = 1;
}

void flow_object_set_is_unitialized(struct flow_object * p_object)
{
    p_object->current.state = 1;
    if (p_object->current.alternatives.size > 0)
    {
        {
            int i = 0;
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
        p_object->current.state =  ~4;
        p_object->current.state = 8;
        p_object->current.state = 8;
    }
}

void flow_object_set_is_moved(struct flow_object * p_object)
{
    p_object->current.state = 8;
    if (p_object->current.alternatives.size > 0)
    {
        {
            int i = 0;
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
        struct struct_or_union_specifier * p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);
        if (p_struct_or_union_specifier)
        {
            struct member_declaration * p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;
            while (p_member_declaration)
            {
                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator * p_member_declarator = p_member_declaration->member_declarator_list_opt->head;
                    while (p_member_declarator)
                    {
                        if (p_member_declarator->declarator)
                        {
                            if (p_visitor->member_index < p_visitor->p_object->members.size)
                            {
                                struct object_visitor  visitor = {0};
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
                    if (p_member_declaration->specifier_qualifier_list != ((void *)0))
                    {
                        if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                        {
                            struct type  t = {0};
                            t.category = 0;
                            t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                            t.type_specifier_flags = 32768;
                            struct type * temp = p_visitor->p_type;
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
    struct object_visitor  visitor = {0};
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    object_set_zero_core(&visitor);
}

void object_set_end_of_lifetime_core(struct object_visitor * p_visitor)
{
    if (p_visitor->p_type->struct_or_union_specifier)
    {
        struct struct_or_union_specifier * p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);
        if (p_struct_or_union_specifier)
        {
            struct member_declaration * p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;
            while (p_member_declaration)
            {
                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator * p_member_declarator = p_member_declaration->member_declarator_list_opt->head;
                    while (p_member_declarator)
                    {
                        if (p_member_declarator->declarator)
                        {
                            if (p_visitor->member_index < p_visitor->p_object->members.size)
                            {
                                struct object_visitor  visitor = {0};
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
                    if (p_member_declaration->specifier_qualifier_list != ((void *)0))
                    {
                        if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                        {
                            struct type  t = {0};
                            t.category = 0;
                            t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                            t.type_specifier_flags = 32768;
                            struct type * temp = p_visitor->p_type;
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
    struct object_visitor  visitor = {0};
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    object_set_end_of_lifetime_core(&visitor);
}

unsigned char  type_is_owner(struct type * p_type);

unsigned char  object_check(struct type * p_type, struct flow_object * p_object)
{
    if (p_type->type_qualifier_flags & 64)
    {
        return 0;
    }
    if ( !type_is_any_owner(p_type))
    {
        return 0;
    }
    if (p_type->struct_or_union_specifier && p_object->members.size > 0)
    {
        struct struct_or_union_specifier * p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
        struct member_declaration * p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;
        int possible_need_destroy_count = 0;
        int need_destroy_count = 0;
        int member_index = 0;
        while (p_member_declaration)
        {
            if (p_member_declaration->member_declarator_list_opt)
            {
                struct member_declarator * p_member_declarator = p_member_declaration->member_declarator_list_opt->head;
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
                if (p_member_declaration->specifier_qualifier_list != ((void *)0))
                {
                }
            }
            p_member_declaration = p_member_declaration->next;
        }
        return need_destroy_count > 1 && (need_destroy_count == possible_need_destroy_count);
    }
    else
    {
        unsigned char   should_had_been_moved = 0;
        if (type_is_pointer(p_type))
        {
            should_had_been_moved = (p_object->current.state & 4);
        }
        else
        {
            if (p_object->current.state == 1 || (p_object->current.state & 8) || p_object->current.state == 4 || p_object->current.state == (1))
            {
            }
            else
            {
                should_had_been_moved = 1;
            }
        }
        return should_had_been_moved;
    }
    return 0;
}

void object_get_name_core(struct type * p_type, struct flow_object * p_object, struct flow_object * p_object_target, char * previous_names, char * outname, int out_size, unsigned int visit_number)
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
        struct struct_or_union_specifier * p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
        struct member_declaration * p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;
        int member_index = 0;
        while (p_member_declaration)
        {
            if (p_member_declaration->member_declarator_list_opt)
            {
                struct member_declarator * p_member_declarator = p_member_declaration->member_declarator_list_opt->head;
                while (p_member_declarator)
                {
                    if (p_member_declarator->declarator)
                    {
                        char * name = p_member_declarator->declarator->name_opt ? p_member_declarator->declarator->name_opt->lexeme : "";
                        char buffer[200] = {0};
                        if (type_is_pointer(p_type))
                        {
                            snprintf(buffer, sizeof buffer, "%s->%s", previous_names, name);
                        }
                        else
                        {
                            snprintf(buffer, sizeof buffer, "%s.%s", previous_names, name);
                        }
                        object_get_name_core(&p_member_declarator->declarator->type, p_object->members.data[member_index], p_object_target, buffer, outname, out_size, visit_number);
                        member_index++;
                    }
                    p_member_declarator = p_member_declarator->next;
                }
            }
            else
            {
                if (p_member_declaration->specifier_qualifier_list != ((void *)0))
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

void object_get_name(struct type * p_type, struct flow_object * p_object, char * outname, int out_size)
{
    outname[0] = 0;
    if (p_object->p_declarator_origin)
    {
        char * root_name = p_object->p_declarator_origin->name_opt ? p_object->p_declarator_origin->name_opt->lexeme : "?";
        struct flow_object * root = p_object->p_declarator_origin->p_object;
        object_get_name_core(&p_object->p_declarator_origin->type, root, p_object, root_name, outname, out_size, s_visit_number++);
    }
    else
    {
        if (p_object->p_expression_origin)
        {
            int bytes_written = 0;
            struct token * p = p_object->p_expression_origin->first_token;
            {
                int i = 0;
                for (; i < 10; i++)
                {
                    char * ps = p->lexeme;
                    while ( *ps)
                    {
                        if (bytes_written < (out_size - 1))
                        {
                            outname[bytes_written] =  *ps;
                        }
                        bytes_written++;
                        ps++;
                    }
                    if (p == p_object->p_expression_origin->last_token)
                    {
                        break;
                    }
                    p = p->next;
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

void checked_moved_core(struct flow_visit_ctx * ctx, struct type * p_type, struct flow_object * p_object, struct token * position_token, unsigned int visit_number)
{
    if (p_object->visit_number == visit_number)
    {
        return;
    }
    p_object->visit_number = visit_number;
    if (p_type->struct_or_union_specifier && p_object->members.size > 0)
    {
        struct struct_or_union_specifier * p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
        struct member_declaration * p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;
        int member_index = 0;
        while (p_member_declaration)
        {
            if (p_member_declaration->member_declarator_list_opt)
            {
                struct member_declarator * p_member_declarator = p_member_declaration->member_declarator_list_opt->head;
                while (p_member_declarator)
                {
                    if (p_member_declarator->declarator)
                    {
                        checked_moved_core(ctx, &p_member_declarator->declarator->type, p_object->members.data[member_index], position_token, visit_number);
                        member_index++;
                    }
                    p_member_declarator = p_member_declarator->next;
                }
            }
            else
            {
                if (p_member_declaration->specifier_qualifier_list != ((void *)0))
                {
                    ;
                }
            }
            p_member_declaration = p_member_declaration->next;
        }
        return;
    }
    else
    {
        if (type_is_pointer(p_type) &&  !type_is_any_owner(p_type))
        {
            if (p_object->current.state != 1 && p_object->current.state != 2)
            {
                struct type  t2 = type_remove_pointer(p_type);
                type_destroy(&t2);
            }
        }
        if (p_object->current.state & 8)
        {
            struct token * name_pos = flow_object_get_token(p_object);
            char * parameter_name = name_pos ? name_pos->lexeme : "?";
            char name[200] = {0};
            object_get_name(p_type, p_object, name, sizeof name);
            if (compiler_diagnostic_message(28, ctx->ctx, position_token, ((void *)0), "parameter '%s' is leaving scoped with a moved object '%s'", parameter_name, name))
            {
                compiler_diagnostic_message(62, ctx->ctx, name_pos, ((void *)0), "parameter", name);
            }
        }
        if (p_object->current.state & 1)
        {
            struct token * name_pos = flow_object_get_token(p_object);
            char * parameter_name = name_pos ? name_pos->lexeme : "?";
            char name[200] = {0};
            object_get_name(p_type, p_object, name, sizeof name);
            if (compiler_diagnostic_message(28, ctx->ctx, position_token, ((void *)0), "parameter '%s' is leaving scoped with a uninitialized object '%s'", parameter_name, name))
            {
                compiler_diagnostic_message(62, ctx->ctx, name_pos, ((void *)0), "parameter", name);
            }
        }
    }
}

void checked_moved(struct flow_visit_ctx * ctx, struct type * p_type, struct flow_object * p_object, struct token * position_token)
{
    checked_moved_core(ctx, p_type, p_object, position_token, s_visit_number++);
}

static void checked_read_object_core(struct flow_visit_ctx * ctx, struct object_visitor * p_visitor, unsigned char   is_nullable, struct token * position_token_opt, struct marker * p_marker_opt, unsigned char   check_pointed_object, char * previous_names, unsigned int visit_number)
{
    ;
    if (p_visitor->p_object == ((void *)0))
    {
        return;
    }
    if (p_visitor->p_object->visit_number == visit_number)
    {
        return;
    }
    p_visitor->p_object->visit_number = visit_number;
    if (p_visitor->p_type->struct_or_union_specifier && p_visitor->p_object->members.size > 0)
    {
        struct struct_or_union_specifier * p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);
        struct member_declaration * p_member_declaration = p_struct_or_union_specifier ? p_struct_or_union_specifier->member_declaration_list.head : ((void *)0);
        while (p_member_declaration)
        {
            if (p_member_declaration->member_declarator_list_opt)
            {
                struct member_declarator * p_member_declarator = p_member_declaration->member_declarator_list_opt->head;
                while (p_member_declarator)
                {
                    if (p_member_declarator->declarator)
                    {
                        char * name = p_member_declarator->declarator->name_opt ? p_member_declarator->declarator->name_opt->lexeme : "?";
                        char buffer[200] = {0};
                        if (type_is_pointer(p_visitor->p_type))
                        {
                            snprintf(buffer, sizeof buffer, "%s->%s", previous_names, name);
                        }
                        else
                        {
                            snprintf(buffer, sizeof buffer, "%s.%s", previous_names, name);
                        }
                        struct object_visitor  visitor = {0};
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
                    struct type  t = {0};
                    t.category = 0;
                    t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                    t.type_specifier_flags = 32768;
                    char buffer[200] = {0};
                    if (type_is_pointer(p_visitor->p_type))
                    {
                        snprintf(buffer, sizeof buffer, "%s->", previous_names);
                    }
                    else
                    {
                        snprintf(buffer, sizeof buffer, "%s.", previous_names);
                    }
                    struct type * temp = p_visitor->p_type;
                    p_visitor->p_type = &t;
                    int visit_number0 = p_visitor->p_object->visit_number;
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
        if (type_is_pointer(p_visitor->p_type) &&  !is_nullable &&  !type_is_nullable(p_visitor->p_type, ctx->ctx->options.null_checks_enabled) && flow_object_can_be_null(p_visitor->p_object))
        {
            compiler_diagnostic_message(32, ctx->ctx, ((void *)0), p_marker_opt, "non-nullable pointer '%s' may be null", previous_names);
        }
        if (type_is_pointer(p_visitor->p_type) && check_pointed_object && flow_object_can_be_not_null_or_moved(p_visitor->p_object))
        {
            struct type  t2 = type_remove_pointer(p_visitor->p_type);
            if (p_visitor->p_object->current.pointed)
            {
                struct object_visitor  visitor = {0};
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
                compiler_diagnostic_message(29, ctx->ctx, position_token_opt, p_marker_opt, "uninitialized object '%s'", previous_names);
            }
        }
        if (p_visitor->p_object->current.state & 128)
        {
            compiler_diagnostic_message(30, ctx->ctx, position_token_opt, p_marker_opt, "lifetime ended '%s'", previous_names);
        }
    }
}

void checked_read_object(struct flow_visit_ctx * ctx, struct type * p_type, unsigned char   is_nullable, struct flow_object * p_object, struct token * position_token, struct marker * p_marker_opt, unsigned char   check_pointed_object)
{
    char * s = ((void *)0);
    char name[200] = {0};
    object_get_name(p_type, p_object, name, sizeof name);
    struct object_visitor  visitor = {0};
    visitor.p_object = p_object;
    visitor.p_type = p_type;
    checked_read_object_core(ctx, &visitor, is_nullable, position_token, p_marker_opt, check_pointed_object, name, s_visit_number++);
    free((void *)s);
}

unsigned char  type_is_view(struct type * p_type);
unsigned char  type_is_obj_owner(struct type * p_type);

static void flow_end_of_block_visit_core(struct flow_visit_ctx * ctx, struct object_visitor * p_visitor, unsigned char   b_type_is_view, struct token * position_token, char * previous_names, unsigned int visit_number)
{
    if (p_visitor->p_object == ((void *)0))
    {
        return;
    }
    if (p_visitor->p_object->visit_number == visit_number)
    {
        return;
    }
    p_visitor->p_object->visit_number = visit_number;
    if (p_visitor->p_type->struct_or_union_specifier && p_visitor->p_object->members.size > 0)
    {
        struct struct_or_union_specifier * p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);
        if (p_struct_or_union_specifier == ((void *)0))
        {
            return;
        }
        struct member_declaration * p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;
        if (object_check(p_visitor->p_type, p_visitor->p_object))
        {
            struct token * name = flow_object_get_token(p_visitor->p_object);
            if (compiler_diagnostic_message(28, ctx->ctx, name, ((void *)0), "object '%s' was not moved/destroyed", previous_names))
            {
                if (p_visitor->p_object->p_declarator_origin)
                {
                    compiler_diagnostic_message(62, ctx->ctx, position_token, ((void *)0), "end of '%s' scope", previous_names);
                }
            }
        }
        else
        {
            while (p_member_declaration)
            {
                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator * p_member_declarator = p_member_declaration->member_declarator_list_opt->head;
                    while (p_member_declarator)
                    {
                        if (p_member_declarator->declarator)
                        {
                            char * name = p_member_declarator->declarator->name_opt ? p_member_declarator->declarator->name_opt->lexeme : "?";
                            char buffer[200] = {0};
                            if (type_is_pointer(p_visitor->p_type))
                            {
                                snprintf(buffer, sizeof buffer, "%s->%s", previous_names, name);
                            }
                            else
                            {
                                snprintf(buffer, sizeof buffer, "%s.%s", previous_names, name);
                            }
                            unsigned char   member_is_view = type_is_view(&p_member_declarator->declarator->type);
                            struct object_visitor  visitor = {0};
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
                    if (p_member_declaration->specifier_qualifier_list != ((void *)0))
                    {
                        struct type  t = {0};
                        t.category = 0;
                        t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                        t.type_specifier_flags = 32768;
                        struct type * temp = p_visitor->p_type;
                        p_visitor->p_type = &t;
                        unsigned char   member_is_view = type_is_view(&t);
                        int visit_number0 = p_visitor->p_object->visit_number;
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
        char * name = previous_names;
        struct token * position = ((void *)0);
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
        if (type_is_pointer(p_visitor->p_type) &&  !b_type_is_view && type_is_owner(p_visitor->p_type) && p_visitor->p_object->current.state & 4)
        {
            if (compiler_diagnostic_message(28, ctx->ctx, position, ((void *)0), "ownership of '%s' not moved before the end of lifetime", previous_names))
            {
                compiler_diagnostic_message(62, ctx->ctx, position_token, ((void *)0), "end of '%s' lifetime", previous_names);
            }
        }
        else
        {
            if ( !b_type_is_view && type_is_obj_owner(p_visitor->p_type) && type_is_pointer(p_visitor->p_type))
            {
                char buffer[100] = {0};
                snprintf(buffer, sizeof buffer, "%s", previous_names);
                struct type  t2 = type_remove_pointer(p_visitor->p_type);
                if (p_visitor->p_object->current.pointed)
                {
                    struct object_visitor  visitor = {0};
                    visitor.p_type = &t2;
                    visitor.p_object = p_visitor->p_object->current.pointed;
                    flow_end_of_block_visit_core(ctx, &visitor, b_type_is_view, position, buffer, visit_number);
                }
                type_destroy(&t2);
            }
            else
            {
                if (type_is_owner(p_visitor->p_type) &&  !type_is_pointer(p_visitor->p_type))
                {
                    if (p_visitor->p_object->current.state == 1 || p_visitor->p_object->current.state == 2 || p_visitor->p_object->current.state == 8)
                    {
                    }
                    else
                    {
                        if (compiler_diagnostic_message(28, ctx->ctx, position, ((void *)0), "ownership of '%s' not moved before the end of lifetime", previous_names))
                        {
                            compiler_diagnostic_message(62, ctx->ctx, position_token, ((void *)0), "end of '%s' lifetime", previous_names);
                        }
                    }
                }
                else
                {
                    if (type_is_pointer(p_visitor->p_type))
                    {
                        if (p_visitor->p_type->storage_class_specifier_flags & 2048)
                        {
                            if (type_is_any_owner(p_visitor->p_type))
                            {
                            }
                            else
                            {
                                struct type  t2 = type_remove_pointer(p_visitor->p_type);
                                if (p_visitor->p_object->current.pointed)
                                {
                                    struct token * name_token = p_visitor->p_object->p_declarator_origin->name_opt ? p_visitor->p_object->p_declarator_origin->name_opt : p_visitor->p_object->p_declarator_origin->first_token_opt;
                                    checked_read_object(ctx, &t2, 0, p_visitor->p_object->current.pointed, name_token, ((void *)0), 1);
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
    struct object_visitor  visitor = {0};
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    flow_end_of_block_visit_core(ctx, &visitor, type_is_view, position_token, previous_names, s_visit_number++);
}

unsigned char  flow_object_is_zero_or_null(struct flow_object * p_object)
{
    return (p_object->current.state == 2) || (p_object->current.state == 32);
}

struct type get_array_item_type(struct type * p_type);
unsigned char  type_is_out(struct type * p_type);
unsigned char  type_is_const(struct type * p_type);
unsigned char  type_is_nullptr_t(struct type * p_type);
unsigned char  type_is_integer(struct type * p_type);
unsigned char  type_is_void_ptr(struct type * p_type);

static void flow_assignment_core(struct flow_visit_ctx * ctx, struct token * error_position, struct marker * p_a_marker, struct marker * p_b_marker, int   assigment_type, unsigned char   check_uninitialized_b, unsigned char   a_type_is_view, unsigned char   a_type_is_nullable, struct object_visitor * p_visitor_a, struct object_visitor * p_visitor_b, unsigned char  * set_argument_to_unkown)
{
    if (check_uninitialized_b && flow_object_can_be_uninitialized(p_visitor_b->p_object))
    {
        if (type_is_array(p_visitor_b->p_type))
        {
            if (assigment_type == 1)
            {
                struct type  item_type = {0};
                if (type_is_array(p_visitor_a->p_type))
                {
                    item_type = get_array_item_type(p_visitor_a->p_type);
                }
                else
                {
                    item_type = type_remove_pointer(p_visitor_a->p_type);
                }
                unsigned char   cannot_be_uninitialized = (ctx->ctx->options.ownership_enabled &&  !type_is_out(&item_type)) || type_is_const(&item_type);
                if (cannot_be_uninitialized)
                {
                    char b_object_name[100] = {0};
                    object_get_name(p_visitor_b->p_type, p_visitor_b->p_object, b_object_name, sizeof b_object_name);
                    compiler_diagnostic_message(29, ctx->ctx, ((void *)0), p_b_marker, "uninitialized object '%s' passed to non-optional parameter", b_object_name);
                }
                type_destroy(&item_type);
            }
        }
        else
        {
            char b_object_name[100] = {0};
            object_get_name(p_visitor_b->p_type, p_visitor_b->p_object, b_object_name, sizeof b_object_name);
            if (assigment_type == 1)
            {
                compiler_diagnostic_message(29, ctx->ctx, ((void *)0), p_b_marker, "passing an uninitialized argument '%s' object", b_object_name);
            }
            else
            {
                if (assigment_type == 0)
                {
                    compiler_diagnostic_message(29, ctx->ctx, ((void *)0), p_b_marker, "returning an uninitialized '%s' object", b_object_name);
                }
                else
                {
                    compiler_diagnostic_message(29, ctx->ctx, ((void *)0), p_b_marker, "reading an uninitialized '%s' object", b_object_name);
                }
            }
        }
        return;
    }
    if (check_uninitialized_b && flow_object_can_have_its_lifetime_ended(p_visitor_a->p_object))
    {
        char buffer[100] = {0};
        object_get_name(p_visitor_a->p_type, p_visitor_a->p_object, buffer, sizeof buffer);
        compiler_diagnostic_message(30, ctx->ctx, ((void *)0), p_a_marker, "The object '%s' may have been deleted or its lifetime have ended.", buffer);
        return;
    }
    if (type_is_pointer(p_visitor_a->p_type) && ( !type_is_nullable(p_visitor_a->p_type, ctx->ctx->options.null_checks_enabled)) && flow_object_can_be_null(p_visitor_b->p_object))
    {
        if ( !a_type_is_nullable)
        {
            char buffer[100] = {0};
            object_get_name(p_visitor_b->p_type, p_visitor_b->p_object, buffer, sizeof buffer);
            if (assigment_type == 1)
            {
                compiler_diagnostic_message(34, ctx->ctx, ((void *)0), p_b_marker, "passing a possible null pointer '%s' to non-nullable pointer parameter", buffer);
            }
            else
            {
                if (assigment_type == 0)
                {
                    compiler_diagnostic_message(34, ctx->ctx, ((void *)0), p_b_marker, "returning a possible null pointer '%s' to non-nullable pointer", buffer);
                }
                else
                {
                    compiler_diagnostic_message(34, ctx->ctx, ((void *)0), p_b_marker, "assignment of possible null pointer '%s' to non-nullable pointer", buffer);
                }
            }
        }
    }
    if (type_is_pointer(p_visitor_a->p_type))
    {
        if ( !a_type_is_view && type_is_owner(p_visitor_a->p_type))
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
    if ( !a_type_is_view && type_is_obj_owner(p_visitor_a->p_type) && type_is_pointer(p_visitor_a->p_type))
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
        if ( !a_type_is_view && type_is_owner(p_visitor_a->p_type))
        {
            struct type  t = type_remove_pointer(p_visitor_b->p_type);
            if (p_visitor_b->p_object->current.pointed == ((void *)0))
            {
                if (flow_object_is_expansible(p_visitor_b->p_object) && type_is_owner(&t))
                {
                    compiler_diagnostic_message(28, ctx->ctx, ((void *)0), p_a_marker, "pointed object may be not empty");
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
        p_visitor_a->p_object->current.state = p_visitor_b->p_object->current.state;
        p_visitor_a->p_object->current.pointed = p_visitor_b->p_object->current.pointed;
        struct type  t = type_remove_pointer(p_visitor_a->p_type);
        unsigned char   checked_pointed_object_read = ctx->ctx->options.ownership_enabled &&  !type_is_out(&t);
        unsigned char   is_nullable = a_type_is_nullable || type_is_nullable(&t, ctx->ctx->options.null_checks_enabled);
        checked_read_object(ctx, p_visitor_b->p_type, is_nullable, p_visitor_b->p_object, error_position, p_b_marker, checked_pointed_object_read);
        type_destroy(&t);
        if ( !a_type_is_view && type_is_owner(p_visitor_a->p_type))
        {
            if (flow_object_can_be_moved(p_visitor_b->p_object))
            {
                compiler_diagnostic_message(31, ctx->ctx, error_position, ((void *)0), "source object has already been moved");
            }
            if (assigment_type == 1)
            {
                p_visitor_b->p_object->current.state = 1;
                if (p_visitor_b->p_object->current.pointed)
                {
                    struct flow_object * pointed = p_visitor_b->p_object->current.pointed;
                    struct type  t2 = type_remove_pointer(p_visitor_b->p_type);
                    object_set_deleted(&t2, pointed);
                    type_destroy(&t2);
                }
            }
            else
            {
                p_visitor_a->p_object->current.state = p_visitor_a->p_object->current.state &  ~8;
                if (p_visitor_b->p_object->current.state & 4)
                {
                    p_visitor_b->p_object->current.state =  ~4;
                    p_visitor_b->p_object->current.state = 8;
                }
            }
        }
        else
        {
            if ( !a_type_is_view && type_is_obj_owner(p_visitor_a->p_type))
            {
                if (type_is_any_owner(p_visitor_b->p_type))
                {
                    if (assigment_type == 1)
                    {
                        if (p_visitor_b->p_object->current.pointed)
                        {
                            struct flow_object * pointed = p_visitor_b->p_object->current.pointed;
                            struct type  t2 = type_remove_pointer(p_visitor_b->p_type);
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
                                struct flow_object * pointed = p_visitor_b->p_object->current.pointed;
                                struct type  t2 = type_remove_pointer(p_visitor_b->p_type);
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
                if (a_type_is_view ||  !type_is_owner(p_visitor_a->p_type))
                {
                    p_visitor_a->p_object->current.state = p_visitor_b->p_object->current.state;
                }
                if (assigment_type == 1)
                {
                    struct type  t3 = type_remove_pointer(p_visitor_a->p_type);
                    if ( !type_is_const(&t3))
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
        struct struct_or_union_specifier * p_a_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_visitor_a->p_type->struct_or_union_specifier);
        if (p_visitor_b->p_type->struct_or_union_specifier == ((void *)0))
        {
            return;
        }
        struct struct_or_union_specifier * p_b_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_visitor_b->p_type->struct_or_union_specifier);
        if (p_a_struct_or_union_specifier && p_b_struct_or_union_specifier)
        {
            struct member_declaration * p_a_member_declaration = p_a_struct_or_union_specifier->member_declaration_list.head;
            struct member_declaration * p_b_member_declaration = p_b_struct_or_union_specifier->member_declaration_list.head;
            while (p_a_member_declaration && p_b_member_declaration)
            {
                if (p_a_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator * p_a_member_declarator = p_a_member_declaration->member_declarator_list_opt->head;
                    struct member_declarator * p_b_member_declarator = p_b_member_declaration->member_declarator_list_opt->head;
                    while (p_a_member_declarator && p_b_member_declarator)
                    {
                        if (p_a_member_declarator->declarator && p_b_member_declarator->declarator)
                        {
                            if (p_visitor_a->member_index < p_visitor_a->p_object->members.size && p_visitor_b->member_index < p_visitor_b->p_object->members.size)
                            {
                                struct object_visitor  visitor_a = {0};
                                visitor_a.p_type = &p_a_member_declarator->declarator->type;
                                visitor_a.p_object = p_visitor_a->p_object->members.data[p_visitor_a->member_index];
                                struct object_visitor  visitor_b = {0};
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
                        struct type  t = {0};
                        t.category = 0;
                        t.struct_or_union_specifier = p_a_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                        t.type_specifier_flags = 32768;
                        struct type * temp1 = p_visitor_a->p_type;
                        struct type * temp2 = p_visitor_b->p_type;
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
    if ( !a_type_is_view && type_is_owner(p_visitor_a->p_type))
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
    if (p_expression == ((void *)0))
    {
        return ((void *)0);
    }
    if (1)
    {
        if (p_expression->expression_type == 2)
        {
            ;
            if (p_expression->declarator->declaration_specifiers &&  !(p_expression->declarator->declaration_specifiers->storage_class_specifier_flags & 4096))
            {
                ;
                if (flow_objects_find(&ctx->arena, p_expression->declarator->p_object) == ((void *)0))
                {
                    p_expression->declarator->p_object = make_flow_object(ctx, &p_expression->declarator->type, p_expression->declarator, ((void *)0));
                    flow_object_set_unknown(&p_expression->declarator->type, type_is_nullable(&p_expression->declarator->type, ctx->ctx->options.null_checks_enabled), p_expression->declarator->p_object, ctx->ctx->options.null_checks_enabled);
                }
            }
            return p_expression->declarator->p_object;
        }
        else
        {
            if (p_expression->expression_type == 33)
            {
                ;
                struct flow_object * p_object = make_flow_object(ctx, &p_expression->type, ((void *)0), p_expression);
                if (p_object == ((void *)0))
                {
                    goto _catch_label_1;
                }
                struct flow_object * p_object_pointed = expression_get_flow_object(ctx, p_expression->right, nullable_enabled);
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
                        ;
                        struct flow_object * p = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
                        if (p)
                        {
                            if (type_is_pointer(&p_expression->type_name->type))
                            {
                                if (p->current.state & 32)
                                {
                                    p->current.state =  ~32;
                                    p->current.state = 2;
                                }
                                if (p->current.state & 64)
                                {
                                    p->current.state =  ~64;
                                    p->current.state = 4;
                                }
                            }
                        }
                        return p;
                    }
                    else
                    {
                        if (p_expression->expression_type == 14)
                        {
                            ;
                            struct flow_object * p_obj = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
                            if (p_obj)
                            {
                                if (p_expression->member_index < p_obj->members.size)
                                {
                                    return p_obj->members.data[p_expression->member_index];
                                }
                                else
                                {
                                    return ((void *)0);
                                }
                            }
                        }
                        else
                        {
                            if (p_expression->expression_type == 13)
                            {
                                ;
                                struct flow_object * p_obj = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
                                if (p_obj)
                                {
                                    if (p_obj->current.pointed == ((void *)0))
                                    {
                                        flow_object_expand_pointer(ctx, &p_expression->left->type, p_obj);
                                    }
                                    if (p_obj->current.pointed)
                                    {
                                        struct flow_object * pointed = p_obj->current.pointed;
                                        return pointed;
                                    }
                                }
                                return ((void *)0);
                            }
                            else
                            {
                                if (p_expression->expression_type == 15)
                                {
                                    ;
                                    struct flow_object * p_obj = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
                                    if (p_obj)
                                    {
                                        if (p_obj->current.pointed == 0)
                                        {
                                            flow_object_expand_pointer(ctx, &p_expression->left->type, p_obj);
                                        }
                                        struct flow_object * pointed = p_obj->current.pointed;
                                        if (pointed == ((void *)0) || p_expression->member_index >= pointed->members.size)
                                        {
                                            return ((void *)0);
                                        }
                                        struct flow_object * p_obj2 = pointed->members.data[p_expression->member_index];
                                        p_obj2->p_declarator_origin = ((void *)0);
                                        p_obj2->p_expression_origin = p_expression;
                                        return p_obj2;
                                    }
                                    return ((void *)0);
                                }
                                else
                                {
                                    if (p_expression->expression_type == 32)
                                    {
                                        ;
                                        struct flow_object * p_obj = expression_get_flow_object(ctx, p_expression->right, nullable_enabled);
                                        if (p_obj)
                                        {
                                            if (p_obj->current.pointed == ((void *)0))
                                            {
                                                flow_object_expand_pointer(ctx, &p_expression->right->type, p_obj);
                                            }
                                            if (p_obj->current.pointed != ((void *)0))
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
                                            struct flow_object * p_object = make_flow_object(ctx, &p_expression->type, ((void *)0), p_expression);
                                            if (p_object == ((void *)0))
                                            {
                                                goto _catch_label_1;
                                            }
                                            unsigned char   is_nullable = type_is_nullable(&p_expression->type, nullable_enabled);
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
                                                return p_expression->type_name->abstract_declarator->p_object;
                                            }
                                            else
                                            {
                                                if (p_expression->expression_type == 3)
                                                {
                                                    struct flow_object * p_object = make_flow_object(ctx, &p_expression->type, ((void *)0), p_expression);
                                                    if (p_object == ((void *)0))
                                                    {
                                                        goto _catch_label_1;
                                                    }
                                                    p_object->current.state = 4;
                                                    return p_object;
                                                }
                                                else
                                                {
                                                    if (p_expression->expression_type == 6)
                                                    {
                                                        struct flow_object * p_object = make_flow_object(ctx, &p_expression->type, ((void *)0), p_expression);
                                                        if (p_object == ((void *)0))
                                                        {
                                                            goto _catch_label_1;
                                                        }
                                                        if (p_expression->type.type_specifier_flags == 16777216)
                                                        {
                                                            p_object->current.state = 2;
                                                        }
                                                        else
                                                        {
                                                            if (object_has_constant_value(&p_expression->object))
                                                            {
                                                                unsigned char   not_zero = object_to_bool(&p_expression->object);
                                                                p_object->current.state = not_zero ? 64 : 32;
                                                            }
                                                        }
                                                        return p_object;
                                                    }
                                                    else
                                                    {
                                                        if (p_expression->expression_type == 53)
                                                        {
                                                            ;
                                                            struct flow_object * p_obj = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
                                                            return p_obj;
                                                        }
                                                        else
                                                        {
                                                            if (p_expression->expression_type == 55)
                                                            {
                                                                ;
                                                                ;
                                                                struct flow_object * p_object = make_flow_object(ctx, &p_expression->type, ((void *)0), p_expression);
                                                                if (p_object == ((void *)0))
                                                                {
                                                                    goto _catch_label_1;
                                                                }
                                                                struct flow_object * p_obj1 = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
                                                                struct flow_object * p_obj2 = expression_get_flow_object(ctx, p_expression->right, nullable_enabled);
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
                                                                    struct flow_object * p_object = make_flow_object(ctx, &p_expression->type, ((void *)0), p_expression);
                                                                    if (p_object == ((void *)0))
                                                                    {
                                                                        goto _catch_label_1;
                                                                    }
                                                                    if (object_has_constant_value(&p_expression->object))
                                                                    {
                                                                        unsigned char   not_zero = object_to_bool(&p_expression->object);
                                                                        p_object->current.state = not_zero ? 64 : 32;
                                                                    }
                                                                    else
                                                                    {
                                                                        p_object->current.state = 64 | 32;
                                                                    }
                                                                    return p_object;
                                                                }
                                                                else
                                                                {
                                                                    if (p_expression->expression_type == 38)
                                                                    {
                                                                        struct flow_object * p_object = make_flow_object(ctx, &p_expression->type, ((void *)0), p_expression);
                                                                        if (p_object == ((void *)0))
                                                                        {
                                                                            goto _catch_label_1;
                                                                        }
                                                                        if (type_is_pointer(&p_expression->type))
                                                                        {
                                                                            p_object->current.state = 4;
                                                                        }
                                                                        else
                                                                        {
                                                                            if (object_has_constant_value(&p_expression->object))
                                                                            {
                                                                                unsigned char   not_zero = object_to_bool(&p_expression->object);
                                                                                p_object->current.state = not_zero ? 4 : 2;
                                                                            }
                                                                            else
                                                                            {
                                                                                p_object->current.state = 4 | 2;
                                                                            }
                                                                        }
                                                                        return p_object;
                                                                    }
                                                                    else
                                                                    {
                                                                        if (p_expression->expression_type == 30 || p_expression->expression_type == 31)
                                                                        {
                                                                            ;
                                                                            struct flow_object * p_obj_right = expression_get_flow_object(ctx, p_expression->right, nullable_enabled);
                                                                            struct flow_object * p_object = make_flow_object(ctx, &p_expression->type, ((void *)0), p_expression);
                                                                            if (p_object == ((void *)0))
                                                                            {
                                                                                goto _catch_label_1;
                                                                            }
                                                                            if (p_obj_right)
                                                                            {
                                                                                p_object->current.state = p_obj_right->current.state;
                                                                            }
                                                                            return p_object;
                                                                        }
                                                                        else
                                                                        {
                                                                            struct flow_object * p_object = make_flow_object(ctx, &p_expression->type, ((void *)0), p_expression);
                                                                            if (p_object == ((void *)0))
                                                                            {
                                                                                goto _catch_label_1;
                                                                            }
                                                                            if (type_is_pointer(&p_expression->type))
                                                                            {
                                                                                if (object_has_constant_value(&p_expression->object))
                                                                                {
                                                                                    unsigned char   not_zero = object_to_bool(&p_expression->object);
                                                                                    p_object->current.state = not_zero ? 4 : 2;
                                                                                }
                                                                                else
                                                                                {
                                                                                    p_object->current.state = 4 | 2;
                                                                                }
                                                                            }
                                                                            else
                                                                            {
                                                                                if (object_has_constant_value(&p_expression->object))
                                                                                {
                                                                                    unsigned char   not_zero = object_to_bool(&p_expression->object);
                                                                                    p_object->current.state = not_zero ? 64 : 32;
                                                                                }
                                                                                else
                                                                                {
                                                                                    p_object->current.state = 64 | 32;
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
    else _catch_label_1:
    {
    }
    return ((void *)0);
}

void flow_check_assignment(struct flow_visit_ctx * ctx, struct token * error_position, struct marker * p_a_marker, struct marker * p_b_marker, int   assigment_type, unsigned char   check_uninitialized_b, unsigned char   a_type_is_view, unsigned char   a_type_is_nullable, struct type * p_a_type, struct flow_object * p_a_object, struct type * p_b_type, struct flow_object * p_b_object, unsigned char  * set_argument_to_unkown)
{
    if (type_is_pointer(p_b_type) && flow_object_is_expansible(p_b_object))
    {
    }
    struct object_visitor  visitor_a = {0, 0, 0};
    visitor_a.p_type = p_a_type;
    visitor_a.p_object = p_a_object;
    struct object_visitor  visitor_b = {0, 0, 0};
    visitor_b.p_type = p_b_type;
    visitor_b.p_object = p_b_object;
    flow_assignment_core(ctx, error_position, p_a_marker, p_b_marker, assigment_type, check_uninitialized_b, a_type_is_view, a_type_is_nullable, &visitor_a, &visitor_b, set_argument_to_unkown);
}

int ss_fprintf(struct osstream * stream, char * fmt, ...);
void ss_close(struct osstream * stream);

void print_object_state_to_str(int   e, char str[], int sz)
{
    unsigned char   first = 1;
    struct osstream  ss = {0};
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
    struct osstream  ss = {0};
    char temp[200] = {0};
    print_object_state_to_str(p_state->state, temp, sizeof temp);
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
        int i = 0;
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
    struct osstream  ss = {0};
    if (p_object->parent)
    {
        ss_fprintf(&ss, "\xe2\x86\x91%d", p_object->parent->id);
        printf("\xe2\x94\x82%-2d\xe2\x94\x82", p_object->id);
        printf("%-20s\xe2\x94\x82", ss.c_str);
    }
    else
    {
        int line = 0;
        int col = 0;
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
    int cols = 0;
    struct flow_object_state * p_state = &p_object->current;
    while (p_state)
    {
        cols++;
        flow_object_state_print(p_state);
        p_state = p_state->next;
    }
    {
        int i = 0;
        for (; i <= extra_cols - cols; i++)
        {
            printf("%-25s\xe2\x94\x82", " ");
        }
    }
    printf("\n");
}

