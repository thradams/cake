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

struct attribute_specifier_sequence {
    struct token * first_token;
    struct token * last_token;
    int attributes_flags;
    struct attribute_specifier * head;
    struct attribute_specifier * tail;
};

struct statement {
    struct labeled_statement * labeled_statement;
    struct unlabeled_statement * unlabeled_statement;
};

struct declaration_specifier {
    struct storage_class_specifier * storage_class_specifier;
    struct type_specifier_qualifier * type_specifier_qualifier;
    struct function_specifier * function_specifier;
    struct declaration_specifier * next;
};

struct pointer {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * calling_convention;
    struct pointer * pointer;
};

struct block_item_list {
    struct block_item * head;
    struct block_item * tail;
};

struct type_qualifier {
    int flags;
    struct token * token;
    struct type_qualifier * next;
};

struct block_item {
    struct token * first_token;
    struct declaration * declaration;
    struct unlabeled_statement * unlabeled_statement;
    struct label * label;
    struct block_item * next;
};

struct scope_list {
    struct scope * head;
    struct scope * tail;
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

struct attribute_specifier {
    struct token * first_token;
    struct token * last_token;
    struct attribute_list * attribute_list;
    struct attribute_specifier * next;
};

struct param_list {
    unsigned char   is_var_args;
    unsigned char   is_void;
    struct param * head;
    struct param * tail;
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

union _struct_tag_10 {
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
    int state;
    int value_type;
    struct type  type;
    char * debug_name;
    union _struct_tag_10  value;
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

struct type_specifier_qualifier {
    struct type_specifier * type_specifier;
    struct type_qualifier * type_qualifier;
    struct alignment_specifier * alignment_specifier;
    struct type_specifier_qualifier * next;
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

struct label_list_item {
    struct token * p_last_usage;
    struct token * p_defined;
    struct label_list_item * next;
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

struct type_specifier {
    int flags;
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

struct balanced_token_sequence {
    struct balanced_token * head;
    struct balanced_token * tail;
};

struct balanced_token {
    struct token * token;
    struct balanced_token * next;
};

struct expression_statement {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct expression * expression_opt;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct label_list {
    struct label_list_item * head;
    struct label_list_item * tail;
};

struct typeof_specifier_argument {
    struct expression * expression;
    struct type_name * type_name;
};

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

struct labeled_statement {
    struct label * label;
    struct statement * statement;
};

struct typeof_specifier {
    struct token * first_token;
    struct token * last_token;
    struct typeof_specifier_argument * typeof_specifier_argument;
    struct type  type;
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

struct generic_association {
    struct type  type;
    struct type_name * p_type_name;
    struct expression * expression;
    struct token * first_token;
    struct token * last_token;
    struct generic_association * next;
};

struct designation {
    struct designator_list * designator_list;
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

struct type_qualifier_list {
    int flags;
    struct type_qualifier * head;
    struct type_qualifier * tail;
};

struct compound_statement {
    struct token * first_token;
    struct token * last_token;
    struct block_item_list  block_item_list;
    struct diagnostic  diagnostic_flags;
    struct defer_list  defer_list;
};

struct case_label_list {
    struct label * head;
    struct label * tail;
};

struct braced_initializer {
    struct token * first_token;
    struct token * last_token;
    struct initializer_list * initializer_list;
};

struct objects {
    struct object ** items;
    int size;
    int capacity;
};

struct pragma_declaration {
    struct token * first_token;
    struct token * last_token;
};

struct member_declarator_list {
    struct member_declarator * head;
    struct member_declarator * tail;
};

struct generic_assoc_list {
    struct generic_association * head;
    struct generic_association * tail;
};

union _struct_tag_11 {
    unsigned int number;
    struct enum_specifier * p_enum_specifier;
    struct enumerator * p_enumerator;
    struct struct_or_union_specifier * p_struct_or_union_specifier;
    struct declarator * p_declarator;
    struct init_declarator * p_init_declarator;
    struct macro * p_macro;
    struct struct_entry * p_struct_entry;
};

struct designator {
    struct expression * constant_expression_opt;
    struct token * token;
    struct designator * next;
};

struct jump_statement {
    struct token * label;
    struct token * first_token;
    struct token * last_token;
    struct expression * expression_opt;
    int label_id;
    struct defer_list  defer_list;
};

struct primary_block {
    struct compound_statement * compound_statement;
    struct selection_statement * selection_statement;
    struct iteration_statement * iteration_statement;
    struct defer_statement * defer_statement;
    struct try_statement * try_statement;
};

struct parameter_declaration {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct declaration_specifiers * declaration_specifiers;
    struct declarator * declarator;
    struct parameter_declaration * next;
};

struct simple_declaration {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator_list  init_declarator_list;
    struct token * first_token;
    struct token * last_token;
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

struct parameter_type_list {
    unsigned char   is_var_args;
    unsigned char   is_void;
    struct parameter_list * parameter_list;
};

struct init_statement {
    struct expression_statement * p_expression_statement;
    struct simple_declaration * p_simple_declaration;
};

struct alignment_specifier {
    struct type_name * type_name;
    struct expression * constant_expression;
    struct token * token;
};

struct secondary_block {
    struct token * first_token;
    struct token * last_token;
    struct statement * statement;
};

struct type_name {
    struct token * first_token;
    struct token * last_token;
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct declarator * abstract_declarator;
    struct type  type;
};

struct atomic_type_specifier {
    struct token * token;
    struct type_name * type_name;
};

struct array_declarator {
    struct direct_declarator * direct_declarator;
    struct expression * assignment_expression;
    struct expression * expression;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * token;
    struct token * static_token_opt;
};

struct attribute_argument_clause {
    struct balanced_token_sequence * p_balanced_token_sequence;
    struct token * token;
};

struct map_entry {
    struct map_entry * next;
    unsigned int hash;
    char * key;
    int type;
    union _struct_tag_11  data;
};

struct label {
    struct expression * constant_expression;
    struct expression * constant_expression_end;
    struct token * p_identifier_opt;
    struct token * p_first_token;
    struct label * next;
    int label_id;
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
    int flags;
    struct token * token;
};

struct member_declaration {
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct member_declarator_list * member_declarator_list_opt;
    struct static_assert_declaration * static_assert_declaration;
    struct pragma_declaration * pragma_declaration;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct member_declaration * next;
};

struct member_declarator {
    struct declarator * declarator;
    struct expression * constant_expression;
    struct member_declarator * next;
};

struct _iobuf {
    void * _Placeholder;
};

struct attribute {
    int attributes_flags;
    struct attribute_token * attribute_token;
    struct attribute_argument_clause * attribute_argument_clause;
    struct attribute * next;
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

struct token_list {
    struct token * head;
    struct token * tail;
};

struct direct_declarator {
    struct token * name_opt;
    struct token * p_calling_convention;
    struct declarator * declarator;
    struct array_declarator * array_declarator;
    struct function_declarator * function_declarator;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
};

struct defer_statement {
    struct token * first_token;
    struct token * last_token;
    struct secondary_block * secondary_block;
};

struct attribute_list {
    int attributes_flags;
    struct attribute * head;
    struct attribute * tail;
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

struct storage_class_specifier {
    int flags;
    struct token * token;
};

struct defer_list_item {
    struct declarator * declarator;
    struct defer_statement * defer_statement;
    struct defer_list_item * next;
};

struct argument_expression {
    struct expression * expression;
    struct argument_expression * next;
    unsigned char   set_unkown;
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

struct try_statement {
    struct secondary_block * secondary_block;
    struct secondary_block * catch_secondary_block_opt;
    struct token * first_token;
    struct token * last_token;
    struct token * catch_token_opt;
    int catch_label_id;
};

struct param {
    struct type  type;
    struct param * next;
};

struct condition {
    struct expression * expression;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator * p_init_declarator;
    struct token * first_token;
    struct token * last_token;
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

struct function_declarator {
    struct direct_declarator * direct_declarator;
    struct scope  parameters_scope;
    struct parameter_type_list * parameter_type_list_opt;
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

struct initializer_list {
    struct token * first_token;
    struct initializer_list_item * head;
    struct initializer_list_item * tail;
    int size;
};

struct static_assert_declaration {
    struct token * first_token;
    struct token * last_token;
    struct expression * constant_expression;
    struct token * string_literal_opt;
};

struct attribute_token {
    int attributes_flags;
    struct token * token;
};

struct initializer_list_item {
    struct designation * designation;
    struct initializer * initializer;
    struct initializer_list_item * next;
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

struct enumerator {
    unsigned char   has_shared_ownership;
    struct token * token;
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct expression * constant_expression_opt;
    struct enum_specifier * enum_specifier;
    struct enumerator * next;
    struct object  value;
};

struct unlabeled_statement {
    struct expression_statement * expression_statement;
    struct primary_block * primary_block;
    struct jump_statement * jump_statement;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

int _Avx2WmemEnabledWeakValue = 0;
unsigned char  unsigned_long_long_sub(unsigned long long * result, unsigned long long a, unsigned long long b)
{
    *result = 0;
    if (a < b)
    {
        return 0;
    }
    *result = a - b;
    return 1;
}

unsigned char  unsigned_long_long_mul(unsigned long long * result, unsigned long long a, unsigned long long b)
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

unsigned char  unsigned_long_long_add(unsigned long long * result, unsigned long long a, unsigned long long b)
{
    *result = 0;
    if (a > 18446744073709551615ULL - b)
    {
        return 0;
    }
    *result = a + b;
    return 1;
}

unsigned char  signed_long_long_sub(signed long long * result, signed long long a, signed long long b)
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

unsigned char  signed_long_long_add(signed long long * result, signed long long a, signed long long b)
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

unsigned char  signed_long_long_mul(signed long long * result, signed long long a, signed long long b)
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
    free((void *)p->debug_name);
    item = p->members;
    while (item)
    {
        struct object * next;

        next = item->next;
        item->next = 0U;
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

unsigned char  object_has_constant_value(struct object * a)
{
    a = object_get_referenced(a);
    return !!(a->state == 2);
}

int snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...);

void object_to_string(struct object * a, char buffer[], int sz)
{
    a = object_get_referenced(a);
    buffer[0] = 0;
    /*switch*/
    {
        register int _R0 = a->value_type;
        if (_R0 == 2) goto _CKL1; /*case 2*/
        if (_R0 == 3) goto _CKL2; /*case 3*/
        if (_R0 == 4) goto _CKL3; /*case 4*/
        if (_R0 == 5) goto _CKL4; /*case 5*/
        if (_R0 == 6) goto _CKL5; /*case 6*/
        if (_R0 == 0) goto _CKL6; /*case 0*/
        if (_R0 == 1) goto _CKL7; /*case 1*/
        if (_R0 == 7) goto _CKL8; /*case 7*/
        if (_R0 == 8) goto _CKL9; /*case 8*/
        if (_R0 == 9) goto _CKL10; /*case 9*/
        if (_R0 == 10) goto _CKL11; /*case 10*/
        if (_R0 == 11) goto _CKL12; /*case 11*/
        if (_R0 == 12) goto _CKL13; /*case 12*/
        if (_R0 == 13) goto _CKL14; /*case 13*/
        if (_R0 == 14) goto _CKL15; /*case 14*/
        goto _CKL0;

        {
            /*case 2*/ _CKL1:
            snprintf(buffer, sz, "%s", a->value.signed_char_value ? "true" : "false");
            /*break*/ goto _CKL0;

            /*case 3*/ _CKL2:
            snprintf(buffer, sz, "%c", a->value.signed_char_value);
            /*break*/ goto _CKL0;

            /*case 4*/ _CKL3:
            snprintf(buffer, sz, "%c", a->value.unsigned_char_value);
            /*break*/ goto _CKL0;

            /*case 5*/ _CKL4:
            snprintf(buffer, sz, "%c", a->value.signed_short_value);
            /*break*/ goto _CKL0;

            /*case 6*/ _CKL5:
            snprintf(buffer, sz, "%c", a->value.signed_short_value);
            /*break*/ goto _CKL0;

            /*case 0*/ _CKL6:
            snprintf(buffer, sz, "%d", a->value.signed_int_value);
            /*break*/ goto _CKL0;

            /*case 1*/ _CKL7:
            snprintf(buffer, sz, "%u", a->value.signed_int_value);
            /*break*/ goto _CKL0;

            /*case 7*/ _CKL8:
            snprintf(buffer, sz, "%ld", a->value.signed_long_value);
            /*break*/ goto _CKL0;

            /*case 8*/ _CKL9:
            /*break*/ goto _CKL0;

            /*case 9*/ _CKL10:
            snprintf(buffer, sz, "%lud", a->value.signed_long_value);
            /*break*/ goto _CKL0;

            /*case 10*/ _CKL11:
            snprintf(buffer, sz, "%llu", a->value.signed_long_long_value);
            /*break*/ goto _CKL0;

            /*case 11*/ _CKL12:
            snprintf(buffer, sz, "%f", a->value.float_value);
            /*break*/ goto _CKL0;

            /*case 12*/ _CKL13:
            snprintf(buffer, sz, "%f", a->value.double_value);
            /*break*/ goto _CKL0;

            /*case 13*/ _CKL14:
            snprintf(buffer, sz, "%Lf", a->value.long_double_value);
            /*break*/ goto _CKL0;

            /*case 14*/ _CKL15:
            if (a->value.void_pointer == 0U)
            {
                snprintf(buffer, sz, "null");
            }
            else
            {
                snprintf(buffer, sz, "%p", a->value.void_pointer);
            }
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
}

struct object object_make_size_t(unsigned int value)
{
    struct object  r;

    _cake_zmem(&r, 96);
    r.state = 2;
    r.value_type = 1;
    r.value.unsigned_int_value = value;
    return r;
}

struct object object_make_nullptr()
{
    struct object  r;

    _cake_zmem(&r, 96);
    r.state = 2;
    r.value_type = 0;
    r.value.signed_short_value = 0;
    return r;
}

struct object object_make_wchar_t(unsigned short value)
{
    struct object  r;

    _cake_zmem(&r, 96);
    r.state = 2;
    r.value_type = 5;
    r.value.signed_short_value = value;
    return r;
}

struct object object_make_bool(unsigned char   value)
{
    struct object  r;

    _cake_zmem(&r, 96);
    r.state = 2;
    r.value_type = 2;
    r.value.bool_value = value;
    return r;
}

signed long long object_to_signed_long_long(struct object * a);
unsigned long long object_to_unsigned_long_long(struct object * a);
long double object_to_long_double(struct object * a);

int object_to_str(struct object * a, int n, char str[])
{
    str[0] = 0;
    a = object_get_referenced(a);
    /*switch*/
    {
        register int _R1 = a->value_type;
        if (_R1 == 2) goto _CKL1; /*case 2*/
        if (_R1 == 3) goto _CKL2; /*case 3*/
        if (_R1 == 5) goto _CKL3; /*case 5*/
        if (_R1 == 0) goto _CKL4; /*case 0*/
        if (_R1 == 7) goto _CKL5; /*case 7*/
        if (_R1 == 9) goto _CKL6; /*case 9*/
        if (_R1 == 4) goto _CKL7; /*case 4*/
        if (_R1 == 6) goto _CKL8; /*case 6*/
        if (_R1 == 1) goto _CKL9; /*case 1*/
        if (_R1 == 8) goto _CKL10; /*case 8*/
        if (_R1 == 10) goto _CKL11; /*case 10*/
        if (_R1 == 11) goto _CKL12; /*case 11*/
        if (_R1 == 12) goto _CKL13; /*case 12*/
        if (_R1 == 13) goto _CKL14; /*case 13*/
        goto _CKL0;

        {
            /*case 2*/ _CKL1:
            /*case 3*/ _CKL2:
            /*case 5*/ _CKL3:
            /*case 0*/ _CKL4:
            /*case 7*/ _CKL5:
            /*case 9*/ _CKL6:
            {
                long long v;

                v = object_to_signed_long_long(a);
                snprintf(str, n, "%lld", v);
            }
            /*break*/ goto _CKL0;

            /*case 4*/ _CKL7:
            /*case 6*/ _CKL8:
            /*case 1*/ _CKL9:
            /*case 8*/ _CKL10:
            /*case 10*/ _CKL11:
            {
                unsigned long long v;

                v = object_to_unsigned_long_long(a);
                snprintf(str, n, "%llu", v);
            }
            /*break*/ goto _CKL0;

            /*case 11*/ _CKL12:
            return a->value.float_value;
            /*case 12*/ _CKL13:
            return a->value.double_value;
            /*case 13*/ _CKL14:
            return a->value.long_double_value;
            {
                long double v;

                v = object_to_long_double(a);
                snprintf(str, n, "%Lf", v);
            }
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    return 0;
}

struct object *object_get_non_const_referenced(struct object * p_object);

void object_set_signed_int(struct object * a, long long value)
{
    a = object_get_non_const_referenced(a);
    a->state = 4;
    /*switch*/
    {
        register int _R2 = a->value_type;
        if (_R2 == 2) goto _CKL1; /*case 2*/
        if (_R2 == 3) goto _CKL2; /*case 3*/
        if (_R2 == 4) goto _CKL3; /*case 4*/
        if (_R2 == 5) goto _CKL4; /*case 5*/
        if (_R2 == 6) goto _CKL5; /*case 6*/
        if (_R2 == 0) goto _CKL6; /*case 0*/
        if (_R2 == 1) goto _CKL7; /*case 1*/
        if (_R2 == 7) goto _CKL8; /*case 7*/
        if (_R2 == 8) goto _CKL9; /*case 8*/
        if (_R2 == 9) goto _CKL10; /*case 9*/
        if (_R2 == 10) goto _CKL11; /*case 10*/
        if (_R2 == 11) goto _CKL12; /*case 11*/
        if (_R2 == 12) goto _CKL13; /*case 12*/
        if (_R2 == 13) goto _CKL14; /*case 13*/
        if (_R2 == 14) goto _CKL15; /*case 14*/
        goto _CKL0;

        {
            /*case 2*/ _CKL1:
            a->value.bool_value = !!(value);
            /*break*/ goto _CKL0;

            /*case 3*/ _CKL2:
            a->value.signed_char_value = value;
            /*break*/ goto _CKL0;

            /*case 4*/ _CKL3:
            a->value.unsigned_char_value = value;
            /*break*/ goto _CKL0;

            /*case 5*/ _CKL4:
            a->value.signed_short_value = value;
            /*break*/ goto _CKL0;

            /*case 6*/ _CKL5:
            a->value.unsigned_short_value = value;
            /*break*/ goto _CKL0;

            /*case 0*/ _CKL6:
            a->value.signed_int_value = value;
            /*break*/ goto _CKL0;

            /*case 1*/ _CKL7:
            a->value.unsigned_int_value = value;
            /*break*/ goto _CKL0;

            /*case 7*/ _CKL8:
            a->value.signed_long_value = value;
            /*break*/ goto _CKL0;

            /*case 8*/ _CKL9:
            a->value.unsigned_long_value = value;
            /*break*/ goto _CKL0;

            /*case 9*/ _CKL10:
            a->value.signed_long_long_value = value;
            /*break*/ goto _CKL0;

            /*case 10*/ _CKL11:
            a->value.unsigned_long_long_value = value;
            /*break*/ goto _CKL0;

            /*case 11*/ _CKL12:
            a->value.float_value = value;
            /*break*/ goto _CKL0;

            /*case 12*/ _CKL13:
            a->value.double_value = value;
            /*break*/ goto _CKL0;

            /*case 13*/ _CKL14:
            a->value.long_double_value = value;
            /*break*/ goto _CKL0;

            /*case 14*/ _CKL15:
            ;
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
}

void object_set_unsigned_int(struct object * a, unsigned long long value)
{
    a = object_get_non_const_referenced(a);
    a->state = 4;
    /*switch*/
    {
        register int _R3 = a->value_type;
        if (_R3 == 2) goto _CKL1; /*case 2*/
        if (_R3 == 3) goto _CKL2; /*case 3*/
        if (_R3 == 4) goto _CKL3; /*case 4*/
        if (_R3 == 5) goto _CKL4; /*case 5*/
        if (_R3 == 6) goto _CKL5; /*case 6*/
        if (_R3 == 0) goto _CKL6; /*case 0*/
        if (_R3 == 1) goto _CKL7; /*case 1*/
        if (_R3 == 7) goto _CKL8; /*case 7*/
        if (_R3 == 8) goto _CKL9; /*case 8*/
        if (_R3 == 9) goto _CKL10; /*case 9*/
        if (_R3 == 10) goto _CKL11; /*case 10*/
        if (_R3 == 11) goto _CKL12; /*case 11*/
        if (_R3 == 12) goto _CKL13; /*case 12*/
        if (_R3 == 13) goto _CKL14; /*case 13*/
        if (_R3 == 14) goto _CKL15; /*case 14*/
        goto _CKL0;

        {
            /*case 2*/ _CKL1:
            a->value.bool_value = !!(value);
            /*break*/ goto _CKL0;

            /*case 3*/ _CKL2:
            a->value.signed_char_value = value;
            /*break*/ goto _CKL0;

            /*case 4*/ _CKL3:
            a->value.unsigned_char_value = value;
            /*break*/ goto _CKL0;

            /*case 5*/ _CKL4:
            a->value.signed_short_value = value;
            /*break*/ goto _CKL0;

            /*case 6*/ _CKL5:
            a->value.unsigned_short_value = value;
            /*break*/ goto _CKL0;

            /*case 0*/ _CKL6:
            a->value.signed_int_value = value;
            /*break*/ goto _CKL0;

            /*case 1*/ _CKL7:
            a->value.unsigned_int_value = value;
            /*break*/ goto _CKL0;

            /*case 7*/ _CKL8:
            a->value.signed_long_value = value;
            /*break*/ goto _CKL0;

            /*case 8*/ _CKL9:
            a->value.unsigned_long_value = value;
            /*break*/ goto _CKL0;

            /*case 9*/ _CKL10:
            a->value.signed_long_long_value = value;
            /*break*/ goto _CKL0;

            /*case 10*/ _CKL11:
            a->value.unsigned_long_long_value = value;
            /*break*/ goto _CKL0;

            /*case 11*/ _CKL12:
            a->value.float_value = value;
            /*break*/ goto _CKL0;

            /*case 12*/ _CKL13:
            a->value.double_value = value;
            /*break*/ goto _CKL0;

            /*case 13*/ _CKL14:
            a->value.long_double_value = value;
            /*break*/ goto _CKL0;

            /*case 14*/ _CKL15:
            ;
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
}

unsigned char  object_to_bool(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int _R4 = a->value_type;
        if (_R4 == 2) goto _CKL1; /*case 2*/
        if (_R4 == 3) goto _CKL2; /*case 3*/
        if (_R4 == 4) goto _CKL3; /*case 4*/
        if (_R4 == 5) goto _CKL4; /*case 5*/
        if (_R4 == 6) goto _CKL5; /*case 6*/
        if (_R4 == 0) goto _CKL6; /*case 0*/
        if (_R4 == 1) goto _CKL7; /*case 1*/
        if (_R4 == 7) goto _CKL8; /*case 7*/
        if (_R4 == 8) goto _CKL9; /*case 8*/
        if (_R4 == 9) goto _CKL10; /*case 9*/
        if (_R4 == 10) goto _CKL11; /*case 10*/
        if (_R4 == 11) goto _CKL12; /*case 11*/
        if (_R4 == 12) goto _CKL13; /*case 12*/
        if (_R4 == 13) goto _CKL14; /*case 13*/
        if (_R4 == 14) goto _CKL15; /*case 14*/
        goto _CKL0;

        {
            /*case 2*/ _CKL1:
            return a->value.bool_value;
            /*case 3*/ _CKL2:
            return !!(a->value.signed_char_value);
            /*case 4*/ _CKL3:
            return !!(a->value.unsigned_char_value);
            /*case 5*/ _CKL4:
            return !!(a->value.signed_short_value);
            /*case 6*/ _CKL5:
            return !!(a->value.unsigned_short_value);
            /*case 0*/ _CKL6:
            return !!(a->value.signed_int_value);
            /*case 1*/ _CKL7:
            return !!(a->value.unsigned_int_value);
            /*case 7*/ _CKL8:
            return !!(a->value.signed_long_value);
            /*case 8*/ _CKL9:
            return !!(a->value.unsigned_long_value);
            /*case 9*/ _CKL10:
            return !!(a->value.signed_long_long_value);
            /*case 10*/ _CKL11:
            return !!(a->value.unsigned_long_long_value);
            /*case 11*/ _CKL12:
            return !!(a->value.float_value);
            /*case 12*/ _CKL13:
            return !!(a->value.double_value);
            /*case 13*/ _CKL14:
            return !!(a->value.long_double_value);
            /*case 14*/ _CKL15:
            return !!(a->value.void_pointer != 0);
        }
        _CKL0:;
    }
    ;
    return 0;
}

struct object object_make_signed_char(signed char value)
{
    struct object  r;

    _cake_zmem(&r, 96);
    r.state = 2;
    r.value_type = 3;
    r.value.signed_char_value = value;
    return r;
}

int object_increment_value(struct object * a)
{
    a = object_get_non_const_referenced(a);
    /*switch*/
    {
        register int _R5 = a->value_type;
        if (_R5 == 2) goto _CKL1; /*case 2*/
        if (_R5 == 3) goto _CKL2; /*case 3*/
        if (_R5 == 4) goto _CKL3; /*case 4*/
        if (_R5 == 5) goto _CKL4; /*case 5*/
        if (_R5 == 6) goto _CKL5; /*case 6*/
        if (_R5 == 0) goto _CKL6; /*case 0*/
        if (_R5 == 1) goto _CKL7; /*case 1*/
        if (_R5 == 7) goto _CKL8; /*case 7*/
        if (_R5 == 8) goto _CKL9; /*case 8*/
        if (_R5 == 9) goto _CKL10; /*case 9*/
        if (_R5 == 10) goto _CKL11; /*case 10*/
        if (_R5 == 11) goto _CKL12; /*case 11*/
        if (_R5 == 12) goto _CKL13; /*case 12*/
        if (_R5 == 13) goto _CKL14; /*case 13*/
        if (_R5 == 14) goto _CKL15; /*case 14*/
        goto /*default*/ _CKL16;

        {
            /*case 2*/ _CKL1:
            a->value.bool_value++;
            /*break*/ goto _CKL0;

            /*case 3*/ _CKL2:
            a->value.signed_char_value++;
            /*break*/ goto _CKL0;

            /*case 4*/ _CKL3:
            a->value.unsigned_char_value++;
            /*break*/ goto _CKL0;

            /*case 5*/ _CKL4:
            a->value.signed_short_value++;
            /*break*/ goto _CKL0;

            /*case 6*/ _CKL5:
            a->value.unsigned_short_value++;
            /*break*/ goto _CKL0;

            /*case 0*/ _CKL6:
            a->value.signed_int_value++;
            /*break*/ goto _CKL0;

            /*case 1*/ _CKL7:
            a->value.unsigned_int_value++;
            /*break*/ goto _CKL0;

            /*case 7*/ _CKL8:
            a->value.signed_long_value++;
            /*break*/ goto _CKL0;

            /*case 8*/ _CKL9:
            a->value.unsigned_long_value++;
            /*break*/ goto _CKL0;

            /*case 9*/ _CKL10:
            a->value.signed_long_long_value++;
            /*break*/ goto _CKL0;

            /*case 10*/ _CKL11:
            a->value.unsigned_long_long_value++;
            /*break*/ goto _CKL0;

            /*case 11*/ _CKL12:
            a->value.float_value++;
            /*break*/ goto _CKL0;

            /*case 12*/ _CKL13:
            a->value.double_value++;
            /*break*/ goto _CKL0;

            /*case 13*/ _CKL14:
            a->value.long_double_value++;
            /*break*/ goto _CKL0;

            /*case 14*/ _CKL15:
            ;
            /*break*/ goto _CKL0;

            /*default*/ _CKL16:
            return 1;
        }
        _CKL0:;
    }
    return 0;
}

signed char object_to_signed_char(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int _R6 = a->value_type;
        if (_R6 == 2) goto _CKL1; /*case 2*/
        if (_R6 == 3) goto _CKL2; /*case 3*/
        if (_R6 == 4) goto _CKL3; /*case 4*/
        if (_R6 == 5) goto _CKL4; /*case 5*/
        if (_R6 == 6) goto _CKL5; /*case 6*/
        if (_R6 == 0) goto _CKL6; /*case 0*/
        if (_R6 == 1) goto _CKL7; /*case 1*/
        if (_R6 == 7) goto _CKL8; /*case 7*/
        if (_R6 == 8) goto _CKL9; /*case 8*/
        if (_R6 == 9) goto _CKL10; /*case 9*/
        if (_R6 == 10) goto _CKL11; /*case 10*/
        if (_R6 == 11) goto _CKL12; /*case 11*/
        if (_R6 == 12) goto _CKL13; /*case 12*/
        if (_R6 == 13) goto _CKL14; /*case 13*/
        if (_R6 == 14) goto _CKL15; /*case 14*/
        goto _CKL0;

        {
            /*case 2*/ _CKL1:
            return a->value.bool_value;
            /*case 3*/ _CKL2:
            return a->value.signed_char_value;
            /*case 4*/ _CKL3:
            return a->value.unsigned_char_value;
            /*case 5*/ _CKL4:
            return a->value.signed_short_value;
            /*case 6*/ _CKL5:
            return a->value.unsigned_short_value;
            /*case 0*/ _CKL6:
            return a->value.signed_int_value;
            /*case 1*/ _CKL7:
            return a->value.unsigned_int_value;
            /*case 7*/ _CKL8:
            return a->value.signed_long_value;
            /*case 8*/ _CKL9:
            return a->value.unsigned_long_value;
            /*case 9*/ _CKL10:
            return a->value.signed_long_long_value;
            /*case 10*/ _CKL11:
            return a->value.unsigned_long_long_value;
            /*case 11*/ _CKL12:
            return a->value.float_value;
            /*case 12*/ _CKL13:
            return a->value.double_value;
            /*case 13*/ _CKL14:
            return a->value.long_double_value;
            /*case 14*/ _CKL15:
            return 0;
        }
        _CKL0:;
    }
    ;
    return 0;
}

struct object object_make_unsigned_char(unsigned char value)
{
    struct object  r;

    _cake_zmem(&r, 96);
    r.state = 2;
    r.value_type = 4;
    r.value.unsigned_char_value = value;
    return r;
}

unsigned char object_to_unsigned_char(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int _R7 = a->value_type;
        if (_R7 == 2) goto _CKL1; /*case 2*/
        if (_R7 == 3) goto _CKL2; /*case 3*/
        if (_R7 == 4) goto _CKL3; /*case 4*/
        if (_R7 == 5) goto _CKL4; /*case 5*/
        if (_R7 == 6) goto _CKL5; /*case 6*/
        if (_R7 == 0) goto _CKL6; /*case 0*/
        if (_R7 == 1) goto _CKL7; /*case 1*/
        if (_R7 == 7) goto _CKL8; /*case 7*/
        if (_R7 == 8) goto _CKL9; /*case 8*/
        if (_R7 == 9) goto _CKL10; /*case 9*/
        if (_R7 == 10) goto _CKL11; /*case 10*/
        if (_R7 == 11) goto _CKL12; /*case 11*/
        if (_R7 == 12) goto _CKL13; /*case 12*/
        if (_R7 == 13) goto _CKL14; /*case 13*/
        goto _CKL0;

        {
            /*case 2*/ _CKL1:
            return a->value.bool_value;
            /*case 3*/ _CKL2:
            return a->value.signed_char_value;
            /*case 4*/ _CKL3:
            return a->value.unsigned_char_value;
            /*case 5*/ _CKL4:
            return a->value.signed_short_value;
            /*case 6*/ _CKL5:
            return a->value.unsigned_short_value;
            /*case 0*/ _CKL6:
            return a->value.signed_int_value;
            /*case 1*/ _CKL7:
            return a->value.unsigned_int_value;
            /*case 7*/ _CKL8:
            return a->value.signed_long_value;
            /*case 8*/ _CKL9:
            return a->value.unsigned_long_value;
            /*case 9*/ _CKL10:
            return a->value.signed_long_long_value;
            /*case 10*/ _CKL11:
            return a->value.unsigned_long_long_value;
            /*case 11*/ _CKL12:
            return a->value.float_value;
            /*case 12*/ _CKL13:
            return a->value.double_value;
            /*case 13*/ _CKL14:
            return a->value.long_double_value;
        }
        _CKL0:;
    }
    ;
    return 0;
}

struct object object_make_signed_short(signed short value)
{
    struct object  r;

    _cake_zmem(&r, 96);
    r.state = 2;
    r.value_type = 5;
    r.value.signed_short_value = value;
    return r;
}

signed short object_to_signed_short(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int _R8 = a->value_type;
        if (_R8 == 2) goto _CKL1; /*case 2*/
        if (_R8 == 3) goto _CKL2; /*case 3*/
        if (_R8 == 4) goto _CKL3; /*case 4*/
        if (_R8 == 5) goto _CKL4; /*case 5*/
        if (_R8 == 6) goto _CKL5; /*case 6*/
        if (_R8 == 0) goto _CKL6; /*case 0*/
        if (_R8 == 1) goto _CKL7; /*case 1*/
        if (_R8 == 7) goto _CKL8; /*case 7*/
        if (_R8 == 8) goto _CKL9; /*case 8*/
        if (_R8 == 9) goto _CKL10; /*case 9*/
        if (_R8 == 10) goto _CKL11; /*case 10*/
        if (_R8 == 11) goto _CKL12; /*case 11*/
        if (_R8 == 12) goto _CKL13; /*case 12*/
        if (_R8 == 13) goto _CKL14; /*case 13*/
        if (_R8 == 14) goto _CKL15; /*case 14*/
        goto _CKL0;

        {
            /*case 2*/ _CKL1:
            return a->value.bool_value;
            /*case 3*/ _CKL2:
            return a->value.signed_char_value;
            /*case 4*/ _CKL3:
            return a->value.unsigned_char_value;
            /*case 5*/ _CKL4:
            return a->value.signed_short_value;
            /*case 6*/ _CKL5:
            return a->value.unsigned_short_value;
            /*case 0*/ _CKL6:
            return a->value.signed_int_value;
            /*case 1*/ _CKL7:
            return a->value.unsigned_int_value;
            /*case 7*/ _CKL8:
            return a->value.signed_long_value;
            /*case 8*/ _CKL9:
            return a->value.unsigned_long_value;
            /*case 9*/ _CKL10:
            return a->value.signed_long_long_value;
            /*case 10*/ _CKL11:
            return a->value.unsigned_long_long_value;
            /*case 11*/ _CKL12:
            return a->value.float_value;
            /*case 12*/ _CKL13:
            return a->value.double_value;
            /*case 13*/ _CKL14:
            return a->value.long_double_value;
            /*case 14*/ _CKL15:
            ;
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    ;
    return 0;
}

struct object object_make_unsigned_short(unsigned short value)
{
    struct object  r;

    _cake_zmem(&r, 96);
    r.state = 2;
    r.value_type = 6;
    r.value.unsigned_short_value = value;
    return r;
}

unsigned short object_to_unsigned_short(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int _R9 = a->value_type;
        if (_R9 == 2) goto _CKL1; /*case 2*/
        if (_R9 == 3) goto _CKL2; /*case 3*/
        if (_R9 == 4) goto _CKL3; /*case 4*/
        if (_R9 == 5) goto _CKL4; /*case 5*/
        if (_R9 == 6) goto _CKL5; /*case 6*/
        if (_R9 == 0) goto _CKL6; /*case 0*/
        if (_R9 == 1) goto _CKL7; /*case 1*/
        if (_R9 == 7) goto _CKL8; /*case 7*/
        if (_R9 == 8) goto _CKL9; /*case 8*/
        if (_R9 == 9) goto _CKL10; /*case 9*/
        if (_R9 == 10) goto _CKL11; /*case 10*/
        if (_R9 == 11) goto _CKL12; /*case 11*/
        if (_R9 == 12) goto _CKL13; /*case 12*/
        if (_R9 == 13) goto _CKL14; /*case 13*/
        if (_R9 == 14) goto _CKL15; /*case 14*/
        goto _CKL0;

        {
            /*case 2*/ _CKL1:
            return a->value.bool_value;
            /*case 3*/ _CKL2:
            return a->value.signed_char_value;
            /*case 4*/ _CKL3:
            return a->value.unsigned_char_value;
            /*case 5*/ _CKL4:
            return a->value.signed_short_value;
            /*case 6*/ _CKL5:
            return a->value.unsigned_short_value;
            /*case 0*/ _CKL6:
            return a->value.signed_int_value;
            /*case 1*/ _CKL7:
            return a->value.unsigned_int_value;
            /*case 7*/ _CKL8:
            return a->value.signed_long_value;
            /*case 8*/ _CKL9:
            return a->value.unsigned_long_value;
            /*case 9*/ _CKL10:
            return a->value.signed_long_long_value;
            /*case 10*/ _CKL11:
            return a->value.unsigned_long_long_value;
            /*case 11*/ _CKL12:
            return a->value.float_value;
            /*case 12*/ _CKL13:
            return a->value.double_value;
            /*case 13*/ _CKL14:
            return a->value.long_double_value;
            /*case 14*/ _CKL15:
            ;
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    ;
    return 0;
}

struct object object_make_signed_int(signed int value)
{
    struct object  r;

    _cake_zmem(&r, 96);
    r.state = 2;
    r.value_type = 0;
    r.value.signed_int_value = value;
    return r;
}

signed int object_to_signed_int(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int _R10 = a->value_type;
        if (_R10 == 2) goto _CKL1; /*case 2*/
        if (_R10 == 3) goto _CKL2; /*case 3*/
        if (_R10 == 4) goto _CKL3; /*case 4*/
        if (_R10 == 5) goto _CKL4; /*case 5*/
        if (_R10 == 6) goto _CKL5; /*case 6*/
        if (_R10 == 0) goto _CKL6; /*case 0*/
        if (_R10 == 1) goto _CKL7; /*case 1*/
        if (_R10 == 7) goto _CKL8; /*case 7*/
        if (_R10 == 8) goto _CKL9; /*case 8*/
        if (_R10 == 9) goto _CKL10; /*case 9*/
        if (_R10 == 10) goto _CKL11; /*case 10*/
        if (_R10 == 11) goto _CKL12; /*case 11*/
        if (_R10 == 12) goto _CKL13; /*case 12*/
        if (_R10 == 13) goto _CKL14; /*case 13*/
        if (_R10 == 14) goto _CKL15; /*case 14*/
        goto _CKL0;

        {
            /*case 2*/ _CKL1:
            return a->value.bool_value;
            /*case 3*/ _CKL2:
            return a->value.signed_char_value;
            /*case 4*/ _CKL3:
            return a->value.unsigned_char_value;
            /*case 5*/ _CKL4:
            return a->value.signed_short_value;
            /*case 6*/ _CKL5:
            return a->value.unsigned_short_value;
            /*case 0*/ _CKL6:
            return a->value.signed_int_value;
            /*case 1*/ _CKL7:
            return a->value.unsigned_int_value;
            /*case 7*/ _CKL8:
            return a->value.signed_long_value;
            /*case 8*/ _CKL9:
            return a->value.unsigned_long_value;
            /*case 9*/ _CKL10:
            return a->value.signed_long_long_value;
            /*case 10*/ _CKL11:
            return a->value.unsigned_long_long_value;
            /*case 11*/ _CKL12:
            return a->value.float_value;
            /*case 12*/ _CKL13:
            return a->value.double_value;
            /*case 13*/ _CKL14:
            return a->value.long_double_value;
            /*case 14*/ _CKL15:
            return (int)a->value.void_pointer;
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    ;
    return 0;
}

struct object object_make_unsigned_int(unsigned int value)
{
    struct object  r;

    _cake_zmem(&r, 96);
    r.state = 2;
    r.value_type = 1;
    r.value.unsigned_int_value = value;
    return r;
}

unsigned int object_to_unsigned_int(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int _R11 = a->value_type;
        if (_R11 == 2) goto _CKL1; /*case 2*/
        if (_R11 == 3) goto _CKL2; /*case 3*/
        if (_R11 == 4) goto _CKL3; /*case 4*/
        if (_R11 == 5) goto _CKL4; /*case 5*/
        if (_R11 == 6) goto _CKL5; /*case 6*/
        if (_R11 == 0) goto _CKL6; /*case 0*/
        if (_R11 == 1) goto _CKL7; /*case 1*/
        if (_R11 == 7) goto _CKL8; /*case 7*/
        if (_R11 == 8) goto _CKL9; /*case 8*/
        if (_R11 == 9) goto _CKL10; /*case 9*/
        if (_R11 == 10) goto _CKL11; /*case 10*/
        if (_R11 == 11) goto _CKL12; /*case 11*/
        if (_R11 == 12) goto _CKL13; /*case 12*/
        if (_R11 == 13) goto _CKL14; /*case 13*/
        if (_R11 == 14) goto _CKL15; /*case 14*/
        goto _CKL0;

        {
            /*case 2*/ _CKL1:
            return a->value.bool_value;
            /*case 3*/ _CKL2:
            return a->value.signed_char_value;
            /*case 4*/ _CKL3:
            return a->value.unsigned_char_value;
            /*case 5*/ _CKL4:
            return a->value.signed_short_value;
            /*case 6*/ _CKL5:
            return a->value.unsigned_short_value;
            /*case 0*/ _CKL6:
            return a->value.signed_int_value;
            /*case 1*/ _CKL7:
            return a->value.unsigned_int_value;
            /*case 7*/ _CKL8:
            return a->value.signed_long_value;
            /*case 8*/ _CKL9:
            return a->value.unsigned_long_value;
            /*case 9*/ _CKL10:
            return a->value.signed_long_long_value;
            /*case 10*/ _CKL11:
            return a->value.unsigned_long_long_value;
            /*case 11*/ _CKL12:
            return a->value.float_value;
            /*case 12*/ _CKL13:
            return a->value.double_value;
            /*case 13*/ _CKL14:
            return a->value.long_double_value;
            /*case 14*/ _CKL15:
            return (int)a->value.void_pointer;
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    ;
    return 0;
}

struct object object_make_signed_long(signed long value)
{
    struct object  r;

    _cake_zmem(&r, 96);
    r.state = 2;
    r.value_type = 7;
    r.value.signed_long_value = value;
    return r;
}

signed long object_to_signed_long(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int _R12 = a->value_type;
        if (_R12 == 2) goto _CKL1; /*case 2*/
        if (_R12 == 3) goto _CKL2; /*case 3*/
        if (_R12 == 4) goto _CKL3; /*case 4*/
        if (_R12 == 5) goto _CKL4; /*case 5*/
        if (_R12 == 6) goto _CKL5; /*case 6*/
        if (_R12 == 0) goto _CKL6; /*case 0*/
        if (_R12 == 1) goto _CKL7; /*case 1*/
        if (_R12 == 7) goto _CKL8; /*case 7*/
        if (_R12 == 8) goto _CKL9; /*case 8*/
        if (_R12 == 9) goto _CKL10; /*case 9*/
        if (_R12 == 10) goto _CKL11; /*case 10*/
        if (_R12 == 11) goto _CKL12; /*case 11*/
        if (_R12 == 12) goto _CKL13; /*case 12*/
        if (_R12 == 13) goto _CKL14; /*case 13*/
        if (_R12 == 14) goto _CKL15; /*case 14*/
        goto _CKL0;

        {
            /*case 2*/ _CKL1:
            return a->value.bool_value;
            /*case 3*/ _CKL2:
            return a->value.signed_char_value;
            /*case 4*/ _CKL3:
            return a->value.unsigned_char_value;
            /*case 5*/ _CKL4:
            return a->value.signed_short_value;
            /*case 6*/ _CKL5:
            return a->value.unsigned_short_value;
            /*case 0*/ _CKL6:
            return a->value.signed_int_value;
            /*case 1*/ _CKL7:
            return a->value.unsigned_int_value;
            /*case 7*/ _CKL8:
            return a->value.signed_long_value;
            /*case 8*/ _CKL9:
            return a->value.unsigned_long_value;
            /*case 9*/ _CKL10:
            return a->value.signed_long_long_value;
            /*case 10*/ _CKL11:
            return a->value.unsigned_long_long_value;
            /*case 11*/ _CKL12:
            return a->value.float_value;
            /*case 12*/ _CKL13:
            return a->value.double_value;
            /*case 13*/ _CKL14:
            return a->value.long_double_value;
            /*case 14*/ _CKL15:
            ;
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    ;
    return 0;
}

struct object object_make_unsigned_long(unsigned long value)
{
    struct object  r;

    _cake_zmem(&r, 96);
    r.state = 2;
    r.value_type = 8;
    r.value.unsigned_long_value = value;
    return r;
}

unsigned long object_to_unsigned_long(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int _R13 = a->value_type;
        if (_R13 == 2) goto _CKL1; /*case 2*/
        if (_R13 == 3) goto _CKL2; /*case 3*/
        if (_R13 == 4) goto _CKL3; /*case 4*/
        if (_R13 == 5) goto _CKL4; /*case 5*/
        if (_R13 == 6) goto _CKL5; /*case 6*/
        if (_R13 == 0) goto _CKL6; /*case 0*/
        if (_R13 == 1) goto _CKL7; /*case 1*/
        if (_R13 == 7) goto _CKL8; /*case 7*/
        if (_R13 == 8) goto _CKL9; /*case 8*/
        if (_R13 == 9) goto _CKL10; /*case 9*/
        if (_R13 == 10) goto _CKL11; /*case 10*/
        if (_R13 == 11) goto _CKL12; /*case 11*/
        if (_R13 == 12) goto _CKL13; /*case 12*/
        if (_R13 == 13) goto _CKL14; /*case 13*/
        if (_R13 == 14) goto _CKL15; /*case 14*/
        goto _CKL0;

        {
            /*case 2*/ _CKL1:
            return a->value.bool_value;
            /*case 3*/ _CKL2:
            return a->value.signed_char_value;
            /*case 4*/ _CKL3:
            return a->value.unsigned_char_value;
            /*case 5*/ _CKL4:
            return a->value.signed_short_value;
            /*case 6*/ _CKL5:
            return a->value.unsigned_short_value;
            /*case 0*/ _CKL6:
            return a->value.signed_int_value;
            /*case 1*/ _CKL7:
            return a->value.unsigned_int_value;
            /*case 7*/ _CKL8:
            return a->value.signed_long_value;
            /*case 8*/ _CKL9:
            return a->value.unsigned_long_value;
            /*case 9*/ _CKL10:
            return a->value.signed_long_long_value;
            /*case 10*/ _CKL11:
            return a->value.unsigned_long_long_value;
            /*case 11*/ _CKL12:
            return a->value.float_value;
            /*case 12*/ _CKL13:
            return a->value.double_value;
            /*case 13*/ _CKL14:
            return a->value.long_double_value;
            /*case 14*/ _CKL15:
            ;
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    ;
    return 0;
}

struct object object_make_signed_long_long(signed long long value)
{
    struct object  r;

    _cake_zmem(&r, 96);
    r.state = 2;
    r.value_type = 9;
    r.value.signed_long_long_value = value;
    return r;
}

signed long long object_to_signed_long_long(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int _R14 = a->value_type;
        if (_R14 == 2) goto _CKL1; /*case 2*/
        if (_R14 == 3) goto _CKL2; /*case 3*/
        if (_R14 == 4) goto _CKL3; /*case 4*/
        if (_R14 == 5) goto _CKL4; /*case 5*/
        if (_R14 == 6) goto _CKL5; /*case 6*/
        if (_R14 == 0) goto _CKL6; /*case 0*/
        if (_R14 == 1) goto _CKL7; /*case 1*/
        if (_R14 == 7) goto _CKL8; /*case 7*/
        if (_R14 == 8) goto _CKL9; /*case 8*/
        if (_R14 == 9) goto _CKL10; /*case 9*/
        if (_R14 == 10) goto _CKL11; /*case 10*/
        if (_R14 == 11) goto _CKL12; /*case 11*/
        if (_R14 == 12) goto _CKL13; /*case 12*/
        if (_R14 == 13) goto _CKL14; /*case 13*/
        if (_R14 == 14) goto _CKL15; /*case 14*/
        goto _CKL0;

        {
            /*case 2*/ _CKL1:
            return a->value.bool_value;
            /*case 3*/ _CKL2:
            return a->value.signed_char_value;
            /*case 4*/ _CKL3:
            return a->value.unsigned_char_value;
            /*case 5*/ _CKL4:
            return a->value.signed_short_value;
            /*case 6*/ _CKL5:
            return a->value.unsigned_short_value;
            /*case 0*/ _CKL6:
            return a->value.signed_int_value;
            /*case 1*/ _CKL7:
            return a->value.unsigned_int_value;
            /*case 7*/ _CKL8:
            return a->value.signed_long_value;
            /*case 8*/ _CKL9:
            return a->value.unsigned_long_value;
            /*case 9*/ _CKL10:
            return a->value.signed_long_long_value;
            /*case 10*/ _CKL11:
            return a->value.unsigned_long_long_value;
            /*case 11*/ _CKL12:
            return a->value.float_value;
            /*case 12*/ _CKL13:
            return a->value.double_value;
            /*case 13*/ _CKL14:
            return a->value.long_double_value;
            /*case 14*/ _CKL15:
            ;
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    ;
    return 0;
}

struct object object_make_unsigned_long_long(unsigned long long value)
{
    struct object  r;

    _cake_zmem(&r, 96);
    r.state = 2;
    r.value_type = 10;
    r.value.unsigned_long_long_value = value;
    return r;
}

unsigned long long object_to_unsigned_long_long(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int _R15 = a->value_type;
        if (_R15 == 2) goto _CKL1; /*case 2*/
        if (_R15 == 3) goto _CKL2; /*case 3*/
        if (_R15 == 4) goto _CKL3; /*case 4*/
        if (_R15 == 5) goto _CKL4; /*case 5*/
        if (_R15 == 6) goto _CKL5; /*case 6*/
        if (_R15 == 0) goto _CKL6; /*case 0*/
        if (_R15 == 1) goto _CKL7; /*case 1*/
        if (_R15 == 7) goto _CKL8; /*case 7*/
        if (_R15 == 8) goto _CKL9; /*case 8*/
        if (_R15 == 9) goto _CKL10; /*case 9*/
        if (_R15 == 10) goto _CKL11; /*case 10*/
        if (_R15 == 11) goto _CKL12; /*case 11*/
        if (_R15 == 12) goto _CKL13; /*case 12*/
        if (_R15 == 13) goto _CKL14; /*case 13*/
        if (_R15 == 14) goto _CKL15; /*case 14*/
        goto _CKL0;

        {
            /*case 2*/ _CKL1:
            return a->value.bool_value;
            /*case 3*/ _CKL2:
            return a->value.signed_char_value;
            /*case 4*/ _CKL3:
            return a->value.unsigned_char_value;
            /*case 5*/ _CKL4:
            return a->value.signed_short_value;
            /*case 6*/ _CKL5:
            return a->value.unsigned_short_value;
            /*case 0*/ _CKL6:
            return a->value.signed_int_value;
            /*case 1*/ _CKL7:
            return a->value.unsigned_int_value;
            /*case 7*/ _CKL8:
            return a->value.signed_long_value;
            /*case 8*/ _CKL9:
            return a->value.unsigned_long_value;
            /*case 9*/ _CKL10:
            return a->value.signed_long_long_value;
            /*case 10*/ _CKL11:
            return a->value.unsigned_long_long_value;
            /*case 11*/ _CKL12:
            return a->value.float_value;
            /*case 12*/ _CKL13:
            return a->value.double_value;
            /*case 13*/ _CKL14:
            return a->value.long_double_value;
            /*case 14*/ _CKL15:
            ;
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    ;
    return 0;
}

struct object object_make_float(float value)
{
    struct object  r;

    _cake_zmem(&r, 96);
    r.state = 2;
    r.value_type = 11;
    r.value.float_value = value;
    return r;
}

float object_to_float(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int _R16 = a->value_type;
        if (_R16 == 2) goto _CKL1; /*case 2*/
        if (_R16 == 3) goto _CKL2; /*case 3*/
        if (_R16 == 4) goto _CKL3; /*case 4*/
        if (_R16 == 5) goto _CKL4; /*case 5*/
        if (_R16 == 6) goto _CKL5; /*case 6*/
        if (_R16 == 0) goto _CKL6; /*case 0*/
        if (_R16 == 1) goto _CKL7; /*case 1*/
        if (_R16 == 7) goto _CKL8; /*case 7*/
        if (_R16 == 8) goto _CKL9; /*case 8*/
        if (_R16 == 9) goto _CKL10; /*case 9*/
        if (_R16 == 10) goto _CKL11; /*case 10*/
        if (_R16 == 11) goto _CKL12; /*case 11*/
        if (_R16 == 12) goto _CKL13; /*case 12*/
        if (_R16 == 13) goto _CKL14; /*case 13*/
        if (_R16 == 14) goto _CKL15; /*case 14*/
        goto _CKL0;

        {
            /*case 2*/ _CKL1:
            return a->value.bool_value;
            /*case 3*/ _CKL2:
            return a->value.signed_char_value;
            /*case 4*/ _CKL3:
            return a->value.unsigned_char_value;
            /*case 5*/ _CKL4:
            return a->value.signed_short_value;
            /*case 6*/ _CKL5:
            return a->value.unsigned_short_value;
            /*case 0*/ _CKL6:
            return a->value.signed_int_value;
            /*case 1*/ _CKL7:
            return a->value.unsigned_int_value;
            /*case 7*/ _CKL8:
            return a->value.signed_long_value;
            /*case 8*/ _CKL9:
            return a->value.unsigned_long_value;
            /*case 9*/ _CKL10:
            return a->value.signed_long_long_value;
            /*case 10*/ _CKL11:
            return a->value.unsigned_long_long_value;
            /*case 11*/ _CKL12:
            return a->value.float_value;
            /*case 12*/ _CKL13:
            return a->value.double_value;
            /*case 13*/ _CKL14:
            return a->value.long_double_value;
            /*case 14*/ _CKL15:
            ;
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    ;
    return 0;
}

struct object object_make_double(double value)
{
    struct object  r;

    _cake_zmem(&r, 96);
    r.state = 2;
    r.value_type = 12;
    r.value.double_value = value;
    return r;
}

double object_to_double(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int _R17 = a->value_type;
        if (_R17 == 2) goto _CKL1; /*case 2*/
        if (_R17 == 3) goto _CKL2; /*case 3*/
        if (_R17 == 4) goto _CKL3; /*case 4*/
        if (_R17 == 5) goto _CKL4; /*case 5*/
        if (_R17 == 6) goto _CKL5; /*case 6*/
        if (_R17 == 0) goto _CKL6; /*case 0*/
        if (_R17 == 1) goto _CKL7; /*case 1*/
        if (_R17 == 7) goto _CKL8; /*case 7*/
        if (_R17 == 8) goto _CKL9; /*case 8*/
        if (_R17 == 9) goto _CKL10; /*case 9*/
        if (_R17 == 10) goto _CKL11; /*case 10*/
        if (_R17 == 11) goto _CKL12; /*case 11*/
        if (_R17 == 12) goto _CKL13; /*case 12*/
        if (_R17 == 13) goto _CKL14; /*case 13*/
        if (_R17 == 14) goto _CKL15; /*case 14*/
        goto _CKL0;

        {
            /*case 2*/ _CKL1:
            return a->value.bool_value;
            /*case 3*/ _CKL2:
            return a->value.signed_char_value;
            /*case 4*/ _CKL3:
            return a->value.unsigned_char_value;
            /*case 5*/ _CKL4:
            return a->value.signed_short_value;
            /*case 6*/ _CKL5:
            return a->value.unsigned_short_value;
            /*case 0*/ _CKL6:
            return a->value.signed_int_value;
            /*case 1*/ _CKL7:
            return a->value.unsigned_int_value;
            /*case 7*/ _CKL8:
            return a->value.signed_long_value;
            /*case 8*/ _CKL9:
            return a->value.unsigned_long_value;
            /*case 9*/ _CKL10:
            return a->value.signed_long_long_value;
            /*case 10*/ _CKL11:
            return a->value.unsigned_long_long_value;
            /*case 11*/ _CKL12:
            return a->value.float_value;
            /*case 12*/ _CKL13:
            return a->value.double_value;
            /*case 13*/ _CKL14:
            return a->value.long_double_value;
            /*case 14*/ _CKL15:
            ;
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    ;
    return 0;
}

struct object object_make_null_pointer()
{
    struct object  null_object;

    null_object.state = 4;
    null_object.value_type = 14;
    null_object.type.category = 0;
    null_object.type.attributes_flags = 0;
    null_object.type.type_specifier_flags = 0;
    null_object.type.type_qualifier_flags = 0;
    null_object.type.storage_class_specifier_flags = 0;
    null_object.type.name_opt = 0;
    null_object.type.struct_or_union_specifier = 0;
    null_object.type.enum_specifier = 0;
    null_object.type.array_num_elements_expression = 0;
    null_object.type.num_of_elements = 0;
    null_object.type.has_static_array_size = 0;
    null_object.type.address_of = 0;
    null_object.type.params.is_var_args = 0;
    null_object.type.params.is_void = 0;
    null_object.type.params.head = 0;
    null_object.type.params.tail = 0;
    null_object.type.next = 0;
    null_object.debug_name = 0;
    null_object.value.bool_value = 0;
    null_object.value.signed_char_value = 0;
    null_object.value.unsigned_char_value = 0;
    null_object.value.signed_short_value = 0;
    null_object.value.unsigned_short_value = 0;
    null_object.value.signed_int_value = 0;
    null_object.value.unsigned_int_value = 0;
    null_object.value.signed_long_value = 0;
    null_object.value.unsigned_long_value = 0;
    null_object.value.signed_long_long_value = 0;
    null_object.value.unsigned_long_long_value = 0;
    null_object.value.float_value = 0;
    null_object.value.double_value = 0;
    null_object.value.long_double_value = 0;
    null_object.value.void_pointer = 0U;
    null_object.parent = 0;
    null_object.p_init_expression = 0;
    null_object.members = 0;
    null_object.next = 0;
    return null_object;
}

struct object object_make_pointer(struct object * object)
{
    struct object  r;

    object = object_get_non_const_referenced(object);
    _cake_zmem(&r, 96);
    r.state = 4;
    r.value_type = 14;
    r.value.void_pointer = object;
    return r;
}

struct object object_make_reference(struct object * object)
{
    struct object  r;

    object = object_get_non_const_referenced(object);
    _cake_zmem(&r, 96);
    r.state = 4;
    r.value_type = 15;
    r.value.void_pointer = object;
    return r;
}

struct object object_make_long_double(long double value)
{
    struct object  r;

    _cake_zmem(&r, 96);
    r.state = 2;
    r.value_type = 13;
    r.value.long_double_value = value;
    return r;
}

long double object_to_long_double(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int _R18 = a->value_type;
        if (_R18 == 2) goto _CKL1; /*case 2*/
        if (_R18 == 3) goto _CKL2; /*case 3*/
        if (_R18 == 4) goto _CKL3; /*case 4*/
        if (_R18 == 5) goto _CKL4; /*case 5*/
        if (_R18 == 6) goto _CKL5; /*case 6*/
        if (_R18 == 0) goto _CKL6; /*case 0*/
        if (_R18 == 1) goto _CKL7; /*case 1*/
        if (_R18 == 7) goto _CKL8; /*case 7*/
        if (_R18 == 8) goto _CKL9; /*case 8*/
        if (_R18 == 9) goto _CKL10; /*case 9*/
        if (_R18 == 10) goto _CKL11; /*case 10*/
        if (_R18 == 11) goto _CKL12; /*case 11*/
        if (_R18 == 12) goto _CKL13; /*case 12*/
        if (_R18 == 13) goto _CKL14; /*case 13*/
        if (_R18 == 14) goto _CKL15; /*case 14*/
        goto _CKL0;

        {
            /*case 2*/ _CKL1:
            return a->value.bool_value;
            /*case 3*/ _CKL2:
            return a->value.signed_char_value;
            /*case 4*/ _CKL3:
            return a->value.unsigned_char_value;
            /*case 5*/ _CKL4:
            return a->value.signed_short_value;
            /*case 6*/ _CKL5:
            return a->value.unsigned_short_value;
            /*case 0*/ _CKL6:
            return a->value.signed_int_value;
            /*case 1*/ _CKL7:
            return a->value.unsigned_int_value;
            /*case 7*/ _CKL8:
            return a->value.signed_long_value;
            /*case 8*/ _CKL9:
            return a->value.unsigned_long_value;
            /*case 9*/ _CKL10:
            return a->value.signed_long_long_value;
            /*case 10*/ _CKL11:
            return a->value.unsigned_long_long_value;
            /*case 11*/ _CKL12:
            return a->value.float_value;
            /*case 12*/ _CKL13:
            return a->value.double_value;
            /*case 13*/ _CKL14:
            return a->value.long_double_value;
            /*case 14*/ _CKL15:
            ;
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    ;
    return 0;
}

struct object object_cast(int t, struct object * v)
{
    struct object  empty;

    v = object_get_referenced(v);
    if (v->value_type == t)
    {
        return *v;
    }
    if (t == 2)
    {
        if (v->value_type == 3)
        {
            return object_make_bool((unsigned char )v->value.signed_char_value);
        }
        if (v->value_type == 4)
        {
            return object_make_bool((unsigned char )v->value.unsigned_char_value);
        }
        if (v->value_type == 5)
        {
            return object_make_bool((unsigned char )v->value.signed_short_value);
        }
        if (v->value_type == 6)
        {
            return object_make_bool((unsigned char )v->value.unsigned_short_value);
        }
        if (v->value_type == 0)
        {
            return object_make_bool((unsigned char )v->value.signed_int_value);
        }
        if (v->value_type == 1)
        {
            return object_make_bool((unsigned char )v->value.unsigned_int_value);
        }
        if (v->value_type == 7)
        {
            return object_make_bool((unsigned char )v->value.signed_long_value);
        }
        if (v->value_type == 8)
        {
            return object_make_bool((unsigned char )v->value.unsigned_long_value);
        }
        if (v->value_type == 9)
        {
            return object_make_bool((unsigned char )v->value.signed_long_long_value);
        }
        if (v->value_type == 10)
        {
            return object_make_bool((unsigned char )v->value.unsigned_long_long_value);
        }
        if (v->value_type == 11)
        {
            return object_make_bool((unsigned char )v->value.float_value);
        }
        if (v->value_type == 12)
        {
            return object_make_bool((unsigned char )v->value.double_value);
        }
        if (v->value_type == 13)
        {
            return object_make_bool((unsigned char )v->value.long_double_value);
        }
    }
    if (t == 3)
    {
        if (v->value_type == 2)
        {
            return object_make_signed_char((signed char)v->value.bool_value);
        }
        if (v->value_type == 4)
        {
            return object_make_signed_char((signed char)v->value.unsigned_char_value);
        }
        if (v->value_type == 5)
        {
            return object_make_signed_char((signed char)v->value.signed_short_value);
        }
        if (v->value_type == 6)
        {
            return object_make_signed_char((signed char)v->value.unsigned_short_value);
        }
        if (v->value_type == 0)
        {
            return object_make_signed_char((signed char)v->value.signed_int_value);
        }
        if (v->value_type == 1)
        {
            return object_make_signed_char((signed char)v->value.unsigned_int_value);
        }
        if (v->value_type == 7)
        {
            return object_make_signed_char((signed char)v->value.signed_long_value);
        }
        if (v->value_type == 8)
        {
            return object_make_signed_char((signed char)v->value.unsigned_long_value);
        }
        if (v->value_type == 9)
        {
            return object_make_signed_char((signed char)v->value.signed_long_long_value);
        }
        if (v->value_type == 10)
        {
            return object_make_signed_char((signed char)v->value.unsigned_long_long_value);
        }
        if (v->value_type == 11)
        {
            return object_make_signed_char((signed char)v->value.float_value);
        }
        if (v->value_type == 12)
        {
            return object_make_signed_char((signed char)v->value.double_value);
        }
        if (v->value_type == 13)
        {
            return object_make_signed_char((signed char)v->value.long_double_value);
        }
    }
    if (t == 4)
    {
        if (v->value_type == 2)
        {
            return object_make_unsigned_char((unsigned char)v->value.bool_value);
        }
        if (v->value_type == 3)
        {
            return object_make_unsigned_char((unsigned char)v->value.signed_char_value);
        }
        if (v->value_type == 5)
        {
            return object_make_unsigned_char((unsigned char)v->value.signed_short_value);
        }
        if (v->value_type == 6)
        {
            return object_make_unsigned_char((unsigned char)v->value.unsigned_short_value);
        }
        if (v->value_type == 0)
        {
            return object_make_unsigned_char((unsigned char)v->value.signed_int_value);
        }
        if (v->value_type == 1)
        {
            return object_make_unsigned_char((unsigned char)v->value.unsigned_int_value);
        }
        if (v->value_type == 7)
        {
            return object_make_unsigned_char((unsigned char)v->value.signed_long_value);
        }
        if (v->value_type == 8)
        {
            return object_make_unsigned_char((unsigned char)v->value.unsigned_long_value);
        }
        if (v->value_type == 9)
        {
            return object_make_unsigned_char((unsigned char)v->value.signed_long_long_value);
        }
        if (v->value_type == 10)
        {
            return object_make_unsigned_char((unsigned char)v->value.unsigned_long_long_value);
        }
        if (v->value_type == 11)
        {
            return object_make_unsigned_char((unsigned char)v->value.float_value);
        }
        if (v->value_type == 12)
        {
            return object_make_unsigned_char((unsigned char)v->value.double_value);
        }
        if (v->value_type == 13)
        {
            return object_make_unsigned_char((unsigned char)v->value.long_double_value);
        }
    }
    if (t == 5)
    {
        if (v->value_type == 2)
        {
            return object_make_signed_short((signed short)v->value.bool_value);
        }
        if (v->value_type == 3)
        {
            return object_make_signed_short((signed short)v->value.signed_char_value);
        }
        if (v->value_type == 4)
        {
            return object_make_signed_short((signed short)v->value.unsigned_char_value);
        }
        if (v->value_type == 6)
        {
            return object_make_signed_short((signed short)v->value.unsigned_short_value);
        }
        if (v->value_type == 0)
        {
            return object_make_signed_short((signed short)v->value.signed_int_value);
        }
        if (v->value_type == 1)
        {
            return object_make_signed_short((signed short)v->value.unsigned_int_value);
        }
        if (v->value_type == 7)
        {
            return object_make_signed_short((signed short)v->value.signed_long_value);
        }
        if (v->value_type == 8)
        {
            return object_make_signed_short((signed short)v->value.unsigned_long_value);
        }
        if (v->value_type == 9)
        {
            return object_make_signed_short((signed short)v->value.signed_long_long_value);
        }
        if (v->value_type == 10)
        {
            return object_make_signed_short((signed short)v->value.unsigned_long_long_value);
        }
        if (v->value_type == 11)
        {
            return object_make_signed_short((signed short)v->value.float_value);
        }
        if (v->value_type == 12)
        {
            return object_make_signed_short((signed short)v->value.double_value);
        }
        if (v->value_type == 13)
        {
            return object_make_signed_short((signed short)v->value.long_double_value);
        }
    }
    if (t == 6)
    {
        if (v->value_type == 2)
        {
            return object_make_unsigned_short((unsigned short)v->value.bool_value);
        }
        if (v->value_type == 3)
        {
            return object_make_unsigned_short((unsigned short)v->value.signed_char_value);
        }
        if (v->value_type == 4)
        {
            return object_make_unsigned_short((unsigned short)v->value.unsigned_char_value);
        }
        if (v->value_type == 5)
        {
            return object_make_unsigned_short((unsigned short)v->value.signed_short_value);
        }
        if (v->value_type == 0)
        {
            return object_make_unsigned_short((unsigned short)v->value.signed_int_value);
        }
        if (v->value_type == 1)
        {
            return object_make_unsigned_short((unsigned short)v->value.unsigned_int_value);
        }
        if (v->value_type == 7)
        {
            return object_make_unsigned_short((unsigned short)v->value.signed_long_value);
        }
        if (v->value_type == 8)
        {
            return object_make_unsigned_short((unsigned short)v->value.unsigned_long_value);
        }
        if (v->value_type == 9)
        {
            return object_make_unsigned_short((unsigned short)v->value.signed_long_long_value);
        }
        if (v->value_type == 10)
        {
            return object_make_unsigned_short((unsigned short)v->value.unsigned_long_long_value);
        }
        if (v->value_type == 11)
        {
            return object_make_unsigned_short((unsigned short)v->value.float_value);
        }
        if (v->value_type == 12)
        {
            return object_make_unsigned_short((unsigned short)v->value.double_value);
        }
        if (v->value_type == 13)
        {
            return object_make_unsigned_short((unsigned short)v->value.long_double_value);
        }
    }
    if (t == 0)
    {
        if (v->value_type == 2)
        {
            return object_make_signed_int((signed int)v->value.bool_value);
        }
        if (v->value_type == 3)
        {
            return object_make_signed_int((signed int)v->value.signed_char_value);
        }
        if (v->value_type == 4)
        {
            return object_make_signed_int((signed int)v->value.unsigned_char_value);
        }
        if (v->value_type == 5)
        {
            return object_make_signed_int((signed int)v->value.signed_short_value);
        }
        if (v->value_type == 6)
        {
            return object_make_signed_int((signed int)v->value.unsigned_short_value);
        }
        if (v->value_type == 1)
        {
            return object_make_signed_int((signed int)v->value.unsigned_int_value);
        }
        if (v->value_type == 7)
        {
            return object_make_signed_int((signed int)v->value.signed_long_value);
        }
        if (v->value_type == 8)
        {
            return object_make_signed_int((signed int)v->value.unsigned_long_value);
        }
        if (v->value_type == 9)
        {
            return object_make_signed_int((signed int)v->value.signed_long_long_value);
        }
        if (v->value_type == 10)
        {
            return object_make_signed_int((signed int)v->value.unsigned_long_long_value);
        }
        if (v->value_type == 11)
        {
            return object_make_signed_int((signed int)v->value.float_value);
        }
        if (v->value_type == 12)
        {
            return object_make_signed_int((signed int)v->value.double_value);
        }
        if (v->value_type == 13)
        {
            return object_make_signed_int((signed int)v->value.long_double_value);
        }
    }
    if (t == 1)
    {
        if (v->value_type == 2)
        {
            return object_make_unsigned_int((unsigned int)v->value.bool_value);
        }
        if (v->value_type == 3)
        {
            return object_make_unsigned_int((unsigned int)v->value.signed_char_value);
        }
        if (v->value_type == 4)
        {
            return object_make_unsigned_int((unsigned int)v->value.unsigned_char_value);
        }
        if (v->value_type == 5)
        {
            return object_make_unsigned_int((unsigned int)v->value.signed_short_value);
        }
        if (v->value_type == 6)
        {
            return object_make_unsigned_int((unsigned int)v->value.unsigned_short_value);
        }
        if (v->value_type == 0)
        {
            return object_make_unsigned_int((unsigned int)v->value.signed_int_value);
        }
        if (v->value_type == 7)
        {
            return object_make_unsigned_int((unsigned int)v->value.signed_long_value);
        }
        if (v->value_type == 8)
        {
            return object_make_unsigned_int((unsigned int)v->value.unsigned_long_value);
        }
        if (v->value_type == 9)
        {
            return object_make_unsigned_int((unsigned int)v->value.signed_long_long_value);
        }
        if (v->value_type == 10)
        {
            return object_make_unsigned_int((unsigned int)v->value.unsigned_long_long_value);
        }
        if (v->value_type == 11)
        {
            return object_make_unsigned_int((unsigned int)v->value.float_value);
        }
        if (v->value_type == 12)
        {
            return object_make_unsigned_int((unsigned int)v->value.double_value);
        }
        if (v->value_type == 13)
        {
            return object_make_unsigned_int((unsigned int)v->value.long_double_value);
        }
    }
    if (t == 7)
    {
        if (v->value_type == 2)
        {
            return object_make_signed_long((signed long)v->value.bool_value);
        }
        if (v->value_type == 3)
        {
            return object_make_signed_long((signed long)v->value.signed_char_value);
        }
        if (v->value_type == 4)
        {
            return object_make_signed_long((signed long)v->value.unsigned_char_value);
        }
        if (v->value_type == 5)
        {
            return object_make_signed_long((signed long)v->value.signed_short_value);
        }
        if (v->value_type == 6)
        {
            return object_make_signed_long((signed long)v->value.unsigned_short_value);
        }
        if (v->value_type == 0)
        {
            return object_make_signed_long((signed long)v->value.signed_int_value);
        }
        if (v->value_type == 1)
        {
            return object_make_signed_long((signed long)v->value.unsigned_int_value);
        }
        if (v->value_type == 8)
        {
            return object_make_signed_long((signed long)v->value.unsigned_long_value);
        }
        if (v->value_type == 9)
        {
            return object_make_signed_long((signed long)v->value.signed_long_long_value);
        }
        if (v->value_type == 10)
        {
            return object_make_signed_long((signed long)v->value.unsigned_long_long_value);
        }
        if (v->value_type == 11)
        {
            return object_make_signed_long((signed long)v->value.float_value);
        }
        if (v->value_type == 12)
        {
            return object_make_signed_long((signed long)v->value.double_value);
        }
        if (v->value_type == 13)
        {
            return object_make_signed_long((signed long)v->value.long_double_value);
        }
    }
    if (t == 8)
    {
        if (v->value_type == 2)
        {
            return object_make_unsigned_long((unsigned long)v->value.bool_value);
        }
        if (v->value_type == 3)
        {
            return object_make_unsigned_long((unsigned long)v->value.signed_char_value);
        }
        if (v->value_type == 4)
        {
            return object_make_unsigned_long((unsigned long)v->value.unsigned_char_value);
        }
        if (v->value_type == 5)
        {
            return object_make_unsigned_long((unsigned long)v->value.signed_short_value);
        }
        if (v->value_type == 6)
        {
            return object_make_unsigned_long((unsigned long)v->value.unsigned_short_value);
        }
        if (v->value_type == 0)
        {
            return object_make_unsigned_long((unsigned long)v->value.signed_int_value);
        }
        if (v->value_type == 1)
        {
            return object_make_unsigned_long((unsigned long)v->value.unsigned_int_value);
        }
        if (v->value_type == 7)
        {
            return object_make_unsigned_long((unsigned long)v->value.signed_long_value);
        }
        if (v->value_type == 9)
        {
            return object_make_unsigned_long((unsigned long)v->value.signed_long_long_value);
        }
        if (v->value_type == 10)
        {
            return object_make_unsigned_long((unsigned long)v->value.unsigned_long_long_value);
        }
        if (v->value_type == 11)
        {
            return object_make_unsigned_long((unsigned long)v->value.float_value);
        }
        if (v->value_type == 12)
        {
            return object_make_unsigned_long((unsigned long)v->value.double_value);
        }
        if (v->value_type == 13)
        {
            return object_make_unsigned_long((unsigned long)v->value.long_double_value);
        }
    }
    if (t == 9)
    {
        if (v->value_type == 2)
        {
            return object_make_signed_long_long((signed long long)v->value.bool_value);
        }
        if (v->value_type == 3)
        {
            return object_make_signed_long_long((signed long long)v->value.signed_char_value);
        }
        if (v->value_type == 4)
        {
            return object_make_signed_long_long((signed long long)v->value.unsigned_char_value);
        }
        if (v->value_type == 5)
        {
            return object_make_signed_long_long((signed long long)v->value.signed_short_value);
        }
        if (v->value_type == 6)
        {
            return object_make_signed_long_long((signed long long)v->value.unsigned_short_value);
        }
        if (v->value_type == 0)
        {
            return object_make_signed_long_long((signed long long)v->value.signed_int_value);
        }
        if (v->value_type == 1)
        {
            return object_make_signed_long_long((signed long long)v->value.unsigned_int_value);
        }
        if (v->value_type == 7)
        {
            return object_make_signed_long_long((signed long long)v->value.signed_long_value);
        }
        if (v->value_type == 8)
        {
            return object_make_signed_long_long((signed long long)v->value.unsigned_long_value);
        }
        if (v->value_type == 10)
        {
            return object_make_signed_long_long((signed long long)v->value.unsigned_long_long_value);
        }
        if (v->value_type == 11)
        {
            return object_make_signed_long_long((signed long long)v->value.float_value);
        }
        if (v->value_type == 12)
        {
            return object_make_signed_long_long((signed long long)v->value.double_value);
        }
        if (v->value_type == 13)
        {
            return object_make_signed_long_long((signed long long)v->value.long_double_value);
        }
    }
    if (t == 10)
    {
        if (v->value_type == 2)
        {
            return object_make_unsigned_long_long((unsigned long long)v->value.bool_value);
        }
        if (v->value_type == 3)
        {
            return object_make_unsigned_long_long((unsigned long long)v->value.signed_char_value);
        }
        if (v->value_type == 4)
        {
            return object_make_unsigned_long_long((unsigned long long)v->value.unsigned_char_value);
        }
        if (v->value_type == 5)
        {
            return object_make_unsigned_long_long((unsigned long long)v->value.signed_short_value);
        }
        if (v->value_type == 6)
        {
            return object_make_unsigned_long_long((unsigned long long)v->value.unsigned_short_value);
        }
        if (v->value_type == 0)
        {
            return object_make_unsigned_long_long((unsigned long long)v->value.signed_int_value);
        }
        if (v->value_type == 1)
        {
            return object_make_unsigned_long_long((unsigned long long)v->value.unsigned_int_value);
        }
        if (v->value_type == 7)
        {
            return object_make_unsigned_long_long((unsigned long long)v->value.signed_long_value);
        }
        if (v->value_type == 8)
        {
            return object_make_unsigned_long_long((unsigned long long)v->value.unsigned_long_value);
        }
        if (v->value_type == 9)
        {
            return object_make_unsigned_long_long((unsigned long long)v->value.signed_long_long_value);
        }
        if (v->value_type == 11)
        {
            return object_make_unsigned_long_long((unsigned long long)v->value.float_value);
        }
        if (v->value_type == 12)
        {
            return object_make_unsigned_long_long((unsigned long long)v->value.double_value);
        }
        if (v->value_type == 13)
        {
            return object_make_unsigned_long_long((unsigned long long)v->value.long_double_value);
        }
    }
    if (t == 11)
    {
        if (v->value_type == 2)
        {
            return object_make_float((float)v->value.bool_value);
        }
        if (v->value_type == 3)
        {
            return object_make_float((float)v->value.signed_char_value);
        }
        if (v->value_type == 4)
        {
            return object_make_float((float)v->value.unsigned_char_value);
        }
        if (v->value_type == 5)
        {
            return object_make_float((float)v->value.signed_short_value);
        }
        if (v->value_type == 6)
        {
            return object_make_float((float)v->value.unsigned_short_value);
        }
        if (v->value_type == 0)
        {
            return object_make_float((float)v->value.signed_int_value);
        }
        if (v->value_type == 1)
        {
            return object_make_float((float)v->value.unsigned_int_value);
        }
        if (v->value_type == 7)
        {
            return object_make_float((float)v->value.signed_long_value);
        }
        if (v->value_type == 8)
        {
            return object_make_float((float)v->value.unsigned_long_value);
        }
        if (v->value_type == 9)
        {
            return object_make_float((float)v->value.signed_long_long_value);
        }
        if (v->value_type == 10)
        {
            return object_make_float((float)v->value.unsigned_long_long_value);
        }
        if (v->value_type == 12)
        {
            return object_make_float((float)v->value.double_value);
        }
        if (v->value_type == 13)
        {
            return object_make_float((float)v->value.long_double_value);
        }
    }
    if (t == 12)
    {
        if (v->value_type == 2)
        {
            return object_make_double((double)v->value.bool_value);
        }
        if (v->value_type == 3)
        {
            return object_make_double((double)v->value.signed_char_value);
        }
        if (v->value_type == 4)
        {
            return object_make_double((double)v->value.unsigned_char_value);
        }
        if (v->value_type == 5)
        {
            return object_make_double((double)v->value.signed_short_value);
        }
        if (v->value_type == 6)
        {
            return object_make_double((double)v->value.unsigned_short_value);
        }
        if (v->value_type == 0)
        {
            return object_make_double((double)v->value.signed_int_value);
        }
        if (v->value_type == 1)
        {
            return object_make_double((double)v->value.unsigned_int_value);
        }
        if (v->value_type == 7)
        {
            return object_make_double((double)v->value.signed_long_value);
        }
        if (v->value_type == 8)
        {
            return object_make_double((double)v->value.unsigned_long_value);
        }
        if (v->value_type == 9)
        {
            return object_make_double((double)v->value.signed_long_long_value);
        }
        if (v->value_type == 10)
        {
            return object_make_double((double)v->value.unsigned_long_long_value);
        }
        if (v->value_type == 11)
        {
            return object_make_double((double)v->value.float_value);
        }
        if (v->value_type == 13)
        {
            return object_make_double((double)v->value.long_double_value);
        }
    }
    if (t == 13)
    {
        if (v->value_type == 2)
        {
            return object_make_long_double((long double)v->value.bool_value);
        }
        if (v->value_type == 3)
        {
            return object_make_long_double((long double)v->value.signed_char_value);
        }
        if (v->value_type == 4)
        {
            return object_make_long_double((long double)v->value.unsigned_char_value);
        }
        if (v->value_type == 5)
        {
            return object_make_long_double((long double)v->value.signed_short_value);
        }
        if (v->value_type == 6)
        {
            return object_make_long_double((long double)v->value.unsigned_short_value);
        }
        if (v->value_type == 0)
        {
            return object_make_long_double((long double)v->value.signed_int_value);
        }
        if (v->value_type == 1)
        {
            return object_make_long_double((long double)v->value.unsigned_int_value);
        }
        if (v->value_type == 7)
        {
            return object_make_long_double((long double)v->value.signed_long_value);
        }
        if (v->value_type == 8)
        {
            return object_make_long_double((long double)v->value.unsigned_long_value);
        }
        if (v->value_type == 9)
        {
            return object_make_long_double((long double)v->value.signed_long_long_value);
        }
        if (v->value_type == 10)
        {
            return object_make_long_double((long double)v->value.unsigned_long_long_value);
        }
        if (v->value_type == 11)
        {
            return object_make_long_double((long double)v->value.float_value);
        }
        if (v->value_type == 12)
        {
            return object_make_long_double((long double)v->value.double_value);
        }
    }
    _cake_zmem(&empty, 96);
    return empty;
}

unsigned char  type_is_union(struct type * p_type);

void object_default_initialization(struct object * p_object, unsigned char   is_constant)
{
    if (p_object->members == 0U)
    {
        if (is_constant)
        {
            p_object->state = 2;
        }
        else
        {
            p_object->state = 4;
        }
        p_object->value.unsigned_long_long_value = 0;
    }
    if (type_is_union(&p_object->type))
    {
        struct object * p;

        p = p_object->members;
        if (p)
        {
            object_default_initialization(p, is_constant);
        }
    }
    else
    {
        struct object * p;

        p = p_object->members;
        while (p)
        {
            object_default_initialization(p, is_constant);
            p = p->next;
        }
    }
}

struct object *object_get_non_const_referenced(struct object * p_object)
{
    if (p_object->value_type == 15)
    {
        p_object = p_object->value.void_pointer;
    }
    ;
    return p_object;
}

struct object *object_get_referenced(struct object * p_object)
{
    if (p_object->value_type == 15)
    {
        p_object = p_object->value.void_pointer;
    }
    ;
    return p_object;
}

int get_rank(int t)
{
    if (t == 9 || t == 10)
    {
        return 80;
    }
    else
    {
        if (t == 7 || t == 8)
        {
            return 50;
        }
        else
        {
            if (t == 0 || t == 1)
            {
                return 40;
            }
            else
            {
                if (t == 5 || t == 6)
                {
                    return 30;
                }
                else
                {
                    if (t == 3 || t == 4)
                    {
                        return 20;
                    }
                }
            }
        }
    }
    return 0;
}

int get_size(int t)
{
    if (t == 9 || t == 10)
    {
        return 8U;
    }
    else
    {
        if (t == 7 || t == 8)
        {
            return 4U;
        }
        else
        {
            if (t == 0 || t == 1)
            {
                return 4U;
            }
            else
            {
                if (t == 5 || t == 6)
                {
                    return 2U;
                }
                else
                {
                    if (t == 3 || t == 4)
                    {
                        return 1U;
                    }
                    else
                    {
                        if (t == 14)
                        {
                            return 4U;
                        }
                    }
                }
            }
        }
    }
    return 1;
}

unsigned char  is_signed(int t)
{
    /*switch*/
    {
        register int _R19 = t;
        if (_R19 == 2) goto _CKL1; /*case 2*/
        if (_R19 == 3) goto _CKL2; /*case 3*/
        if (_R19 == 5) goto _CKL3; /*case 5*/
        if (_R19 == 0) goto _CKL4; /*case 0*/
        if (_R19 == 7) goto _CKL5; /*case 7*/
        if (_R19 == 9) goto _CKL6; /*case 9*/
        if (_R19 == 12) goto _CKL7; /*case 12*/
        if (_R19 == 13) goto _CKL8; /*case 13*/
        if (_R19 == 14) goto _CKL9; /*case 14*/
        goto /*default*/ _CKL10;

        {
            /*case 2*/ _CKL1:
            /*case 3*/ _CKL2:
            /*case 5*/ _CKL3:
            /*case 0*/ _CKL4:
            /*case 7*/ _CKL5:
            /*case 9*/ _CKL6:
            /*case 12*/ _CKL7:
            /*case 13*/ _CKL8:
            return 1;
            /*case 14*/ _CKL9:
            /*break*/ goto _CKL0;

            /*default*/ _CKL10:
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    return 0;
}

int to_unsigned(int t)
{
    /*switch*/
    {
        register int _R20 = t;
        if (_R20 == 3) goto _CKL1; /*case 3*/
        if (_R20 == 5) goto _CKL2; /*case 5*/
        if (_R20 == 0) goto _CKL3; /*case 0*/
        if (_R20 == 7) goto _CKL4; /*case 7*/
        if (_R20 == 9) goto _CKL5; /*case 9*/
        goto /*default*/ _CKL6;

        {
            /*case 3*/ _CKL1:
            return 4;
            /*case 5*/ _CKL2:
            return 6;
            /*case 0*/ _CKL3:
            return 1;
            /*case 7*/ _CKL4:
            return 8;
            /*case 9*/ _CKL5:
            return 10;
            /*default*/ _CKL6:
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    return t;
}

unsigned char  is_unsigned(int t)
{
    /*switch*/
    {
        register int _R21 = t;
        if (_R21 == 2) goto _CKL1; /*case 2*/
        if (_R21 == 4) goto _CKL2; /*case 4*/
        if (_R21 == 6) goto _CKL3; /*case 6*/
        if (_R21 == 1) goto _CKL4; /*case 1*/
        if (_R21 == 8) goto _CKL5; /*case 8*/
        if (_R21 == 10) goto _CKL6; /*case 10*/
        goto /*default*/ _CKL7;

        {
            /*case 2*/ _CKL1:
            /*case 4*/ _CKL2:
            /*case 6*/ _CKL3:
            /*case 1*/ _CKL4:
            /*case 8*/ _CKL5:
            /*case 10*/ _CKL6:
            return 1;
            /*default*/ _CKL7:
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    return 0;
}

void object_set_any(struct object * p_object)
{
    struct object * p;

    p_object = object_get_non_const_referenced(p_object);
    p_object->state = 1;
    p = p_object->members;
    while (p)
    {
        object_set_any(p);
        p = p->next;
    }
}

unsigned char  object_is_signed(struct object * p_object)
{
    p_object = (struct object *)object_get_referenced(p_object);
    return is_signed(p_object->value_type);
}

unsigned char  object_is_derived(struct object * p_object)
{
    if (p_object->value_type == 15)
    {
        return 0;
    }
    return !!(p_object->members != 0U);
}

unsigned char  object_is_reference(struct object * p_object)
{
    return !!(p_object->value_type == 15);
}

static void object_fix_parent(struct object * p_object, struct object * parent)
{
    struct object * it;

    it = p_object->members;
    while (it)
    {
        it->parent = parent;
        it = it->next;
    }
}

struct object *object_get_member(struct object * p_object, int index)
{
    struct object * it;
    int count;

    p_object = (struct object *)object_get_referenced(p_object);
    if (p_object->members == 0U)
    {
        return 0U;
    }
    it = p_object->members;
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
    return 0U;
}

unsigned char  type_is_pointer_or_array(struct type * p_type);
unsigned char  type_is_function(struct type * p_type);
unsigned char  compiler_diagnostic(int w, struct parser_ctx * ctx, struct token * p_token, struct marker * p_marker, char * fmt, ...);

int object_set(struct parser_ctx * ctx, struct object * to, struct expression * p_init_expression, struct object * from, unsigned char   is_constant, unsigned char   requires_constant_initialization)
{
    /*try*/ if (1)
    {
        from = object_get_referenced(from);
        to->p_init_expression = p_init_expression;
        if (object_is_derived(to))
        {
            struct object * it_to;
            struct object * it_from;

            it_to = to->members;
            it_from = from->members;
            while (it_from && it_to)
            {
                object_set(ctx, it_to, 0U, it_from, is_constant, requires_constant_initialization);
                it_to = it_to->next;
                it_from = it_from->next;
            }
            if (it_from != 0U || it_to != 0U)
            {
            }
        }
        else
        {
            ;
            to->state = from->state;
            to->value = object_cast(to->value_type, from).value;
            if (requires_constant_initialization && !object_has_constant_value(from))
            {
                if (!type_is_pointer_or_array(&p_init_expression->type) && !type_is_function(&p_init_expression->type))
                {
                    struct token * tk;

                    tk = p_init_expression ? p_init_expression->first_token : ctx->current;
                    compiler_diagnostic(1860, ctx, tk, 0U, "requires a compile time object");
                    /*throw*/ goto _CKL0;
                }
            }
            if (is_constant)
            {
                if (to->state == 2 || to->state == 4)
                {
                    to->state = 2;
                }
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
    /*catch*/ else _CKL0:
    {
        return 1;
    }
    return 0;
}

void *calloc(unsigned int nmemb, unsigned int size);
char *strdup(char * src);
struct type type_dup(struct type * p_type);
struct type get_array_item_type(struct type * p_type);
int type_to_object_type(struct type * type);
struct struct_or_union_specifier *get_complete_struct_or_union_specifier(struct struct_or_union_specifier * p_struct_or_union_specifier);

struct object *make_object_ptr_core(struct type * p_type, char * name)
{
    struct object * p_object;

    p_object = 0U;
    /*try*/ if (1)
    {
        struct struct_or_union_specifier * p_struct_or_union_specifier;
        struct object * p_last_member_obj;
        struct member_declaration * p_member_declaration;

        if (p_type->category == 1)
        {
            p_object = calloc(1, 96U);
            if (p_object == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            p_object->debug_name = strdup(name);
            p_object->type = type_dup(p_type);
            return p_object;
        }
        if (p_type->category == 3)
        {
            p_object = calloc(1, 96U);
            if (p_object == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            *p_object = object_make_nullptr();
            p_object->state = 0;
            ;
            p_object->debug_name = strdup(name);
            type_destroy(&p_object->type);
            p_object->type = type_dup(p_type);
            return p_object;
        }
        if (p_type->category == 2)
        {
            p_object = calloc(1, 96U);
            if (p_object == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            p_object->type = type_dup(p_type);
            p_object->debug_name = strdup(name);
            if (p_type->num_of_elements > 0)
            {
                struct type  array_item_type;
                unsigned long long max_elements;
                struct object * p_tail_object;

                array_item_type = get_array_item_type(p_type);
                max_elements = p_type->num_of_elements > 1000 ? 1000 : p_type->num_of_elements;
                p_tail_object = 0U;
                {
                    unsigned long long i;
                    i = 0;
                    for (; i < max_elements; i++)
                    {
                        char buffer[200];
                        struct object * p_member_obj;

                        _cake_zmem(&buffer, 200);
                        snprintf(buffer, 200U, "%s[%llu]", name, i);
                        p_member_obj = make_object_ptr_core(&array_item_type, buffer);
                        if (p_member_obj == 0U)
                        {
                            type_destroy(&array_item_type);
                            /*throw*/ goto _CKL0;
                        }
                        p_member_obj->parent = p_object;
                        free(p_member_obj->debug_name);
                        p_member_obj->debug_name = strdup(buffer);
                        if (p_tail_object == 0U)
                        {
                            ;
                            p_object->members = p_member_obj;
                        }
                        else
                        {
                            ;
                            p_tail_object->next = p_member_obj;
                        }
                        p_tail_object = p_member_obj;
                    }
                }
                type_destroy(&array_item_type);
            }
            return p_object;
        }
        if (p_type->struct_or_union_specifier == 0U)
        {
            p_object = calloc(1, 96U);
            if (p_object == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            p_object->state = 0;
            p_object->value_type = type_to_object_type(p_type);
            p_object->value.signed_long_long_value = -1;
            p_object->debug_name = strdup(name);
            p_object->type = type_dup(p_type);
            return p_object;
        }
        p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
        if (p_struct_or_union_specifier == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_object = calloc(1, 96U);
        if (p_object == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_object->debug_name = strdup(name);
        p_object->type = type_dup(p_type);
        p_last_member_obj = 0U;
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
                        snprintf(buffer, 200U, "%s.%s", name, p_member_declarator->declarator->name_opt->lexeme);
                        p_member_obj = make_object_ptr_core(&p_member_declarator->declarator->type, buffer);
                        if (p_member_obj == 0U)
                        {
                            /*throw*/ goto _CKL0;
                        }
                        p_member_obj->parent = p_object;
                        free(p_member_obj->debug_name);
                        p_member_obj->debug_name = strdup(buffer);
                        if (p_object->members == 0U)
                        {
                            p_object->members = p_member_obj;
                        }
                        else
                        {
                            p_last_member_obj->next = p_member_obj;
                        }
                        p_last_member_obj = p_member_obj;
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
                        char buffer[200];
                        struct object * p_member_obj;

                        _cake_zmem(&t, 60);
                        t.category = 0;
                        t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                        t.type_specifier_flags = 32768;
                        _cake_zmem(&buffer, 200);
                        snprintf(buffer, 200U, ".%s", name);
                        p_member_obj = make_object_ptr_core(&t, buffer);
                        if (p_member_obj == 0U)
                        {
                            /*throw*/ goto _CKL0;
                        }
                        free(p_member_obj->debug_name);
                        p_member_obj->debug_name = strdup(buffer);
                        p_member_obj->parent = p_object;
                        if (p_last_member_obj == 0U)
                        {
                            ;
                            p_object->members = p_member_obj;
                        }
                        else
                        {
                            p_last_member_obj->next = p_member_obj;
                        }
                        p_last_member_obj = p_member_obj;
                        type_destroy(&t);
                    }
                }
            }
            p_member_declaration = p_member_declaration->next;
        }
        return p_object;
    }
    /*catch*/ else _CKL0:
    {
        object_delete(p_object);
        p_object = 0U;
    }
    return 0U;
}

struct object *make_object_ptr(struct type * p_type)
{
    return make_object_ptr_core(p_type, "");
}

int make_object_with_name(struct type * p_type, struct object * obj, char * name)
{
    struct object * p;

    p = make_object_ptr_core(p_type, name);
    if (p)
    {
        *obj = *p;
        object_fix_parent(obj, obj);
        free(p);
        return 0;
    }
    return 1;
}

struct object object_dup(struct object * src)
{
    struct object  result;

    ;
    result = *src;
    result.type = type_dup(&src->type);
    if (src->debug_name)
    {
        result.debug_name = strdup(src->debug_name);
    }
    result.next = 0U;
    return result;
}

int make_object(struct type * p_type, struct object * obj)
{
    return make_object_with_name(p_type, obj, "");
}

int type_specifier_to_object_type(int type_specifier_flags)
{
    if (type_specifier_flags & 512)
    {
        return 2;
    }
    if (type_specifier_flags & 32)
    {
        return 11;
    }
    if (type_specifier_flags & 64)
    {
        if (type_specifier_flags & 16)
        {
            return 13;
        }
        return 12;
    }
    if (type_specifier_flags & 256)
    {
        if (type_specifier_flags & 2)
        {
            return 4;
        }
        if (type_specifier_flags & 4)
        {
            return 6;
        }
        if (type_specifier_flags & 16)
        {
            return 8;
        }
        if (type_specifier_flags & 8)
        {
            return 1;
        }
        if (type_specifier_flags & 4194304)
        {
            return 10;
        }
    }
    else
    {
        if (type_specifier_flags & 2)
        {
            return 3;
        }
        if (type_specifier_flags & 4)
        {
            return 5;
        }
        if (type_specifier_flags & 16)
        {
            return 7;
        }
        if (type_specifier_flags & 8)
        {
            return 0;
        }
        if (type_specifier_flags & 4194304)
        {
            return 9;
        }
    }
    return 0;
}

unsigned char  type_is_pointer(struct type * p_type);

int type_to_object_type(struct type * type)
{
    if (type_is_pointer(type))
    {
        return 1;
    }
    return type_specifier_to_object_type(type->type_specifier_flags);
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

void object_print_value_debug(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int _R22 = a->value_type;
        if (_R22 == 2) goto _CKL1; /*case 2*/
        if (_R22 == 3) goto _CKL2; /*case 3*/
        if (_R22 == 4) goto _CKL3; /*case 4*/
        if (_R22 == 5) goto _CKL4; /*case 5*/
        if (_R22 == 6) goto _CKL5; /*case 6*/
        if (_R22 == 0) goto _CKL6; /*case 0*/
        if (_R22 == 1) goto _CKL7; /*case 1*/
        if (_R22 == 7) goto _CKL8; /*case 7*/
        if (_R22 == 8) goto _CKL9; /*case 8*/
        if (_R22 == 9) goto _CKL10; /*case 9*/
        if (_R22 == 10) goto _CKL11; /*case 10*/
        if (_R22 == 11) goto _CKL12; /*case 11*/
        if (_R22 == 12) goto _CKL13; /*case 12*/
        if (_R22 == 13) goto _CKL14; /*case 13*/
        if (_R22 == 14) goto _CKL15; /*case 14*/
        goto _CKL0;

        {
            /*case 2*/ _CKL1:
            printf("%s (bool)", a->value.bool_value ? "true" : "false");
            /*break*/ goto _CKL0;

            /*case 3*/ _CKL2:
            printf("%d (signed char)", (int)a->value.signed_char_value);
            /*break*/ goto _CKL0;

            /*case 4*/ _CKL3:
            printf("%d (unsigned char)", (int)a->value.unsigned_char_value);
            /*break*/ goto _CKL0;

            /*case 5*/ _CKL4:
            printf("%d (short)", a->value.signed_short_value);
            /*break*/ goto _CKL0;

            /*case 6*/ _CKL5:
            printf("%d (unsigned short)", a->value.unsigned_short_value);
            /*break*/ goto _CKL0;

            /*case 0*/ _CKL6:
            printf("%d (int)", a->value.signed_int_value);
            /*break*/ goto _CKL0;

            /*case 1*/ _CKL7:
            printf("%du (unsigned int)", a->value.unsigned_int_value);
            /*break*/ goto _CKL0;

            /*case 7*/ _CKL8:
            printf("%ld (long)", a->value.signed_long_value);
            /*break*/ goto _CKL0;

            /*case 8*/ _CKL9:
            printf("%lu (unsigned long)", a->value.unsigned_long_value);
            /*break*/ goto _CKL0;

            /*case 9*/ _CKL10:
            printf("%lld (long long)", a->value.signed_long_long_value);
            /*break*/ goto _CKL0;

            /*case 10*/ _CKL11:
            printf("%llu (unsigned long long)", a->value.unsigned_long_long_value);
            /*break*/ goto _CKL0;

            /*case 11*/ _CKL12:
            printf("%f (float)", a->value.float_value);
            /*break*/ goto _CKL0;

            /*case 12*/ _CKL13:
            printf("%lf (double)", a->value.double_value);
            /*break*/ goto _CKL0;

            /*case 13*/ _CKL14:
            printf("%Lf (long double)", a->value.long_double_value);
            /*break*/ goto _CKL0;

            /*case 14*/ _CKL15:
            printf("%p (void*)", a->value.void_pointer);
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
}

void type_print(struct type * a);

void object_print_to_debug_core(struct object * object, int n)
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
    if (object->debug_name)
    {
        printf("%s ", object->debug_name);
    }
    if (object->members != 0U)
    {
        struct object * member;

        type_print(&object->type);
        printf(" {\n");
        member = object->members;
        while (member)
        {
            object_print_to_debug_core(member, n + 1);
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
        type_print(&object->type);
        printf(" = ");
        object_print_value_debug(object);
        /*switch*/
        {
            register int _R23 = object->state;
            if (_R23 == 0) goto _CKL4; /*case 0*/
            if (_R23 == 1) goto _CKL5; /*case 1*/
            if (_R23 == 4) goto _CKL6; /*case 4*/
            if (_R23 == 2) goto _CKL7; /*case 2*/
            goto _CKL3;

            {
                /*case 0*/ _CKL4:
                printf(" uninitialized ");
                /*break*/ goto _CKL3;

                /*case 1*/ _CKL5:
                printf(" unknown ");
                /*break*/ goto _CKL3;

                /*case 4*/ _CKL6:
                printf(" exact ");
                /*break*/ goto _CKL3;

                /*case 2*/ _CKL7:
                printf(" constant_exact ");
                /*break*/ goto _CKL3;

            }
            _CKL3:;
        }
        printf("\n");
    }
}

void object_print_to_debug(struct object * object)
{
    int n;

    n = 0;
    object_print_to_debug_core(object, n);
}

struct object *object_extend_array_to_index(struct type * p_type, struct object * a, int max_index, unsigned char   is_constant)
{
    struct object * it;

    it = a->members;
    /*try*/ if (1)
    {
        int count;

        count = 0;
        while (it)
        {
            count++;
            if (it->next == 0U)
            {
                break;
            }
            it = it->next;
        }
        while (count < (max_index + 1))
        {
            if (it == 0U)
            {
                char name[100];

                ;
                a->members = make_object_ptr(p_type);
                if (a->members == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
                _cake_zmem(&name, 100);
                snprintf(name, 100U, "[%d]", count);
                free((void *)a->members->debug_name);
                a->members->debug_name = strdup(name);
                object_default_initialization(a->members, is_constant);
                it = a->members;
                it->parent = a;
                count++;
            }
            else
            {
                struct object * p;
                char name[100];

                p = make_object_ptr(p_type);
                if (p == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
                _cake_zmem(&name, 100);
                snprintf(name, 100U, "[%d]", count);
                free((void *)p->debug_name);
                p->debug_name = strdup(name);
                p->parent = a;
                object_default_initialization(p, is_constant);
                ;
                it->next = p;
                it = p;
                count++;
            }
        }
    }
    /*catch*/ else _CKL0:
    {
    }
    return it;
}

unsigned char  object_is_promoted(struct object * a)
{
    if ((a->value_type == 2) || (a->value_type == 3) || (a->value_type == 4) || (a->value_type == 5) || a->value_type == 6)
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
    if (a_type == 13 || b_type == 13)
    {
        return 13;
    }
    if (a_type == 12 || b_type == 12)
    {
        return 13;
    }
    if (a_type == 11 || b_type == 11)
    {
        return 11;
    }
    if (object_is_promoted(a))
    {
        a_type = 0;
    }
    if (object_is_promoted(b))
    {
        b_type = 0;
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

int object_greater_than_or_equal(struct object * a, struct object * b)
{
    int common_type;

    a = object_get_referenced(a);
    b = object_get_referenced(b);
    common_type = object_common(a, b);
    /*switch*/
    {
        register int _R24 = common_type;
        if (_R24 == 0) goto _CKL1; /*case 0*/
        if (_R24 == 1) goto _CKL2; /*case 1*/
        if (_R24 == 2) goto _CKL3; /*case 2*/
        if (_R24 == 3) goto _CKL4; /*case 3*/
        if (_R24 == 4) goto _CKL5; /*case 4*/
        if (_R24 == 5) goto _CKL6; /*case 5*/
        if (_R24 == 6) goto _CKL7; /*case 6*/
        if (_R24 == 7) goto _CKL8; /*case 7*/
        if (_R24 == 8) goto _CKL9; /*case 8*/
        if (_R24 == 9) goto _CKL10; /*case 9*/
        if (_R24 == 10) goto _CKL11; /*case 10*/
        if (_R24 == 11) goto _CKL12; /*case 11*/
        if (_R24 == 12) goto _CKL13; /*case 12*/
        if (_R24 == 13) goto _CKL14; /*case 13*/
        goto _CKL0;

        {
            /*case 0*/ _CKL1:
            return object_to_signed_int(a) >= object_to_signed_int(b);
            /*case 1*/ _CKL2:
            return object_to_unsigned_int(a) >= object_to_unsigned_int(b);
            /*case 2*/ _CKL3:
            return object_to_bool(a) >= object_to_bool(b);
            /*case 3*/ _CKL4:
            return object_to_signed_char(a) >= object_to_signed_char(b);
            /*break*/ goto _CKL0;

            /*case 4*/ _CKL5:
            return object_to_unsigned_char(a) >= object_to_unsigned_char(b);
            /*case 5*/ _CKL6:
            return object_to_signed_short(a) >= object_to_signed_short(b);
            /*case 6*/ _CKL7:
            return object_to_unsigned_short(a) >= object_to_unsigned_short(b);
            /*case 7*/ _CKL8:
            return object_to_signed_long(a) >= object_to_signed_long(b);
            /*case 8*/ _CKL9:
            return object_to_unsigned_long(a) >= object_to_unsigned_long(b);
            /*case 9*/ _CKL10:
            return object_to_signed_long_long(a) >= object_to_signed_long_long(b);
            /*case 10*/ _CKL11:
            return object_to_unsigned_long_long(a) >= object_to_unsigned_long_long(b);
            /*case 11*/ _CKL12:
            return object_to_float(a) >= object_to_float(b);
            /*case 12*/ _CKL13:
            return object_to_double(a) >= object_to_double(b);
            /*case 13*/ _CKL14:
            return object_to_long_double(a) >= object_to_long_double(b);
        }
        _CKL0:;
    }
    ;
    return object_to_unsigned_long_long(a) >= object_to_unsigned_long_long(b);
}

int object_smaller_than_or_equal(struct object * a, struct object * b)
{
    int common_type;

    a = object_get_referenced(a);
    b = object_get_referenced(b);
    common_type = object_common(a, b);
    /*switch*/
    {
        register int _R25 = common_type;
        if (_R25 == 0) goto _CKL1; /*case 0*/
        if (_R25 == 1) goto _CKL2; /*case 1*/
        if (_R25 == 2) goto _CKL3; /*case 2*/
        if (_R25 == 3) goto _CKL4; /*case 3*/
        if (_R25 == 4) goto _CKL5; /*case 4*/
        if (_R25 == 5) goto _CKL6; /*case 5*/
        if (_R25 == 6) goto _CKL7; /*case 6*/
        if (_R25 == 7) goto _CKL8; /*case 7*/
        if (_R25 == 8) goto _CKL9; /*case 8*/
        if (_R25 == 9) goto _CKL10; /*case 9*/
        if (_R25 == 10) goto _CKL11; /*case 10*/
        if (_R25 == 11) goto _CKL12; /*case 11*/
        if (_R25 == 12) goto _CKL13; /*case 12*/
        if (_R25 == 13) goto _CKL14; /*case 13*/
        goto _CKL0;

        {
            /*case 0*/ _CKL1:
            return object_to_signed_int(a) <= object_to_signed_int(b);
            /*case 1*/ _CKL2:
            return object_to_unsigned_int(a) <= object_to_unsigned_int(b);
            /*case 2*/ _CKL3:
            return object_to_bool(a) <= object_to_bool(b);
            /*case 3*/ _CKL4:
            return object_to_signed_char(a) <= object_to_signed_char(b);
            /*break*/ goto _CKL0;

            /*case 4*/ _CKL5:
            return object_to_unsigned_char(a) <= object_to_unsigned_char(b);
            /*case 5*/ _CKL6:
            return object_to_signed_short(a) <= object_to_signed_short(b);
            /*case 6*/ _CKL7:
            return object_to_unsigned_short(a) <= object_to_unsigned_short(b);
            /*case 7*/ _CKL8:
            return object_to_signed_long(a) <= object_to_signed_long(b);
            /*case 8*/ _CKL9:
            return object_to_unsigned_long(a) <= object_to_unsigned_long(b);
            /*case 9*/ _CKL10:
            return object_to_signed_long_long(a) <= object_to_signed_long_long(b);
            /*case 10*/ _CKL11:
            return object_to_unsigned_long_long(a) <= object_to_unsigned_long_long(b);
            /*case 11*/ _CKL12:
            return object_to_float(a) <= object_to_float(b);
            /*case 12*/ _CKL13:
            return object_to_double(a) <= object_to_double(b);
            /*case 13*/ _CKL14:
            return object_to_long_double(a) <= object_to_long_double(b);
        }
        _CKL0:;
    }
    ;
    return object_to_unsigned_long_long(a) <= object_to_unsigned_long_long(b);
}

struct object object_add(struct object * a, struct object * b)
{
    int common_type;
    struct object  o;

    a = object_get_referenced(a);
    b = object_get_referenced(b);
    common_type = object_common(a, b);
    /*switch*/
    {
        register int _R26 = common_type;
        if (_R26 == 0) goto _CKL1; /*case 0*/
        if (_R26 == 1) goto _CKL2; /*case 1*/
        if (_R26 == 2) goto _CKL3; /*case 2*/
        if (_R26 == 7) goto _CKL4; /*case 7*/
        if (_R26 == 8) goto _CKL5; /*case 8*/
        if (_R26 == 9) goto _CKL6; /*case 9*/
        if (_R26 == 10) goto _CKL7; /*case 10*/
        if (_R26 == 11) goto _CKL8; /*case 11*/
        if (_R26 == 12) goto _CKL9; /*case 12*/
        if (_R26 == 13) goto _CKL10; /*case 13*/
        goto _CKL0;

        {
            /*case 0*/ _CKL1:
            return object_make_signed_int(object_to_signed_int(a) + object_to_signed_int(b));
            /*case 1*/ _CKL2:
            return object_make_unsigned_int(object_to_unsigned_int(a) + object_to_unsigned_int(b));
            /*case 2*/ _CKL3:
            return object_make_bool(object_to_bool(a) + object_to_bool(b));
            /*case 7*/ _CKL4:
            return object_make_signed_long(object_to_signed_long(a) + object_to_signed_long(b));
            /*case 8*/ _CKL5:
            return object_make_unsigned_long(object_to_unsigned_long(a) + object_to_unsigned_long(b));
            /*case 9*/ _CKL6:
            return object_make_signed_long_long(object_to_signed_long_long(a) + object_to_signed_long_long(b));
            /*case 10*/ _CKL7:
            return object_make_unsigned_long_long(object_to_unsigned_long_long(a) + object_to_unsigned_long_long(b));
            /*case 11*/ _CKL8:
            return object_make_float(object_to_float(a) + object_to_float(b));
            /*case 12*/ _CKL9:
            return object_make_double(object_to_double(a) + object_to_double(b));
            /*case 13*/ _CKL10:
            return object_make_long_double(object_to_long_double(a) + object_to_long_double(b));
        }
        _CKL0:;
    }
    ;
    _cake_zmem(&o, 96);
    return o;
}

struct object object_sub(struct object * a, struct object * b)
{
    int common_type;
    struct object  o;

    a = object_get_referenced(a);
    b = object_get_referenced(b);
    common_type = object_common(a, b);
    /*switch*/
    {
        register int _R27 = common_type;
        if (_R27 == 0) goto _CKL1; /*case 0*/
        if (_R27 == 1) goto _CKL2; /*case 1*/
        if (_R27 == 2) goto _CKL3; /*case 2*/
        if (_R27 == 7) goto _CKL4; /*case 7*/
        if (_R27 == 8) goto _CKL5; /*case 8*/
        if (_R27 == 9) goto _CKL6; /*case 9*/
        if (_R27 == 10) goto _CKL7; /*case 10*/
        if (_R27 == 11) goto _CKL8; /*case 11*/
        if (_R27 == 12) goto _CKL9; /*case 12*/
        if (_R27 == 13) goto _CKL10; /*case 13*/
        goto _CKL0;

        {
            /*case 0*/ _CKL1:
            return object_make_signed_int(object_to_signed_int(a) - object_to_signed_int(b));
            /*case 1*/ _CKL2:
            return object_make_unsigned_int(object_to_unsigned_int(a) - object_to_unsigned_int(b));
            /*case 2*/ _CKL3:
            return object_make_bool(object_to_bool(a) - object_to_bool(b));
            /*case 7*/ _CKL4:
            return object_make_signed_long(object_to_signed_long(a) - object_to_signed_long(b));
            /*case 8*/ _CKL5:
            return object_make_unsigned_long(object_to_unsigned_long(a) - object_to_unsigned_long(b));
            /*case 9*/ _CKL6:
            return object_make_signed_long_long(object_to_signed_long_long(a) - object_to_signed_long_long(b));
            /*case 10*/ _CKL7:
            return object_make_unsigned_long_long(object_to_unsigned_long_long(a) - object_to_unsigned_long_long(b));
            /*case 11*/ _CKL8:
            return object_make_float(object_to_float(a) - object_to_float(b));
            /*case 12*/ _CKL9:
            return object_make_double(object_to_double(a) - object_to_double(b));
            /*case 13*/ _CKL10:
            return object_make_long_double(object_to_long_double(a) - object_to_long_double(b));
        }
        _CKL0:;
    }
    ;
    _cake_zmem(&o, 96);
    return o;
}

int object_equal(struct object * a, struct object * b)
{
    int common_type;

    a = object_get_referenced(a);
    b = object_get_referenced(b);
    common_type = object_common(a, b);
    /*switch*/
    {
        register int _R28 = common_type;
        if (_R28 == 0) goto _CKL1; /*case 0*/
        if (_R28 == 1) goto _CKL2; /*case 1*/
        if (_R28 == 2) goto _CKL3; /*case 2*/
        if (_R28 == 3) goto _CKL4; /*case 3*/
        if (_R28 == 4) goto _CKL5; /*case 4*/
        if (_R28 == 5) goto _CKL6; /*case 5*/
        if (_R28 == 6) goto _CKL7; /*case 6*/
        if (_R28 == 7) goto _CKL8; /*case 7*/
        if (_R28 == 8) goto _CKL9; /*case 8*/
        if (_R28 == 9) goto _CKL10; /*case 9*/
        if (_R28 == 10) goto _CKL11; /*case 10*/
        if (_R28 == 11) goto _CKL12; /*case 11*/
        if (_R28 == 12) goto _CKL13; /*case 12*/
        if (_R28 == 13) goto _CKL14; /*case 13*/
        goto _CKL0;

        {
            /*case 0*/ _CKL1:
            return object_to_signed_int(a) == object_to_signed_int(b);
            /*case 1*/ _CKL2:
            return object_to_unsigned_int(a) == object_to_unsigned_int(b);
            /*case 2*/ _CKL3:
            return object_to_bool(a) == object_to_bool(b);
            /*case 3*/ _CKL4:
            return object_to_signed_char(a) == object_to_signed_char(b);
            /*break*/ goto _CKL0;

            /*case 4*/ _CKL5:
            return object_to_unsigned_char(a) == object_to_unsigned_char(b);
            /*case 5*/ _CKL6:
            return object_to_signed_short(a) == object_to_signed_short(b);
            /*case 6*/ _CKL7:
            return object_to_unsigned_short(a) == object_to_unsigned_short(b);
            /*case 7*/ _CKL8:
            return object_to_signed_long(a) == object_to_signed_long(b);
            /*case 8*/ _CKL9:
            return object_to_unsigned_long(a) == object_to_unsigned_long(b);
            /*case 9*/ _CKL10:
            return object_to_signed_long_long(a) == object_to_signed_long_long(b);
            /*case 10*/ _CKL11:
            return object_to_unsigned_long_long(a) == object_to_unsigned_long_long(b);
            /*case 11*/ _CKL12:
            return object_to_float(a) == object_to_float(b);
            /*case 12*/ _CKL13:
            return object_to_double(a) == object_to_double(b);
            /*case 13*/ _CKL14:
            return object_to_long_double(a) == object_to_long_double(b);
        }
        _CKL0:;
    }
    ;
    return object_to_unsigned_long_long(a) == object_to_unsigned_long_long(b);
}

int object_not_equal(struct object * a, struct object * b)
{
    int common_type;

    a = object_get_referenced(a);
    b = object_get_referenced(b);
    common_type = object_common(a, b);
    /*switch*/
    {
        register int _R29 = common_type;
        if (_R29 == 0) goto _CKL1; /*case 0*/
        if (_R29 == 1) goto _CKL2; /*case 1*/
        if (_R29 == 2) goto _CKL3; /*case 2*/
        if (_R29 == 3) goto _CKL4; /*case 3*/
        if (_R29 == 4) goto _CKL5; /*case 4*/
        if (_R29 == 5) goto _CKL6; /*case 5*/
        if (_R29 == 6) goto _CKL7; /*case 6*/
        if (_R29 == 7) goto _CKL8; /*case 7*/
        if (_R29 == 8) goto _CKL9; /*case 8*/
        if (_R29 == 9) goto _CKL10; /*case 9*/
        if (_R29 == 10) goto _CKL11; /*case 10*/
        if (_R29 == 11) goto _CKL12; /*case 11*/
        if (_R29 == 12) goto _CKL13; /*case 12*/
        if (_R29 == 13) goto _CKL14; /*case 13*/
        goto _CKL0;

        {
            /*case 0*/ _CKL1:
            return object_to_signed_int(a) != object_to_signed_int(b);
            /*case 1*/ _CKL2:
            return object_to_unsigned_int(a) != object_to_unsigned_int(b);
            /*case 2*/ _CKL3:
            return object_to_bool(a) != object_to_bool(b);
            /*case 3*/ _CKL4:
            return object_to_signed_char(a) != object_to_signed_char(b);
            /*break*/ goto _CKL0;

            /*case 4*/ _CKL5:
            return object_to_unsigned_char(a) != object_to_unsigned_char(b);
            /*case 5*/ _CKL6:
            return object_to_signed_short(a) != object_to_signed_short(b);
            /*case 6*/ _CKL7:
            return object_to_unsigned_short(a) != object_to_unsigned_short(b);
            /*case 7*/ _CKL8:
            return object_to_signed_long(a) != object_to_signed_long(b);
            /*case 8*/ _CKL9:
            return object_to_unsigned_long(a) != object_to_unsigned_long(b);
            /*case 9*/ _CKL10:
            return object_to_signed_long_long(a) != object_to_signed_long_long(b);
            /*case 10*/ _CKL11:
            return object_to_unsigned_long_long(a) != object_to_unsigned_long_long(b);
            /*case 11*/ _CKL12:
            return object_to_float(a) != object_to_float(b);
            /*case 12*/ _CKL13:
            return object_to_double(a) != object_to_double(b);
            /*case 13*/ _CKL14:
            return object_to_long_double(a) != object_to_long_double(b);
        }
        _CKL0:;
    }
    ;
    return object_to_unsigned_long_long(a) != object_to_unsigned_long_long(b);
}

void objects_destroy(struct objects * arr)
{
    free(arr->items);
}

void *malloc(unsigned int size);
void *realloc(void * ptr, unsigned int size);

int objects_push(struct objects * arr, struct object * obj)
{
    if (arr->items == 0U)
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
        new_items = realloc(arr->items, new_capacity * 4U);
        if (!new_items)
        {
            return 12;
        }
        arr->items = new_items;
        arr->capacity = new_capacity;
    }
    arr->items[arr->size++] = obj;
    return 0;
}

