// Cake 0.12.05 target=x86_msvc
struct labeled_statement {
    struct label * label;
    struct statement * statement;
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

struct diagnostic {
    unsigned long long errors;
    unsigned long long warnings;
    unsigned long long notes;
};

struct param_list {
    unsigned char is_var_args;
    unsigned char is_void;
    struct param * head;
    struct param * tail;
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
    unsigned char has_static_array_size;
    unsigned char address_of;
    struct param_list  params;
    struct type * next;
};

union _struct_tag_11 {
    signed char signed_int8;
    unsigned char unsigned_int8;
    short signed_int16;
    unsigned short unsigned_int16;
    int signed_int32;
    unsigned int unsigned_int32;
    long long signed_int64;
    long long unsigned_int64;
    float float32;
    double float64;
};

struct object {
    int state;
    int value_type;
    struct type  type;
    char * debug_name;
    union _struct_tag_11  value;
    struct object * parent;
    struct object * p_ref;
    struct expression * p_init_expression;
    struct object * members;
    struct object * next;
};

struct declarator {
    unsigned char has_shared_ownership;
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
    unsigned char declarator_renamed;
};

struct typeof_specifier_argument {
    struct expression * expression;
    struct type_name * type_name;
};

struct member_declaration_list {
    struct token * first_token;
    struct token * last_token;
    struct member_declaration * head;
    struct member_declaration * tail;
};

struct designator {
    struct expression * constant_expression_opt;
    struct token * token;
    struct designator * next;
};

struct unlabeled_statement {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
    struct expression_statement * expression_statement;
    struct primary_block * primary_block;
    struct jump_statement * jump_statement;
};

struct defer_statement {
    struct token * first_token;
    struct token * last_token;
    struct secondary_block * secondary_block;
};

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

struct token_list {
    struct token * head;
    struct token * tail;
};

struct declaration_specifier {
    struct storage_class_specifier * storage_class_specifier;
    struct type_specifier_qualifier * type_specifier_qualifier;
    struct function_specifier * function_specifier;
    struct alignment_specifier * alignment_specifier;
    struct declaration_specifier * next;
};

struct typeof_specifier {
    struct token * first_token;
    struct token * last_token;
    struct typeof_specifier_argument * typeof_specifier_argument;
    struct type  type;
};

struct parameter_type_list {
    unsigned char is_var_args;
    unsigned char is_void;
    struct parameter_list * parameter_list;
};

struct member_declarator {
    struct declarator * declarator;
    struct expression * constant_expression;
    struct member_declarator * next;
};

union _struct_tag_12 {
    unsigned int number;
    struct enum_specifier * p_enum_specifier;
    struct enumerator * p_enumerator;
    struct struct_or_union_specifier * p_struct_or_union_specifier;
    struct declarator * p_declarator;
    struct init_declarator * p_init_declarator;
    struct macro * p_macro;
    struct struct_entry * p_struct_entry;
};

struct member_declarator_list {
    struct member_declarator * head;
    struct member_declarator * tail;
};

struct hash_map {
    struct map_entry ** table;
    int capacity;
    int size;
};

struct attribute {
    int msvc_declspec_flags;
    int attributes_flags;
    struct attribute_argument_clause * attribute_argument_clause;
    struct token * attribute_token;
    struct attribute * next;
};

struct static_assert_declaration {
    struct token * first_token;
    struct token * last_token;
    struct expression * constant_expression;
    struct token * string_literal_opt;
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

struct label_list_item {
    struct token * p_last_usage;
    struct token * p_defined;
    struct label_list_item * next;
};

struct pragma_declaration {
    struct token * first_token;
    struct token * last_token;
};

struct map_entry {
    struct map_entry * next;
    unsigned int hash;
    char * key;
    int type;
    union _struct_tag_12  data;
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
    int flags;
    struct type_name * type_name;
    struct expression * constant_expression;
    struct token * token;
};

struct designation {
    struct designator_list * designator_list;
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

struct diagnostic_stack {
    int top_index;
    struct diagnostic stack[10];
};

struct options {
    int input;
    int target;
    struct diagnostic_stack  diagnostic_stack;
    int style;
    unsigned char show_includes;
    unsigned char disable_assert;
    unsigned char flow_analysis;
    unsigned char test_mode;
    unsigned char null_checks_enabled;
    unsigned char ownership_enabled;
    unsigned char preprocess_only;
    unsigned char preprocess_def_macro;
    unsigned char clear_error_at_end;
    unsigned char sarif_output;
    unsigned char no_output;
    unsigned char const_literal;
    unsigned char visual_studio_ouput_format;
    unsigned char disable_colors;
    unsigned char dump_tokens;
    unsigned char dump_pptokens;
    unsigned char auto_config;
    unsigned char do_static_debug;
    int static_debug_lines;
    char output[200];
    char sarifpath[200];
};

struct init_declarator {
    unsigned char has_shared_ownership;
    struct declarator * p_declarator;
    struct initializer * initializer;
    struct init_declarator * next;
};

struct struct_or_union_specifier {
    unsigned char has_shared_ownership;
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct member_declaration_list  member_declaration_list;
    struct token * first_token;
    struct token * last_token;
    unsigned char is_owner;
    struct token * tagtoken;
    char tag_name[200];
    unsigned char has_anonymous_tag;
    unsigned char show_anonymous_tag;
    int scope_level;
    int visit_moved;
    struct struct_or_union_specifier * complete_struct_or_union_specifier_indirection;
};

struct pointer {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * calling_convention;
    struct pointer * pointer;
};

struct attribute_argument_clause {
    struct balanced_token_sequence * p_balanced_token_sequence;
    struct token * token;
};

struct parameter_list {
    struct parameter_declaration * head;
    struct parameter_declaration * tail;
};

struct try_statement {
    struct secondary_block * secondary_block;
    struct secondary_block * catch_secondary_block_opt;
    struct token * first_token;
    struct token * last_token;
    struct token * catch_token_opt;
    int catch_label_id;
};

struct diagnostic_id_stack {
    int size;
    int stack[10];
};

struct primary_block {
    struct compound_statement * compound_statement;
    struct selection_statement * selection_statement;
    struct iteration_statement * iteration_statement;
    struct defer_statement * defer_statement;
    struct try_statement * try_statement;
};

struct direct_declarator {
    struct token * name_opt;
    struct token * p_calling_convention;
    struct declarator * declarator;
    struct array_declarator * array_declarator;
    struct function_declarator * function_declarator;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
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
    struct token * offsetof_member_designator;
    struct declarator * declarator;
    struct init_declarator * p_init_declarator;
    int member_index;
    struct argument_expression_list  argument_expression_list;
    struct expression * condition_expr;
    struct expression * left;
    struct expression * right;
    unsigned char is_assignment_expression;
};

struct braced_initializer {
    struct token * first_token;
    struct token * last_token;
    struct initializer_list * initializer_list;
};

struct designator_list {
    struct designator * head;
    struct designator * tail;
};

struct initializer {
    struct token * first_token;
    struct braced_initializer * braced_initializer;
    struct expression * assignment_expression;
};

struct type_specifier_qualifier {
    struct type_specifier * type_specifier;
    struct type_qualifier * type_qualifier;
    struct alignment_specifier * alignment_specifier;
    struct type_specifier_qualifier * next;
};

struct enumerator {
    unsigned char has_shared_ownership;
    struct token * token;
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct expression * constant_expression_opt;
    struct enum_specifier * enum_specifier;
    struct enumerator * next;
    struct object  value;
};

struct initializer_list_item {
    struct designation * designation;
    struct initializer * initializer;
    struct initializer_list_item * next;
};

struct objects {
    struct object ** items;
    int size;
    int capacity;
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

struct scope {
    int scope_level;
    struct hash_map  tags;
    struct hash_map  variables;
    struct scope * next;
    struct scope * previous;
};

struct argument_expression {
    struct expression * expression;
    struct argument_expression * next;
    unsigned char set_unkown;
};

struct array_declarator {
    struct direct_declarator * direct_declarator;
    struct expression * assignment_expression;
    struct expression * expression;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * token;
    struct token * static_token_opt;
};

struct condition {
    struct expression * expression;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator * p_init_declarator;
    struct token * first_token;
    struct token * last_token;
};

struct function_specifier {
    int flags;
    struct token * token;
};

struct expression_statement {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
    struct expression * expression_opt;
};

struct init_declarator_list {
    struct init_declarator * head;
    struct init_declarator * tail;
};

struct simple_declaration {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator_list  init_declarator_list;
    struct token * first_token;
    struct token * last_token;
};

struct report {
    int no_files;
    double cpu_time_used_sec;
    int error_count;
    int warnings_count;
    int info_count;
    unsigned char test_mode;
    int test_failed;
    int test_succeeded;
    unsigned char ignore_this_report;
};

struct jump_statement {
    struct token * label;
    struct token * first_token;
    struct token * last_token;
    struct expression * expression_opt;
    int label_id;
    struct defer_list  defer_list;
};

struct type_qualifier_list {
    int flags;
    struct type_qualifier * head;
    struct type_qualifier * tail;
};

struct generic_assoc_list {
    struct generic_association * head;
    struct generic_association * tail;
};

struct attribute_specifier {
    int ack;
    struct token * first_token;
    struct token * last_token;
    struct attribute_list * attribute_list;
    struct attribute_specifier * next;
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

struct defer_list_item {
    struct declarator * declarator;
    struct defer_statement * defer_statement;
    struct defer_list_item * next;
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

struct atomic_type_specifier {
    struct token * token;
    struct type_name * type_name;
};

struct type_qualifier {
    int flags;
    struct token * token;
    struct type_qualifier * next;
};

struct scope_list {
    struct scope * head;
    struct scope * tail;
};

struct label_list {
    struct label_list_item * head;
    struct label_list_item * tail;
};

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
    unsigned char inside_generic_association;
    int label_id;
    struct report * p_report;
};

struct block_item {
    struct token * first_token;
    struct declaration * declaration;
    struct unlabeled_statement * unlabeled_statement;
    struct label * label;
    struct block_item * next;
};

struct function_declarator {
    struct direct_declarator * direct_declarator;
    struct scope  parameters_scope;
    struct parameter_type_list * parameter_type_list_opt;
};

struct initializer_list {
    struct token * first_token;
    struct initializer_list_item * head;
    struct initializer_list_item * tail;
    int size;
};

struct balanced_token {
    struct token * token;
    struct balanced_token * next;
};

struct param {
    struct type  type;
    struct param * next;
};

struct enumerator_list {
    struct enumerator * head;
    struct enumerator * tail;
};

struct enum_specifier {
    unsigned char has_shared_ownership;
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct specifier_qualifier_list * specifier_qualifier_list;
    char tag_name[200];
    struct enumerator_list  enumerator_list;
    struct token * tag_token;
    struct token * first_token;
    struct enum_specifier * p_complete_enum_specifier;
};

struct statement {
    struct labeled_statement * labeled_statement;
    struct unlabeled_statement * unlabeled_statement;
};

struct type_name {
    struct token * first_token;
    struct token * last_token;
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct declarator * abstract_declarator;
    struct type  type;
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

struct attribute_list {
    int attributes_flags;
    struct attribute * head;
    struct attribute * tail;
};

struct osstream {
    char * c_str;
    int size;
    int capacity;
};

struct init_statement {
    struct expression_statement * p_expression_statement;
    struct simple_declaration * p_simple_declaration;
};

struct parameter_declaration {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct declaration_specifiers * declaration_specifiers;
    struct declarator * declarator;
    struct parameter_declaration * next;
};

struct _iobuf {
    void * _Placeholder;
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

struct member_declaration {
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct member_declarator_list * member_declarator_list_opt;
    struct static_assert_declaration * static_assert_declaration;
    struct pragma_declaration * pragma_declaration;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
    struct member_declaration * next;
};

struct generic_selection {
    struct expression * expression;
    struct type_name * type_name;
    struct expression * p_view_selected_expression;
    struct generic_assoc_list  generic_assoc_list;
    struct token * first_token;
    struct token * last_token;
};

struct balanced_token_sequence {
    struct balanced_token * head;
    struct balanced_token * tail;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct secondary_block {
    struct token * first_token;
    struct token * last_token;
    struct statement * statement;
};

struct attribute_specifier_sequence {
    struct token * first_token;
    struct token * last_token;
    int msvc_declspec_flags;
    int attributes_flags;
    struct attribute_specifier * head;
    struct attribute_specifier * tail;
};

struct storage_class_specifier {
    int flags;
    struct token * token;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

__declspec(selectany) int _Avx2WmemEnabledWeakValue = 0;
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

unsigned char object_has_constant_value(struct object * a)
{
    a = object_get_referenced(a);
    return a->state == 2;
}


static int __cdecl snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...);
static int __cdecl vsnprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, char * _ArgList);
int __cdecl __stdio_common_vsprintf(unsigned long long _Options, char * _Buffer, unsigned int _BufferCount, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __Ck1__OptionsStorage;

void object_to_string(struct object * a, char buffer[], int sz)
{
    a = object_get_referenced(a);
    buffer[0] = 0;
    /*switch*/
    {
        register int __Ck0_temp = a->value_type;
        if (__Ck0_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck0_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck0_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck0_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck0_temp == 4) goto _CKL5; /*case 4*/
        if (__Ck0_temp == 5) goto _CKL6; /*case 5*/
        if (__Ck0_temp == 7) goto _CKL7; /*case 7*/
        if (__Ck0_temp == 8) goto _CKL8; /*case 8*/
        if (__Ck0_temp == 9) goto _CKL9; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            snprintf(buffer, sz, "%hhi", a->value.signed_int8);
            goto _CKL0; /*break*/

            _CKL2: /*case 1*/ 
            snprintf(buffer, sz, "%hhu", a->value.unsigned_int8);
            goto _CKL0; /*break*/

            _CKL3: /*case 2*/ 
            snprintf(buffer, sz, "%hi", a->value.signed_int16);
            goto _CKL0; /*break*/

            _CKL4: /*case 3*/ 
            snprintf(buffer, sz, "%hu", a->value.signed_int16);
            goto _CKL0; /*break*/

            _CKL5: /*case 4*/ 
            snprintf(buffer, sz, "%i", a->value.signed_int32);
            goto _CKL0; /*break*/

            _CKL6: /*case 5*/ 
            snprintf(buffer, sz, "%u", a->value.signed_int32);
            goto _CKL0; /*break*/

            _CKL7: /*case 7*/ 
            snprintf(buffer, sz, "%llu", a->value.signed_int64);
            goto _CKL0; /*break*/

            _CKL8: /*case 8*/ 
            snprintf(buffer, sz, "%f", a->value.float32);
            goto _CKL0; /*break*/

            _CKL9: /*case 9*/ 
            snprintf(buffer, sz, "%f", a->value.float64);
            goto _CKL0; /*break*/

        }
        _CKL0:;
    }
}



static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &__Ck1__OptionsStorage;
}

static int __cdecl vsnprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, char * _ArgList)
{
    int _Result;

    _Result = __stdio_common_vsprintf((*__local_stdio_printf_options()) | 2ULL, _Buffer, _BufferCount, _Format, 0U, _ArgList);
    return _Result < 0 ? -1 : _Result;
}

static int __cdecl snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4U));
    _Result = vsnprintf(_Buffer, _BufferCount, _Format, _ArgList);
    ((void)(_ArgList = 0U));
    return _Result;
}
struct object object_make_size_t(int target, unsigned long long value)
{
    struct object  r;

    _cake_zmem(&r, 112);
    r.state = 2;
    /*switch*/
    {
        register int __Ck2_temp = target;
        if (__Ck2_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck2_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck2_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck2_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck2_temp == 4) goto _CKL5; /*case 4*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            r.value_type = 7;
            r.value.unsigned_int64 = value;
            goto _CKL0; /*break*/

            _CKL2: /*case 1*/ 
            r.value_type = 5;
            r.value.unsigned_int32 = (unsigned int)value;
            goto _CKL0; /*break*/

            _CKL3: /*case 2*/ 
            r.value_type = 7;
            r.value.unsigned_int64 = value;
            goto _CKL0; /*break*/

            _CKL4: /*case 3*/ 
            r.value_type = 3;
            r.value.unsigned_int16 = value;
            goto _CKL0; /*break*/

            _CKL5: /*case 4*/ 
            r.value_type = 5;
            r.value.unsigned_int32 = (unsigned int)value;
            goto _CKL0; /*break*/

        }
        _CKL0:;
    }
    return r;
}


struct object object_make_nullptr(int target)
{
    struct object  r;

    _cake_zmem(&r, 112);
    r.state = 2;
    /*switch*/
    {
        register int __Ck3_temp = target;
        if (__Ck3_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck3_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck3_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck3_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck3_temp == 4) goto _CKL5; /*case 4*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            r.value_type = 7;
            r.value.unsigned_int64 = 0;
            goto _CKL0; /*break*/

            _CKL2: /*case 1*/ 
            r.value_type = 5;
            r.value.unsigned_int32 = 0;
            goto _CKL0; /*break*/

            _CKL3: /*case 2*/ 
            r.value_type = 7;
            r.value.unsigned_int64 = 0;
            goto _CKL0; /*break*/

            _CKL4: /*case 3*/ 
            r.value_type = 3;
            r.value.unsigned_int16 = 0;
            goto _CKL0; /*break*/

            _CKL5: /*case 4*/ 
            r.value_type = 5;
            r.value.unsigned_int32 = 0;
            goto _CKL0; /*break*/

        }
        _CKL0:;
    }
    return r;
}


struct object object_make_wchar_t(int target, int value)
{
    struct object  r;

    _cake_zmem(&r, 112);
    r.state = 2;
    /*switch*/
    {
        register int __Ck4_temp = target;
        if (__Ck4_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck4_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck4_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck4_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck4_temp == 4) goto _CKL5; /*case 4*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            r.value_type = 4;
            r.value.signed_int32 = value;
            goto _CKL0; /*break*/

            _CKL2: /*case 1*/ 
            _CKL3: /*case 2*/ 
            r.value_type = 3;
            r.value.unsigned_int16 = (unsigned short)value;
            goto _CKL0; /*break*/

            _CKL4: /*case 3*/ 
            r.value_type = 1;
            r.value.unsigned_int8 = value;
            goto _CKL0; /*break*/

            _CKL5: /*case 4*/ 
            r.value_type = 1;
            r.value.unsigned_int8 = value;
            goto _CKL0; /*break*/

        }
        _CKL0:;
    }
    return r;
}


struct object object_make_bool(unsigned char value)
{
    struct object  r;

    _cake_zmem(&r, 112);
    r.state = 2;
    r.value_type = 1;
    r.value.unsigned_int8 = value;
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
        register int __Ck5_temp = a->value_type;
        if (__Ck5_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck5_temp == 2) goto _CKL2; /*case 2*/
        if (__Ck5_temp == 4) goto _CKL3; /*case 4*/
        if (__Ck5_temp == 6) goto _CKL4; /*case 6*/
        if (__Ck5_temp == 1) goto _CKL5; /*case 1*/
        if (__Ck5_temp == 3) goto _CKL6; /*case 3*/
        if (__Ck5_temp == 5) goto _CKL7; /*case 5*/
        if (__Ck5_temp == 7) goto _CKL8; /*case 7*/
        if (__Ck5_temp == 8) goto _CKL9; /*case 8*/
        if (__Ck5_temp == 9) goto _CKL10; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            _CKL2: /*case 2*/ 
            _CKL3: /*case 4*/ 
            _CKL4: /*case 6*/ 
            {
                long long v;

                v = object_to_signed_long_long(a);
                snprintf(str, n, "%lld", v);
            }
            goto _CKL0; /*break*/

            _CKL5: /*case 1*/ 
            _CKL6: /*case 3*/ 
            _CKL7: /*case 5*/ 
            _CKL8: /*case 7*/ 
            {
                unsigned long long v;

                v = object_to_unsigned_long_long(a);
                snprintf(str, n, "%llu", v);
            }
            goto _CKL0; /*break*/

            _CKL9: /*case 8*/ 
            _CKL10: /*case 9*/ 
            {
                long double v;

                v = object_to_long_double(a);
                snprintf(str, n, "%Lf", v);
            }
            goto _CKL0; /*break*/

            goto _CKL0; /*break*/

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
        register int __Ck6_temp = a->value_type;
        if (__Ck6_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck6_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck6_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck6_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck6_temp == 4) goto _CKL5; /*case 4*/
        if (__Ck6_temp == 5) goto _CKL6; /*case 5*/
        if (__Ck6_temp == 6) goto _CKL7; /*case 6*/
        if (__Ck6_temp == 7) goto _CKL8; /*case 7*/
        if (__Ck6_temp == 8) goto _CKL9; /*case 8*/
        if (__Ck6_temp == 9) goto _CKL10; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            a->value.signed_int8 = value;
            goto _CKL0; /*break*/

            _CKL2: /*case 1*/ 
            a->value.unsigned_int8 = value;
            goto _CKL0; /*break*/

            _CKL3: /*case 2*/ 
            a->value.signed_int16 = value;
            goto _CKL0; /*break*/

            _CKL4: /*case 3*/ 
            a->value.unsigned_int16 = value;
            goto _CKL0; /*break*/

            _CKL5: /*case 4*/ 
            a->value.signed_int32 = value;
            goto _CKL0; /*break*/

            _CKL6: /*case 5*/ 
            a->value.unsigned_int32 = value;
            goto _CKL0; /*break*/

            _CKL7: /*case 6*/ 
            a->value.signed_int64 = value;
            goto _CKL0; /*break*/

            _CKL8: /*case 7*/ 
            a->value.unsigned_int64 = value;
            goto _CKL0; /*break*/

            _CKL9: /*case 8*/ 
            a->value.float32 = value;
            goto _CKL0; /*break*/

            _CKL10: /*case 9*/ 
            a->value.float64 = value;
            goto _CKL0; /*break*/

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
        register int __Ck7_temp = a->value_type;
        if (__Ck7_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck7_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck7_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck7_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck7_temp == 4) goto _CKL5; /*case 4*/
        if (__Ck7_temp == 5) goto _CKL6; /*case 5*/
        if (__Ck7_temp == 6) goto _CKL7; /*case 6*/
        if (__Ck7_temp == 7) goto _CKL8; /*case 7*/
        if (__Ck7_temp == 8) goto _CKL9; /*case 8*/
        if (__Ck7_temp == 9) goto _CKL10; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            a->value.signed_int8 = value;
            goto _CKL0; /*break*/

            _CKL2: /*case 1*/ 
            a->value.unsigned_int8 = value;
            goto _CKL0; /*break*/

            _CKL3: /*case 2*/ 
            a->value.signed_int16 = value;
            goto _CKL0; /*break*/

            _CKL4: /*case 3*/ 
            a->value.unsigned_int16 = value;
            goto _CKL0; /*break*/

            _CKL5: /*case 4*/ 
            a->value.signed_int32 = value;
            goto _CKL0; /*break*/

            _CKL6: /*case 5*/ 
            a->value.unsigned_int32 = value;
            goto _CKL0; /*break*/

            _CKL7: /*case 6*/ 
            a->value.signed_int64 = value;
            goto _CKL0; /*break*/

            _CKL8: /*case 7*/ 
            a->value.unsigned_int64 = value;
            goto _CKL0; /*break*/

            _CKL9: /*case 8*/ 
            a->value.float32 = value;
            goto _CKL0; /*break*/

            _CKL10: /*case 9*/ 
            a->value.float64 = value;
            goto _CKL0; /*break*/

        }
        _CKL0:;
    }
}


unsigned char object_to_bool(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int __Ck8_temp = a->value_type;
        if (__Ck8_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck8_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck8_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck8_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck8_temp == 4) goto _CKL5; /*case 4*/
        if (__Ck8_temp == 5) goto _CKL6; /*case 5*/
        if (__Ck8_temp == 6) goto _CKL7; /*case 6*/
        if (__Ck8_temp == 7) goto _CKL8; /*case 7*/
        if (__Ck8_temp == 8) goto _CKL9; /*case 8*/
        if (__Ck8_temp == 9) goto _CKL10; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            return (a->value.signed_int8 != 0);
            _CKL2: /*case 1*/ 
            return (a->value.unsigned_int8 != 0);
            _CKL3: /*case 2*/ 
            return (a->value.signed_int16 != 0);
            _CKL4: /*case 3*/ 
            return (a->value.unsigned_int16 != 0);
            _CKL5: /*case 4*/ 
            return (a->value.signed_int32 != 0);
            _CKL6: /*case 5*/ 
            return (a->value.unsigned_int32 != 0);
            _CKL7: /*case 6*/ 
            return (a->value.signed_int64 != 0);
            _CKL8: /*case 7*/ 
            return (a->value.unsigned_int64 != 0);
            _CKL9: /*case 8*/ 
            return (a->value.float32 != 0);
            _CKL10: /*case 9*/ 
            return (a->value.float64 != 0);
        }
        _CKL0:;
    }
    ;
    return 0;
}


struct object object_make_signed_char(signed char value)
{
    struct object  r;

    _cake_zmem(&r, 112);
    r.state = 2;
    r.value_type = 0;
    r.value.signed_int8 = value;
    return r;
}


int object_increment_value(struct object * a)
{
    a = object_get_non_const_referenced(a);
    /*switch*/
    {
        register int __Ck9_temp = a->value_type;
        if (__Ck9_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck9_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck9_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck9_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck9_temp == 4) goto _CKL5; /*case 4*/
        if (__Ck9_temp == 5) goto _CKL6; /*case 5*/
        if (__Ck9_temp == 6) goto _CKL7; /*case 6*/
        if (__Ck9_temp == 7) goto _CKL8; /*case 7*/
        if (__Ck9_temp == 8) goto _CKL9; /*case 8*/
        if (__Ck9_temp == 9) goto _CKL10; /*case 9*/
        goto _CKL11;/*default*/

        {
            _CKL1: /*case 0*/ 
            a->value.signed_int8++;
            goto _CKL0; /*break*/

            _CKL2: /*case 1*/ 
            a->value.unsigned_int8++;
            goto _CKL0; /*break*/

            _CKL3: /*case 2*/ 
            a->value.signed_int16++;
            goto _CKL0; /*break*/

            _CKL4: /*case 3*/ 
            a->value.unsigned_int16++;
            goto _CKL0; /*break*/

            _CKL5: /*case 4*/ 
            a->value.signed_int32++;
            goto _CKL0; /*break*/

            _CKL6: /*case 5*/ 
            a->value.unsigned_int32++;
            goto _CKL0; /*break*/

            _CKL7: /*case 6*/ 
            a->value.signed_int64++;
            goto _CKL0; /*break*/

            _CKL8: /*case 7*/ 
            a->value.unsigned_int64++;
            goto _CKL0; /*break*/

            _CKL9: /*case 8*/ 
            a->value.float32++;
            goto _CKL0; /*break*/

            _CKL10: /*case 9*/ 
            a->value.float64++;
            goto _CKL0; /*break*/

            _CKL11: /*default*/ 
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
        register int __Ck10_temp = a->value_type;
        if (__Ck10_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck10_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck10_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck10_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck10_temp == 4) goto _CKL5; /*case 4*/
        if (__Ck10_temp == 5) goto _CKL6; /*case 5*/
        if (__Ck10_temp == 6) goto _CKL7; /*case 6*/
        if (__Ck10_temp == 7) goto _CKL8; /*case 7*/
        if (__Ck10_temp == 8) goto _CKL9; /*case 8*/
        if (__Ck10_temp == 9) goto _CKL10; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            return a->value.signed_int8;
            _CKL2: /*case 1*/ 
            return a->value.unsigned_int8;
            _CKL3: /*case 2*/ 
            return a->value.signed_int16;
            _CKL4: /*case 3*/ 
            return a->value.unsigned_int16;
            _CKL5: /*case 4*/ 
            return a->value.signed_int32;
            _CKL6: /*case 5*/ 
            return a->value.unsigned_int32;
            _CKL7: /*case 6*/ 
            return a->value.signed_int64;
            _CKL8: /*case 7*/ 
            return a->value.unsigned_int64;
            _CKL9: /*case 8*/ 
            return a->value.float32;
            _CKL10: /*case 9*/ 
            return a->value.float64;
        }
        _CKL0:;
    }
    ;
    return 0;
}


struct object object_make_unsigned_char(unsigned char value)
{
    struct object  r;

    _cake_zmem(&r, 112);
    r.state = 2;
    r.value_type = 1;
    r.value.unsigned_int8 = value;
    return r;
}


unsigned char object_to_unsigned_char(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int __Ck11_temp = a->value_type;
        if (__Ck11_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck11_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck11_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck11_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck11_temp == 4) goto _CKL5; /*case 4*/
        if (__Ck11_temp == 5) goto _CKL6; /*case 5*/
        if (__Ck11_temp == 6) goto _CKL7; /*case 6*/
        if (__Ck11_temp == 7) goto _CKL8; /*case 7*/
        if (__Ck11_temp == 8) goto _CKL9; /*case 8*/
        if (__Ck11_temp == 9) goto _CKL10; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            return a->value.signed_int8;
            _CKL2: /*case 1*/ 
            return a->value.unsigned_int8;
            _CKL3: /*case 2*/ 
            return a->value.signed_int16;
            _CKL4: /*case 3*/ 
            return a->value.unsigned_int16;
            _CKL5: /*case 4*/ 
            return a->value.signed_int32;
            _CKL6: /*case 5*/ 
            return a->value.unsigned_int32;
            _CKL7: /*case 6*/ 
            return a->value.signed_int64;
            _CKL8: /*case 7*/ 
            return a->value.unsigned_int64;
            _CKL9: /*case 8*/ 
            return a->value.float32;
            _CKL10: /*case 9*/ 
            return a->value.float64;
        }
        _CKL0:;
    }
    ;
    return 0;
}


struct object object_make_signed_short(signed short value)
{
    struct object  r;

    _cake_zmem(&r, 112);
    r.state = 2;
    r.value_type = 2;
    r.value.signed_int16 = value;
    return r;
}


signed short object_to_signed_short(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int __Ck12_temp = a->value_type;
        if (__Ck12_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck12_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck12_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck12_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck12_temp == 4) goto _CKL5; /*case 4*/
        if (__Ck12_temp == 5) goto _CKL6; /*case 5*/
        if (__Ck12_temp == 6) goto _CKL7; /*case 6*/
        if (__Ck12_temp == 7) goto _CKL8; /*case 7*/
        if (__Ck12_temp == 8) goto _CKL9; /*case 8*/
        if (__Ck12_temp == 9) goto _CKL10; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            return a->value.signed_int8;
            _CKL2: /*case 1*/ 
            return a->value.unsigned_int8;
            _CKL3: /*case 2*/ 
            return a->value.signed_int16;
            _CKL4: /*case 3*/ 
            return a->value.unsigned_int16;
            _CKL5: /*case 4*/ 
            return a->value.signed_int32;
            _CKL6: /*case 5*/ 
            return a->value.unsigned_int32;
            _CKL7: /*case 6*/ 
            return a->value.signed_int64;
            _CKL8: /*case 7*/ 
            return a->value.unsigned_int64;
            _CKL9: /*case 8*/ 
            return a->value.float32;
            _CKL10: /*case 9*/ 
            return a->value.float64;
        }
        _CKL0:;
    }
    ;
    return 0;
}


struct object object_make_unsigned_short(unsigned short value)
{
    struct object  r;

    _cake_zmem(&r, 112);
    r.state = 2;
    r.value_type = 3;
    r.value.unsigned_int16 = value;
    return r;
}


unsigned short object_to_unsigned_short(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int __Ck13_temp = a->value_type;
        if (__Ck13_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck13_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck13_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck13_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck13_temp == 4) goto _CKL5; /*case 4*/
        if (__Ck13_temp == 5) goto _CKL6; /*case 5*/
        if (__Ck13_temp == 6) goto _CKL7; /*case 6*/
        if (__Ck13_temp == 7) goto _CKL8; /*case 7*/
        if (__Ck13_temp == 8) goto _CKL9; /*case 8*/
        if (__Ck13_temp == 9) goto _CKL10; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            return a->value.signed_int8;
            _CKL2: /*case 1*/ 
            return a->value.unsigned_int8;
            _CKL3: /*case 2*/ 
            return a->value.signed_int16;
            _CKL4: /*case 3*/ 
            return a->value.unsigned_int16;
            _CKL5: /*case 4*/ 
            return a->value.signed_int32;
            _CKL6: /*case 5*/ 
            return a->value.unsigned_int32;
            _CKL7: /*case 6*/ 
            return a->value.signed_int64;
            _CKL8: /*case 7*/ 
            return a->value.unsigned_int64;
            _CKL9: /*case 8*/ 
            return a->value.float32;
            _CKL10: /*case 9*/ 
            return a->value.float64;
        }
        _CKL0:;
    }
    ;
    return 0;
}


struct object object_make_signed_int(signed int value)
{
    struct object  r;

    _cake_zmem(&r, 112);
    r.state = 2;
    r.value_type = 4;
    r.value.signed_int32 = value;
    return r;
}


signed int object_to_signed_int(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int __Ck14_temp = a->value_type;
        if (__Ck14_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck14_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck14_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck14_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck14_temp == 4) goto _CKL5; /*case 4*/
        if (__Ck14_temp == 5) goto _CKL6; /*case 5*/
        if (__Ck14_temp == 6) goto _CKL7; /*case 6*/
        if (__Ck14_temp == 7) goto _CKL8; /*case 7*/
        if (__Ck14_temp == 8) goto _CKL9; /*case 8*/
        if (__Ck14_temp == 9) goto _CKL10; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            return a->value.signed_int8;
            _CKL2: /*case 1*/ 
            return a->value.unsigned_int8;
            _CKL3: /*case 2*/ 
            return a->value.signed_int16;
            _CKL4: /*case 3*/ 
            return a->value.unsigned_int16;
            _CKL5: /*case 4*/ 
            return a->value.signed_int32;
            _CKL6: /*case 5*/ 
            return a->value.unsigned_int32;
            _CKL7: /*case 6*/ 
            return a->value.signed_int64;
            _CKL8: /*case 7*/ 
            return a->value.unsigned_int64;
            _CKL9: /*case 8*/ 
            return a->value.float32;
            _CKL10: /*case 9*/ 
            return a->value.float64;
        }
        _CKL0:;
    }
    ;
    return 0;
}


struct object object_make_unsigned_int(unsigned int value)
{
    struct object  r;

    _cake_zmem(&r, 112);
    r.state = 2;
    r.value_type = 5;
    r.value.unsigned_int32 = value;
    return r;
}


unsigned int object_to_unsigned_int(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int __Ck15_temp = a->value_type;
        if (__Ck15_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck15_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck15_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck15_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck15_temp == 4) goto _CKL5; /*case 4*/
        if (__Ck15_temp == 5) goto _CKL6; /*case 5*/
        if (__Ck15_temp == 6) goto _CKL7; /*case 6*/
        if (__Ck15_temp == 7) goto _CKL8; /*case 7*/
        if (__Ck15_temp == 8) goto _CKL9; /*case 8*/
        if (__Ck15_temp == 9) goto _CKL10; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            return a->value.signed_int8;
            _CKL2: /*case 1*/ 
            return a->value.unsigned_int8;
            _CKL3: /*case 2*/ 
            return a->value.signed_int16;
            _CKL4: /*case 3*/ 
            return a->value.unsigned_int16;
            _CKL5: /*case 4*/ 
            return a->value.signed_int32;
            _CKL6: /*case 5*/ 
            return a->value.unsigned_int32;
            _CKL7: /*case 6*/ 
            return a->value.signed_int64;
            _CKL8: /*case 7*/ 
            return a->value.unsigned_int64;
            _CKL9: /*case 8*/ 
            return a->value.float32;
            _CKL10: /*case 9*/ 
            return a->value.float64;
        }
        _CKL0:;
    }
    ;
    return 0;
}


struct object object_make_signed_long(signed long long value, int target)
{
    struct object  r;

    _cake_zmem(&r, 112);
    r.state = 2;
    /*switch*/
    {
        register int __Ck16_temp = target;
        if (__Ck16_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck16_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck16_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck16_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck16_temp == 4) goto _CKL5; /*case 4*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            r.value_type = 6;
            r.value.signed_int64 = value;
            goto _CKL0; /*break*/

            _CKL2: /*case 1*/ 
            _CKL3: /*case 2*/ 
            r.value_type = 4;
            r.value.signed_int32 = value;
            goto _CKL0; /*break*/

            _CKL4: /*case 3*/ 
            r.value_type = 4;
            r.value.signed_int32 = value;
            goto _CKL0; /*break*/

            _CKL5: /*case 4*/ 
            r.value_type = 4;
            r.value.signed_int32 = value;
            goto _CKL0; /*break*/

        }
        _CKL0:;
    }
    return r;
}


signed long object_to_signed_long(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int __Ck17_temp = a->value_type;
        if (__Ck17_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck17_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck17_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck17_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck17_temp == 4) goto _CKL5; /*case 4*/
        if (__Ck17_temp == 5) goto _CKL6; /*case 5*/
        if (__Ck17_temp == 6) goto _CKL7; /*case 6*/
        if (__Ck17_temp == 7) goto _CKL8; /*case 7*/
        if (__Ck17_temp == 8) goto _CKL9; /*case 8*/
        if (__Ck17_temp == 9) goto _CKL10; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            return a->value.signed_int8;
            _CKL2: /*case 1*/ 
            return a->value.unsigned_int8;
            _CKL3: /*case 2*/ 
            return a->value.signed_int16;
            _CKL4: /*case 3*/ 
            return a->value.unsigned_int16;
            _CKL5: /*case 4*/ 
            return a->value.signed_int32;
            _CKL6: /*case 5*/ 
            return a->value.unsigned_int32;
            _CKL7: /*case 6*/ 
            return a->value.signed_int64;
            _CKL8: /*case 7*/ 
            return a->value.unsigned_int64;
            _CKL9: /*case 8*/ 
            return a->value.float32;
            _CKL10: /*case 9*/ 
            return a->value.float64;
        }
        _CKL0:;
    }
    ;
    return 0;
}


struct object object_make_unsigned_long(unsigned long long value, int target)
{
    struct object  r;

    _cake_zmem(&r, 112);
    r.state = 2;
    /*switch*/
    {
        register int __Ck18_temp = target;
        if (__Ck18_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck18_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck18_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck18_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck18_temp == 4) goto _CKL5; /*case 4*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            r.value_type = 7;
            r.value.unsigned_int64 = value;
            goto _CKL0; /*break*/

            _CKL2: /*case 1*/ 
            _CKL3: /*case 2*/ 
            r.value_type = 5;
            r.value.unsigned_int32 = value;
            goto _CKL0; /*break*/

            _CKL4: /*case 3*/ 
            r.value_type = 5;
            r.value.unsigned_int32 = value;
            goto _CKL0; /*break*/

            _CKL5: /*case 4*/ 
            r.value_type = 5;
            r.value.unsigned_int32 = value;
            goto _CKL0; /*break*/

        }
        _CKL0:;
    }
    return r;
}


unsigned long object_to_unsigned_long(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int __Ck19_temp = a->value_type;
        if (__Ck19_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck19_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck19_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck19_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck19_temp == 4) goto _CKL5; /*case 4*/
        if (__Ck19_temp == 5) goto _CKL6; /*case 5*/
        if (__Ck19_temp == 6) goto _CKL7; /*case 6*/
        if (__Ck19_temp == 7) goto _CKL8; /*case 7*/
        if (__Ck19_temp == 8) goto _CKL9; /*case 8*/
        if (__Ck19_temp == 9) goto _CKL10; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            return a->value.signed_int8;
            _CKL2: /*case 1*/ 
            return a->value.unsigned_int8;
            _CKL3: /*case 2*/ 
            return a->value.signed_int16;
            _CKL4: /*case 3*/ 
            return a->value.unsigned_int16;
            _CKL5: /*case 4*/ 
            return a->value.signed_int32;
            _CKL6: /*case 5*/ 
            return a->value.unsigned_int32;
            _CKL7: /*case 6*/ 
            return a->value.signed_int64;
            _CKL8: /*case 7*/ 
            return a->value.unsigned_int64;
            _CKL9: /*case 8*/ 
            return a->value.float32;
            _CKL10: /*case 9*/ 
            return a->value.float64;
        }
        _CKL0:;
    }
    ;
    return 0;
}


struct object object_make_signed_long_long(signed long long value)
{
    struct object  r;

    _cake_zmem(&r, 112);
    r.state = 2;
    r.value_type = 6;
    r.value.signed_int64 = value;
    return r;
}


signed long long object_to_signed_long_long(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int __Ck20_temp = a->value_type;
        if (__Ck20_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck20_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck20_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck20_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck20_temp == 4) goto _CKL5; /*case 4*/
        if (__Ck20_temp == 5) goto _CKL6; /*case 5*/
        if (__Ck20_temp == 6) goto _CKL7; /*case 6*/
        if (__Ck20_temp == 7) goto _CKL8; /*case 7*/
        if (__Ck20_temp == 8) goto _CKL9; /*case 8*/
        if (__Ck20_temp == 9) goto _CKL10; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            return a->value.signed_int8;
            _CKL2: /*case 1*/ 
            return a->value.unsigned_int8;
            _CKL3: /*case 2*/ 
            return a->value.signed_int16;
            _CKL4: /*case 3*/ 
            return a->value.unsigned_int16;
            _CKL5: /*case 4*/ 
            return a->value.signed_int32;
            _CKL6: /*case 5*/ 
            return a->value.unsigned_int32;
            _CKL7: /*case 6*/ 
            return a->value.signed_int64;
            _CKL8: /*case 7*/ 
            return a->value.unsigned_int64;
            _CKL9: /*case 8*/ 
            return a->value.float32;
            _CKL10: /*case 9*/ 
            return a->value.float64;
        }
        _CKL0:;
    }
    ;
    return 0;
}


struct object object_make_unsigned_long_long(unsigned long long value)
{
    struct object  r;

    _cake_zmem(&r, 112);
    r.state = 2;
    r.value_type = 7;
    r.value.unsigned_int64 = value;
    return r;
}


unsigned long long object_to_unsigned_long_long(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int __Ck21_temp = a->value_type;
        if (__Ck21_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck21_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck21_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck21_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck21_temp == 4) goto _CKL5; /*case 4*/
        if (__Ck21_temp == 5) goto _CKL6; /*case 5*/
        if (__Ck21_temp == 6) goto _CKL7; /*case 6*/
        if (__Ck21_temp == 7) goto _CKL8; /*case 7*/
        if (__Ck21_temp == 8) goto _CKL9; /*case 8*/
        if (__Ck21_temp == 9) goto _CKL10; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            return a->value.signed_int8;
            _CKL2: /*case 1*/ 
            return a->value.unsigned_int8;
            _CKL3: /*case 2*/ 
            return a->value.signed_int16;
            _CKL4: /*case 3*/ 
            return a->value.unsigned_int16;
            _CKL5: /*case 4*/ 
            return a->value.signed_int32;
            _CKL6: /*case 5*/ 
            return a->value.unsigned_int32;
            _CKL7: /*case 6*/ 
            return a->value.signed_int64;
            _CKL8: /*case 7*/ 
            return a->value.unsigned_int64;
            _CKL9: /*case 8*/ 
            return a->value.float32;
            _CKL10: /*case 9*/ 
            return a->value.float64;
        }
        _CKL0:;
    }
    ;
    return 0;
}


struct object object_make_float(float value)
{
    struct object  r;

    _cake_zmem(&r, 112);
    r.state = 2;
    r.value_type = 8;
    r.value.float32 = value;
    return r;
}


float object_to_float(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int __Ck22_temp = a->value_type;
        if (__Ck22_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck22_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck22_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck22_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck22_temp == 4) goto _CKL5; /*case 4*/
        if (__Ck22_temp == 5) goto _CKL6; /*case 5*/
        if (__Ck22_temp == 6) goto _CKL7; /*case 6*/
        if (__Ck22_temp == 7) goto _CKL8; /*case 7*/
        if (__Ck22_temp == 8) goto _CKL9; /*case 8*/
        if (__Ck22_temp == 9) goto _CKL10; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            return a->value.signed_int8;
            _CKL2: /*case 1*/ 
            return a->value.unsigned_int8;
            _CKL3: /*case 2*/ 
            return a->value.signed_int16;
            _CKL4: /*case 3*/ 
            return a->value.unsigned_int16;
            _CKL5: /*case 4*/ 
            return a->value.signed_int32;
            _CKL6: /*case 5*/ 
            return a->value.unsigned_int32;
            _CKL7: /*case 6*/ 
            return a->value.signed_int64;
            _CKL8: /*case 7*/ 
            return a->value.unsigned_int64;
            _CKL9: /*case 8*/ 
            return a->value.float32;
            _CKL10: /*case 9*/ 
            return a->value.float64;
        }
        _CKL0:;
    }
    ;
    return 0;
}


struct object object_make_double(double value)
{
    struct object  r;

    _cake_zmem(&r, 112);
    r.state = 2;
    r.value_type = 9;
    r.value.float64 = value;
    return r;
}


double object_to_double(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int __Ck23_temp = a->value_type;
        if (__Ck23_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck23_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck23_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck23_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck23_temp == 4) goto _CKL5; /*case 4*/
        if (__Ck23_temp == 5) goto _CKL6; /*case 5*/
        if (__Ck23_temp == 6) goto _CKL7; /*case 6*/
        if (__Ck23_temp == 7) goto _CKL8; /*case 7*/
        if (__Ck23_temp == 8) goto _CKL9; /*case 8*/
        if (__Ck23_temp == 9) goto _CKL10; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            return a->value.signed_int8;
            _CKL2: /*case 1*/ 
            return a->value.unsigned_int8;
            _CKL3: /*case 2*/ 
            return a->value.signed_int16;
            _CKL4: /*case 3*/ 
            return a->value.unsigned_int16;
            _CKL5: /*case 4*/ 
            return a->value.signed_int32;
            _CKL6: /*case 5*/ 
            return a->value.unsigned_int32;
            _CKL7: /*case 6*/ 
            return a->value.signed_int64;
            _CKL8: /*case 7*/ 
            return a->value.unsigned_int64;
            _CKL9: /*case 8*/ 
            return a->value.float32;
            _CKL10: /*case 9*/ 
            return a->value.float64;
        }
        _CKL0:;
    }
    ;
    return 0;
}


struct object object_make_pointer(struct object * object)
{
    struct object  r;

    object = object_get_non_const_referenced(object);
    _cake_zmem(&r, 112);
    r.state = 4;
    r.value_type = 7;
    r.value.unsigned_int64 = (unsigned long long)object;
    return r;
}


struct object object_make_reference(struct object * object)
{
    struct object  r;

    object = object_get_non_const_referenced(object);
    _cake_zmem(&r, 112);
    r.state = 4;
    r.value_type = 7;
    r.value.unsigned_int64 = (unsigned long long)object;
    r.p_ref = object;
    return r;
}


struct object object_make_long_double(long double value)
{
    struct object  r;

    _cake_zmem(&r, 112);
    r.state = 2;
    r.value_type = 9;
    r.value.float64 = value;
    return r;
}


long double object_to_long_double(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int __Ck24_temp = a->value_type;
        if (__Ck24_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck24_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck24_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck24_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck24_temp == 4) goto _CKL5; /*case 4*/
        if (__Ck24_temp == 5) goto _CKL6; /*case 5*/
        if (__Ck24_temp == 6) goto _CKL7; /*case 6*/
        if (__Ck24_temp == 7) goto _CKL8; /*case 7*/
        if (__Ck24_temp == 8) goto _CKL9; /*case 8*/
        if (__Ck24_temp == 9) goto _CKL10; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            return a->value.signed_int8;
            _CKL2: /*case 1*/ 
            return a->value.unsigned_int8;
            _CKL3: /*case 2*/ 
            return a->value.signed_int16;
            _CKL4: /*case 3*/ 
            return a->value.unsigned_int16;
            _CKL5: /*case 4*/ 
            return a->value.signed_int32;
            _CKL6: /*case 5*/ 
            return a->value.unsigned_int32;
            _CKL7: /*case 6*/ 
            return a->value.signed_int64;
            _CKL8: /*case 7*/ 
            return a->value.unsigned_int64;
            _CKL9: /*case 8*/ 
            return a->value.float32;
            _CKL10: /*case 9*/ 
            return a->value.float64;
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
    if (t == 0)
    {
        if (v->value_type == 1)
        {
            return object_make_signed_char((signed char)v->value.unsigned_int8);
        }
        if (v->value_type == 2)
        {
            return object_make_signed_char((signed char)v->value.signed_int16);
        }
        if (v->value_type == 3)
        {
            return object_make_signed_char((signed char)v->value.unsigned_int16);
        }
        if (v->value_type == 4)
        {
            return object_make_signed_char((signed char)v->value.signed_int32);
        }
        if (v->value_type == 5)
        {
            return object_make_signed_char((signed char)v->value.unsigned_int32);
        }
        if (v->value_type == 6)
        {
            return object_make_signed_char((signed char)v->value.signed_int64);
        }
        if (v->value_type == 7)
        {
            return object_make_signed_char((signed char)v->value.unsigned_int64);
        }
        if (v->value_type == 8)
        {
            return object_make_signed_char((signed char)v->value.float32);
        }
        if (v->value_type == 9)
        {
            return object_make_signed_char((signed char)v->value.float64);
        }
    }
    if (t == 1)
    {
        if (v->value_type == 0)
        {
            return object_make_unsigned_char((unsigned char)v->value.signed_int8);
        }
        if (v->value_type == 2)
        {
            return object_make_unsigned_char((unsigned char)v->value.signed_int16);
        }
        if (v->value_type == 3)
        {
            return object_make_unsigned_char((unsigned char)v->value.unsigned_int16);
        }
        if (v->value_type == 4)
        {
            return object_make_unsigned_char((unsigned char)v->value.signed_int32);
        }
        if (v->value_type == 5)
        {
            return object_make_unsigned_char((unsigned char)v->value.unsigned_int32);
        }
        if (v->value_type == 6)
        {
            return object_make_unsigned_char((unsigned char)v->value.signed_int64);
        }
        if (v->value_type == 7)
        {
            return object_make_unsigned_char((unsigned char)v->value.unsigned_int64);
        }
        if (v->value_type == 8)
        {
            return object_make_unsigned_char((unsigned char)v->value.float32);
        }
        if (v->value_type == 9)
        {
            return object_make_unsigned_char((unsigned char)v->value.float64);
        }
    }
    if (t == 2)
    {
        if (v->value_type == 0)
        {
            return object_make_signed_short((signed short)v->value.signed_int8);
        }
        if (v->value_type == 1)
        {
            return object_make_signed_short((signed short)v->value.unsigned_int8);
        }
        if (v->value_type == 3)
        {
            return object_make_signed_short((signed short)v->value.unsigned_int16);
        }
        if (v->value_type == 4)
        {
            return object_make_signed_short((signed short)v->value.signed_int32);
        }
        if (v->value_type == 5)
        {
            return object_make_signed_short((signed short)v->value.unsigned_int32);
        }
        if (v->value_type == 6)
        {
            return object_make_signed_short((signed short)v->value.signed_int64);
        }
        if (v->value_type == 7)
        {
            return object_make_signed_short((signed short)v->value.unsigned_int64);
        }
        if (v->value_type == 8)
        {
            return object_make_signed_short((signed short)v->value.float32);
        }
        if (v->value_type == 9)
        {
            return object_make_signed_short((signed short)v->value.float64);
        }
    }
    if (t == 3)
    {
        if (v->value_type == 0)
        {
            return object_make_unsigned_short((unsigned short)v->value.signed_int8);
        }
        if (v->value_type == 1)
        {
            return object_make_unsigned_short((unsigned short)v->value.unsigned_int8);
        }
        if (v->value_type == 2)
        {
            return object_make_unsigned_short((unsigned short)v->value.signed_int16);
        }
        if (v->value_type == 4)
        {
            return object_make_unsigned_short((unsigned short)v->value.signed_int32);
        }
        if (v->value_type == 5)
        {
            return object_make_unsigned_short((unsigned short)v->value.unsigned_int32);
        }
        if (v->value_type == 6)
        {
            return object_make_unsigned_short((unsigned short)v->value.signed_int64);
        }
        if (v->value_type == 7)
        {
            return object_make_unsigned_short((unsigned short)v->value.unsigned_int64);
        }
        if (v->value_type == 8)
        {
            return object_make_unsigned_short((unsigned short)v->value.float32);
        }
        if (v->value_type == 9)
        {
            return object_make_unsigned_short((unsigned short)v->value.float64);
        }
    }
    if (t == 4)
    {
        if (v->value_type == 0)
        {
            return object_make_signed_int((signed int)v->value.signed_int8);
        }
        if (v->value_type == 1)
        {
            return object_make_signed_int((signed int)v->value.unsigned_int8);
        }
        if (v->value_type == 2)
        {
            return object_make_signed_int((signed int)v->value.signed_int16);
        }
        if (v->value_type == 3)
        {
            return object_make_signed_int((signed int)v->value.unsigned_int16);
        }
        if (v->value_type == 5)
        {
            return object_make_signed_int((signed int)v->value.unsigned_int32);
        }
        if (v->value_type == 6)
        {
            return object_make_signed_int((signed int)v->value.signed_int64);
        }
        if (v->value_type == 7)
        {
            return object_make_signed_int((signed int)v->value.unsigned_int64);
        }
        if (v->value_type == 8)
        {
            return object_make_signed_int((signed int)v->value.float32);
        }
        if (v->value_type == 9)
        {
            return object_make_signed_int((signed int)v->value.float64);
        }
    }
    if (t == 5)
    {
        if (v->value_type == 0)
        {
            return object_make_unsigned_int((unsigned int)v->value.signed_int8);
        }
        if (v->value_type == 1)
        {
            return object_make_unsigned_int((unsigned int)v->value.unsigned_int8);
        }
        if (v->value_type == 2)
        {
            return object_make_unsigned_int((unsigned int)v->value.signed_int16);
        }
        if (v->value_type == 3)
        {
            return object_make_unsigned_int((unsigned int)v->value.unsigned_int16);
        }
        if (v->value_type == 4)
        {
            return object_make_unsigned_int((unsigned int)v->value.signed_int32);
        }
        if (v->value_type == 6)
        {
            return object_make_unsigned_int((unsigned int)v->value.signed_int64);
        }
        if (v->value_type == 7)
        {
            return object_make_unsigned_int((unsigned int)v->value.unsigned_int64);
        }
        if (v->value_type == 8)
        {
            return object_make_unsigned_int((unsigned int)v->value.float32);
        }
        if (v->value_type == 9)
        {
            return object_make_unsigned_int((unsigned int)v->value.float64);
        }
    }
    if (t == 6)
    {
        if (v->value_type == 0)
        {
            return object_make_signed_long_long((signed long long)v->value.signed_int8);
        }
        if (v->value_type == 1)
        {
            return object_make_signed_long_long((signed long long)v->value.unsigned_int8);
        }
        if (v->value_type == 2)
        {
            return object_make_signed_long_long((signed long long)v->value.signed_int16);
        }
        if (v->value_type == 3)
        {
            return object_make_signed_long_long((signed long long)v->value.unsigned_int16);
        }
        if (v->value_type == 4)
        {
            return object_make_signed_long_long((signed long long)v->value.signed_int32);
        }
        if (v->value_type == 5)
        {
            return object_make_signed_long_long((signed long long)v->value.unsigned_int32);
        }
        if (v->value_type == 7)
        {
            return object_make_signed_long_long((signed long long)v->value.unsigned_int64);
        }
        if (v->value_type == 8)
        {
            return object_make_signed_long_long((signed long long)v->value.float32);
        }
        if (v->value_type == 9)
        {
            return object_make_signed_long_long((signed long long)v->value.float64);
        }
    }
    if (t == 7)
    {
        if (v->value_type == 0)
        {
            return object_make_unsigned_long_long((unsigned long long)v->value.signed_int8);
        }
        if (v->value_type == 1)
        {
            return object_make_unsigned_long_long((unsigned long long)v->value.unsigned_int8);
        }
        if (v->value_type == 2)
        {
            return object_make_unsigned_long_long((unsigned long long)v->value.signed_int16);
        }
        if (v->value_type == 3)
        {
            return object_make_unsigned_long_long((unsigned long long)v->value.unsigned_int16);
        }
        if (v->value_type == 4)
        {
            return object_make_unsigned_long_long((unsigned long long)v->value.signed_int32);
        }
        if (v->value_type == 5)
        {
            return object_make_unsigned_long_long((unsigned long long)v->value.unsigned_int32);
        }
        if (v->value_type == 6)
        {
            return object_make_unsigned_long_long((unsigned long long)v->value.signed_int64);
        }
        if (v->value_type == 8)
        {
            return object_make_unsigned_long_long((unsigned long long)v->value.float32);
        }
        if (v->value_type == 9)
        {
            return object_make_unsigned_long_long((unsigned long long)v->value.float64);
        }
    }
    if (t == 8)
    {
        if (v->value_type == 0)
        {
            return object_make_float((float)v->value.signed_int8);
        }
        if (v->value_type == 1)
        {
            return object_make_float((float)v->value.unsigned_int8);
        }
        if (v->value_type == 2)
        {
            return object_make_float((float)v->value.signed_int16);
        }
        if (v->value_type == 3)
        {
            return object_make_float((float)v->value.unsigned_int16);
        }
        if (v->value_type == 4)
        {
            return object_make_float((float)v->value.signed_int32);
        }
        if (v->value_type == 5)
        {
            return object_make_float((float)v->value.unsigned_int32);
        }
        if (v->value_type == 6)
        {
            return object_make_float((float)v->value.signed_int64);
        }
        if (v->value_type == 7)
        {
            return object_make_float((float)v->value.unsigned_int64);
        }
        if (v->value_type == 9)
        {
            return object_make_float((float)v->value.float64);
        }
    }
    if (t == 9)
    {
        if (v->value_type == 0)
        {
            return object_make_double((double)v->value.signed_int8);
        }
        if (v->value_type == 1)
        {
            return object_make_double((double)v->value.unsigned_int8);
        }
        if (v->value_type == 2)
        {
            return object_make_double((double)v->value.signed_int16);
        }
        if (v->value_type == 3)
        {
            return object_make_double((double)v->value.unsigned_int16);
        }
        if (v->value_type == 4)
        {
            return object_make_double((double)v->value.signed_int32);
        }
        if (v->value_type == 5)
        {
            return object_make_double((double)v->value.unsigned_int32);
        }
        if (v->value_type == 6)
        {
            return object_make_double((double)v->value.signed_int64);
        }
        if (v->value_type == 7)
        {
            return object_make_double((double)v->value.unsigned_int64);
        }
        if (v->value_type == 8)
        {
            return object_make_double((double)v->value.float32);
        }
    }
    _cake_zmem(&empty, 112);
    return empty;
}


unsigned char type_is_union(struct type * p_type);

void object_default_initialization(struct object * p_object, unsigned char is_constant)
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
        p_object->value.unsigned_int64 = 0;
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
    if (p_object->p_ref != 0U)
    {
        p_object = p_object->p_ref;
    }
    return p_object;
}


struct object *object_get_referenced(struct object * p_object)
{
    if (p_object->p_ref != 0U)
    {
        p_object = p_object->p_ref;
    }
    return p_object;
}


int get_rank(int t)
{
    if (t == 6 || t == 7)
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
    if (t == 6 || t == 7)
    {
        return 8U;
    }
    else
    {
        if (t == 4 || t == 5)
        {
            return 4U;
        }
        else
        {
            if (t == 2 || t == 3)
            {
                return 2U;
            }
            else
            {
                if (t == 0 || t == 1)
                {
                    return 1U;
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
        register int __Ck25_temp = t;
        if (__Ck25_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck25_temp == 2) goto _CKL2; /*case 2*/
        if (__Ck25_temp == 4) goto _CKL3; /*case 4*/
        if (__Ck25_temp == 6) goto _CKL4; /*case 6*/
        if (__Ck25_temp == 9) goto _CKL5; /*case 9*/
        goto _CKL6;/*default*/

        {
            _CKL1: /*case 0*/ 
            _CKL2: /*case 2*/ 
            _CKL3: /*case 4*/ 
            _CKL4: /*case 6*/ 
            _CKL5: /*case 9*/ 
            return 1;
            _CKL6: /*default*/ 
            goto _CKL0; /*break*/

        }
        _CKL0:;
    }
    return 0;
}


int to_unsigned(int t)
{
    /*switch*/
    {
        register int __Ck26_temp = t;
        if (__Ck26_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck26_temp == 2) goto _CKL2; /*case 2*/
        if (__Ck26_temp == 4) goto _CKL3; /*case 4*/
        if (__Ck26_temp == 6) goto _CKL4; /*case 6*/
        goto _CKL5;/*default*/

        {
            _CKL1: /*case 0*/ 
            return 1;
            _CKL2: /*case 2*/ 
            return 3;
            _CKL3: /*case 4*/ 
            return 5;
            _CKL4: /*case 6*/ 
            return 7;
            _CKL5: /*default*/ 
            goto _CKL0; /*break*/

        }
        _CKL0:;
    }
    return t;
}


unsigned char is_unsigned(int t)
{
    /*switch*/
    {
        register int __Ck27_temp = t;
        if (__Ck27_temp == 1) goto _CKL1; /*case 1*/
        if (__Ck27_temp == 3) goto _CKL2; /*case 3*/
        if (__Ck27_temp == 5) goto _CKL3; /*case 5*/
        if (__Ck27_temp == 7) goto _CKL4; /*case 7*/
        goto _CKL5;/*default*/

        {
            _CKL1: /*case 1*/ 
            _CKL2: /*case 3*/ 
            _CKL3: /*case 5*/ 
            _CKL4: /*case 7*/ 
            return 1;
            _CKL5: /*default*/ 
            goto _CKL0; /*break*/

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


unsigned char object_is_signed(struct object * p_object);

unsigned char object_is_zero(struct object * p_object)
{
    p_object = (struct object *)object_get_referenced(p_object);
    if (!object_has_constant_value(p_object))
    {
        return 0;
    }
    if (object_is_signed(p_object))
    {
        signed long long r;

        r = object_to_signed_long_long(p_object);
        return r == 0;
    }
    else
    {
        unsigned long long r;

        r = object_to_unsigned_long_long(p_object);
        return r == 0;
    }
    return 0;
}


unsigned char object_is_one(struct object * p_object)
{
    p_object = (struct object *)object_get_referenced(p_object);
    if (!object_has_constant_value(p_object))
    {
        return 0;
    }
    if (object_is_signed(p_object))
    {
        signed long long r;

        r = object_to_signed_long_long(p_object);
        return r == 1;
    }
    else
    {
        unsigned long long r;

        r = object_to_unsigned_long_long(p_object);
        return r == 1;
    }
    return 0;
}


unsigned char object_is_signed(struct object * p_object)
{
    p_object = (struct object *)object_get_referenced(p_object);
    return is_signed(p_object->value_type);
}


unsigned char object_is_derived(struct object * p_object)
{
    if (p_object->p_ref != 0U)
    {
        return 0;
    }
    return p_object->members != 0U;
}


unsigned char object_is_reference(struct object * p_object)
{
    return p_object->p_ref != 0U;
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


unsigned char type_is_pointer_or_array(struct type * p_type);
unsigned char type_is_function(struct type * p_type);
unsigned char compiler_diagnostic(int w, struct parser_ctx * ctx, struct token * p_token, struct marker * p_marker, char * fmt, ...);

int object_set(struct parser_ctx * ctx, struct object * to, struct expression * p_init_expression, struct object * from, unsigned char is_constant, unsigned char requires_constant_initialization)
{
    if (1) /*try*/
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
                    goto _CKL0;/*throw*/
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
    else _CKL0: /*catch*/ 
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

struct object *make_object_ptr_core(struct type * p_type, char * name, int target)
{
    struct object * p_object;

    p_object = 0U;
    if (1) /*try*/
    {
        struct struct_or_union_specifier * p_struct_or_union_specifier;
        struct object * p_last_member_obj;
        struct member_declaration * p_member_declaration;

        if (p_type->category == 1)
        {
            p_object = calloc(1, 112U);
            if (p_object == 0U)
            {
                goto _CKL0;/*throw*/
            }
            p_object->debug_name = strdup(name);
            p_object->type = type_dup(p_type);
            return p_object;
        }
        if (p_type->category == 3)
        {
            p_object = calloc(1, 112U);
            if (p_object == 0U)
            {
                goto _CKL0;/*throw*/
            }
            *p_object = object_make_nullptr(target);
            p_object->state = 0;
            ;
            p_object->debug_name = strdup(name);
            type_destroy(&p_object->type);
            p_object->type = type_dup(p_type);
            return p_object;
        }
        if (p_type->category == 2)
        {
            p_object = calloc(1, 112U);
            if (p_object == 0U)
            {
                goto _CKL0;/*throw*/
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
                        p_member_obj = make_object_ptr_core(&array_item_type, buffer, target);
                        if (p_member_obj == 0U)
                        {
                            type_destroy(&array_item_type);
                            goto _CKL0;/*throw*/
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
            p_object = calloc(1, 112U);
            if (p_object == 0U)
            {
                goto _CKL0;/*throw*/
            }
            p_object->state = 0;
            p_object->value_type = type_to_object_type(p_type, target);
            p_object->value.signed_int64 = -1;
            p_object->debug_name = strdup(name);
            p_object->type = type_dup(p_type);
            return p_object;
        }
        p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
        if (p_struct_or_union_specifier == 0U)
        {
            goto _CKL0;/*throw*/
        }
        p_object = calloc(1, 112U);
        if (p_object == 0U)
        {
            goto _CKL0;/*throw*/
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
                        p_member_obj = make_object_ptr_core(&p_member_declarator->declarator->type, buffer, target);
                        if (p_member_obj == 0U)
                        {
                            goto _CKL0;/*throw*/
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

                        _cake_zmem(&t, 68);
                        t.category = 0;
                        t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                        t.type_specifier_flags = 32768;
                        _cake_zmem(&buffer, 200);
                        snprintf(buffer, 200U, ".%s", name);
                        p_member_obj = make_object_ptr_core(&t, buffer, target);
                        if (p_member_obj == 0U)
                        {
                            goto _CKL0;/*throw*/
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
    else _CKL0: /*catch*/ 
    {
        object_delete(p_object);
        p_object = 0U;
    }
    return 0U;
}


struct object *make_object_ptr(struct type * p_type, int target)
{
    return make_object_ptr_core(p_type, "", target);
}


static void object_fix_parent(struct object * p_object, struct object * parent);

int make_object_with_name(struct type * p_type, struct object * obj, char * name, int target)
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

    it = p_object->members;
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
    if (src->debug_name)
    {
        result.debug_name = strdup(src->debug_name);
    }
    result.next = 0U;
    return result;
}


int make_object(struct type * p_type, struct object * obj, int target)
{
    return make_object_with_name(p_type, obj, "", target);
}


int type_specifier_to_object_type(int type_specifier_flags, int target)
{
    if (type_specifier_flags & 512)
    {
        return 1;
    }
    if (type_specifier_flags & 32)
    {
        return 8;
    }
    if (type_specifier_flags & 64)
    {
        if (type_specifier_flags & 16)
        {
            return 9;
        }
        return 9;
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
            /*switch*/
            {
                register int __Ck28_temp = target;
                if (__Ck28_temp == 0) goto _CKL9; /*case 0*/
                if (__Ck28_temp == 1) goto _CKL10; /*case 1*/
                if (__Ck28_temp == 2) goto _CKL11; /*case 2*/
                if (__Ck28_temp == 3) goto _CKL12; /*case 3*/
                if (__Ck28_temp == 4) goto _CKL13; /*case 4*/
                goto _CKL8;

                {
                    _CKL9: /*case 0*/ 
                    return 7;
                    _CKL10: /*case 1*/ 
                    _CKL11: /*case 2*/ 
                    return 5;
                    _CKL12: /*case 3*/ 
                    return 5;
                    _CKL13: /*case 4*/ 
                    return 5;
                }
                _CKL8:;
            }
        }
        if (type_specifier_flags & 8)
        {
            return 5;
        }
        if (type_specifier_flags & 4194304)
        {
            return 7;
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
            /*switch*/
            {
                register int __Ck29_temp = target;
                if (__Ck29_temp == 0) goto _CKL20; /*case 0*/
                if (__Ck29_temp == 1) goto _CKL21; /*case 1*/
                if (__Ck29_temp == 2) goto _CKL22; /*case 2*/
                if (__Ck29_temp == 3) goto _CKL23; /*case 3*/
                if (__Ck29_temp == 4) goto _CKL24; /*case 4*/
                goto _CKL19;

                {
                    _CKL20: /*case 0*/ 
                    return 6;
                    _CKL21: /*case 1*/ 
                    _CKL22: /*case 2*/ 
                    return 4;
                    _CKL23: /*case 3*/ 
                    return 4;
                    _CKL24: /*case 4*/ 
                    return 4;
                }
                _CKL19:;
            }
        }
        if (type_specifier_flags & 8)
        {
            return 4;
        }
        if (type_specifier_flags & 4194304)
        {
            return 6;
        }
    }
    return 4;
}


unsigned char type_is_pointer(struct type * p_type);

int type_to_object_type(struct type * type, int target)
{
    if (type_is_pointer(type))
    {
        /*switch*/
        {
            register int __Ck30_temp = target;
            if (__Ck30_temp == 0) goto _CKL2; /*case 0*/
            if (__Ck30_temp == 1) goto _CKL3; /*case 1*/
            if (__Ck30_temp == 2) goto _CKL4; /*case 2*/
            if (__Ck30_temp == 3) goto _CKL5; /*case 3*/
            if (__Ck30_temp == 4) goto _CKL6; /*case 4*/
            goto _CKL1;

            {
                _CKL2: /*case 0*/ 
                return 7;
                _CKL3: /*case 1*/ 
                return 5;
                _CKL4: /*case 2*/ 
                return 7;
                _CKL5: /*case 3*/ 
                return 3;
                _CKL6: /*case 4*/ 
                return 5;
            }
            _CKL1:;
        }
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
        register int __Ck31_temp = a->value_type;
        if (__Ck31_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck31_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck31_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck31_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck31_temp == 4) goto _CKL5; /*case 4*/
        if (__Ck31_temp == 5) goto _CKL6; /*case 5*/
        if (__Ck31_temp == 6) goto _CKL7; /*case 6*/
        if (__Ck31_temp == 7) goto _CKL8; /*case 7*/
        if (__Ck31_temp == 8) goto _CKL9; /*case 8*/
        if (__Ck31_temp == 9) goto _CKL10; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            printf("%d (signed char)", (int)a->value.signed_int8);
            goto _CKL0; /*break*/

            _CKL2: /*case 1*/ 
            printf("%d (unsigned char)", (int)a->value.unsigned_int8);
            goto _CKL0; /*break*/

            _CKL3: /*case 2*/ 
            printf("%d (short)", a->value.signed_int16);
            goto _CKL0; /*break*/

            _CKL4: /*case 3*/ 
            printf("%d (unsigned short)", a->value.unsigned_int16);
            goto _CKL0; /*break*/

            _CKL5: /*case 4*/ 
            printf("%d (int)", a->value.signed_int32);
            goto _CKL0; /*break*/

            _CKL6: /*case 5*/ 
            printf("%du (unsigned int)", a->value.unsigned_int32);
            goto _CKL0; /*break*/

            _CKL7: /*case 6*/ 
            printf("%lld (long long)", a->value.signed_int64);
            goto _CKL0; /*break*/

            _CKL8: /*case 7*/ 
            printf("%llu (unsigned long long)", a->value.unsigned_int64);
            goto _CKL0; /*break*/

            _CKL9: /*case 8*/ 
            printf("%f (float)", a->value.float32);
            goto _CKL0; /*break*/

            _CKL10: /*case 9*/ 
            printf("%lf (double)", a->value.float64);
            goto _CKL0; /*break*/

        }
        _CKL0:;
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

    ((void)(_ArgList = (char *)(&(_Format)) + 4U));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0U, _ArgList);
    ((void)(_ArgList = 0U));
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
    if (object->debug_name)
    {
        printf("%s ", object->debug_name);
    }
    if (object->members != 0U)
    {
        struct object * member;

        type_print(&object->type, target);
        printf(" {\n");
        member = object->members;
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
            register int __Ck32_temp = object->state;
            if (__Ck32_temp == 0) goto _CKL4; /*case 0*/
            if (__Ck32_temp == 1) goto _CKL5; /*case 1*/
            if (__Ck32_temp == 4) goto _CKL6; /*case 4*/
            if (__Ck32_temp == 2) goto _CKL7; /*case 2*/
            goto _CKL3;

            {
                _CKL4: /*case 0*/ 
                printf(" uninitialized ");
                goto _CKL3; /*break*/

                _CKL5: /*case 1*/ 
                printf(" unknown ");
                goto _CKL3; /*break*/

                _CKL6: /*case 4*/ 
                printf(" exact ");
                goto _CKL3; /*break*/

                _CKL7: /*case 2*/ 
                printf(" constant_exact ");
                goto _CKL3; /*break*/

            }
            _CKL3:;
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


struct object *object_extend_array_to_index(struct type * p_type, struct object * a, unsigned int max_index, unsigned char is_constant, int target)
{
    struct object * it;

    it = a->members;
    if (1) /*try*/
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
                a->members = make_object_ptr(p_type, target);
                if (a->members == 0U)
                {
                    goto _CKL0;/*throw*/
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

                p = make_object_ptr(p_type, target);
                if (p == 0U)
                {
                    goto _CKL0;/*throw*/
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
    else _CKL0: /*catch*/ 
    {
    }
    return it;
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
    if (a_type == 9 || b_type == 9)
    {
        return 9;
    }
    if (a_type == 8 || b_type == 8)
    {
        return 8;
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


int object_greater_than_or_equal(struct object * a, struct object * b)
{
    int common_type;

    a = object_get_referenced(a);
    b = object_get_referenced(b);
    common_type = object_common(a, b);
    /*switch*/
    {
        register int __Ck33_temp = common_type;
        if (__Ck33_temp == 4) goto _CKL1; /*case 4*/
        if (__Ck33_temp == 5) goto _CKL2; /*case 5*/
        if (__Ck33_temp == 0) goto _CKL3; /*case 0*/
        if (__Ck33_temp == 1) goto _CKL4; /*case 1*/
        if (__Ck33_temp == 2) goto _CKL5; /*case 2*/
        if (__Ck33_temp == 3) goto _CKL6; /*case 3*/
        if (__Ck33_temp == 6) goto _CKL7; /*case 6*/
        if (__Ck33_temp == 7) goto _CKL8; /*case 7*/
        if (__Ck33_temp == 8) goto _CKL9; /*case 8*/
        if (__Ck33_temp == 9) goto _CKL10; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 4*/ 
            return object_to_signed_int(a) >= object_to_signed_int(b);
            _CKL2: /*case 5*/ 
            return object_to_unsigned_int(a) >= object_to_unsigned_int(b);
            _CKL3: /*case 0*/ 
            return object_to_signed_char(a) >= object_to_signed_char(b);
            goto _CKL0; /*break*/

            _CKL4: /*case 1*/ 
            return object_to_unsigned_char(a) >= object_to_unsigned_char(b);
            _CKL5: /*case 2*/ 
            return object_to_signed_short(a) >= object_to_signed_short(b);
            _CKL6: /*case 3*/ 
            return object_to_unsigned_short(a) >= object_to_unsigned_short(b);
            _CKL7: /*case 6*/ 
            return object_to_signed_long_long(a) >= object_to_signed_long_long(b);
            _CKL8: /*case 7*/ 
            return object_to_unsigned_long_long(a) >= object_to_unsigned_long_long(b);
            _CKL9: /*case 8*/ 
            return object_to_float(a) >= object_to_float(b);
            _CKL10: /*case 9*/ 
            return object_to_double(a) >= object_to_double(b);
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
        register int __Ck34_temp = common_type;
        if (__Ck34_temp == 4) goto _CKL1; /*case 4*/
        if (__Ck34_temp == 5) goto _CKL2; /*case 5*/
        if (__Ck34_temp == 0) goto _CKL3; /*case 0*/
        if (__Ck34_temp == 1) goto _CKL4; /*case 1*/
        if (__Ck34_temp == 2) goto _CKL5; /*case 2*/
        if (__Ck34_temp == 3) goto _CKL6; /*case 3*/
        if (__Ck34_temp == 6) goto _CKL7; /*case 6*/
        if (__Ck34_temp == 7) goto _CKL8; /*case 7*/
        if (__Ck34_temp == 8) goto _CKL9; /*case 8*/
        if (__Ck34_temp == 9) goto _CKL10; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 4*/ 
            return object_to_signed_int(a) <= object_to_signed_int(b);
            _CKL2: /*case 5*/ 
            return object_to_unsigned_int(a) <= object_to_unsigned_int(b);
            _CKL3: /*case 0*/ 
            return object_to_signed_char(a) <= object_to_signed_char(b);
            goto _CKL0; /*break*/

            _CKL4: /*case 1*/ 
            return object_to_unsigned_char(a) <= object_to_unsigned_char(b);
            _CKL5: /*case 2*/ 
            return object_to_signed_short(a) <= object_to_signed_short(b);
            _CKL6: /*case 3*/ 
            return object_to_unsigned_short(a) <= object_to_unsigned_short(b);
            _CKL7: /*case 6*/ 
            return object_to_signed_long_long(a) <= object_to_signed_long_long(b);
            _CKL8: /*case 7*/ 
            return object_to_unsigned_long_long(a) <= object_to_unsigned_long_long(b);
            _CKL9: /*case 8*/ 
            return object_to_float(a) <= object_to_float(b);
            _CKL10: /*case 9*/ 
            return object_to_double(a) <= object_to_double(b);
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
        register int __Ck35_temp = common_type;
        if (__Ck35_temp == 4) goto _CKL1; /*case 4*/
        if (__Ck35_temp == 5) goto _CKL2; /*case 5*/
        if (__Ck35_temp == 6) goto _CKL3; /*case 6*/
        if (__Ck35_temp == 7) goto _CKL4; /*case 7*/
        if (__Ck35_temp == 8) goto _CKL5; /*case 8*/
        if (__Ck35_temp == 9) goto _CKL6; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 4*/ 
            return object_make_signed_int(object_to_signed_int(a) + object_to_signed_int(b));
            _CKL2: /*case 5*/ 
            return object_make_unsigned_int(object_to_unsigned_int(a) + object_to_unsigned_int(b));
            _CKL3: /*case 6*/ 
            return object_make_signed_long_long(object_to_signed_long_long(a) + object_to_signed_long_long(b));
            _CKL4: /*case 7*/ 
            return object_make_unsigned_long_long(object_to_unsigned_long_long(a) + object_to_unsigned_long_long(b));
            _CKL5: /*case 8*/ 
            return object_make_float(object_to_float(a) + object_to_float(b));
            _CKL6: /*case 9*/ 
            return object_make_double(object_to_double(a) + object_to_double(b));
        }
        _CKL0:;
    }
    ;
    _cake_zmem(&o, 112);
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
        register int __Ck36_temp = common_type;
        if (__Ck36_temp == 4) goto _CKL1; /*case 4*/
        if (__Ck36_temp == 5) goto _CKL2; /*case 5*/
        if (__Ck36_temp == 6) goto _CKL3; /*case 6*/
        if (__Ck36_temp == 7) goto _CKL4; /*case 7*/
        if (__Ck36_temp == 8) goto _CKL5; /*case 8*/
        if (__Ck36_temp == 9) goto _CKL6; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 4*/ 
            return object_make_signed_int(object_to_signed_int(a) - object_to_signed_int(b));
            _CKL2: /*case 5*/ 
            return object_make_unsigned_int(object_to_unsigned_int(a) - object_to_unsigned_int(b));
            _CKL3: /*case 6*/ 
            return object_make_signed_long_long(object_to_signed_long_long(a) - object_to_signed_long_long(b));
            _CKL4: /*case 7*/ 
            return object_make_unsigned_long_long(object_to_unsigned_long_long(a) - object_to_unsigned_long_long(b));
            _CKL5: /*case 8*/ 
            return object_make_float(object_to_float(a) - object_to_float(b));
            _CKL6: /*case 9*/ 
            return object_make_double(object_to_double(a) - object_to_double(b));
        }
        _CKL0:;
    }
    ;
    _cake_zmem(&o, 112);
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
        register int __Ck37_temp = common_type;
        if (__Ck37_temp == 4) goto _CKL1; /*case 4*/
        if (__Ck37_temp == 5) goto _CKL2; /*case 5*/
        if (__Ck37_temp == 0) goto _CKL3; /*case 0*/
        if (__Ck37_temp == 1) goto _CKL4; /*case 1*/
        if (__Ck37_temp == 2) goto _CKL5; /*case 2*/
        if (__Ck37_temp == 3) goto _CKL6; /*case 3*/
        if (__Ck37_temp == 6) goto _CKL7; /*case 6*/
        if (__Ck37_temp == 7) goto _CKL8; /*case 7*/
        if (__Ck37_temp == 8) goto _CKL9; /*case 8*/
        if (__Ck37_temp == 9) goto _CKL10; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 4*/ 
            return object_to_signed_int(a) == object_to_signed_int(b);
            _CKL2: /*case 5*/ 
            return object_to_unsigned_int(a) == object_to_unsigned_int(b);
            _CKL3: /*case 0*/ 
            return object_to_signed_char(a) == object_to_signed_char(b);
            goto _CKL0; /*break*/

            _CKL4: /*case 1*/ 
            return object_to_unsigned_char(a) == object_to_unsigned_char(b);
            _CKL5: /*case 2*/ 
            return object_to_signed_short(a) == object_to_signed_short(b);
            _CKL6: /*case 3*/ 
            return object_to_unsigned_short(a) == object_to_unsigned_short(b);
            _CKL7: /*case 6*/ 
            return object_to_signed_long_long(a) == object_to_signed_long_long(b);
            _CKL8: /*case 7*/ 
            return object_to_unsigned_long_long(a) == object_to_unsigned_long_long(b);
            _CKL9: /*case 8*/ 
            return object_to_float(a) == object_to_float(b);
            _CKL10: /*case 9*/ 
            return object_to_double(a) == object_to_double(b);
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
        register int __Ck38_temp = common_type;
        if (__Ck38_temp == 4) goto _CKL1; /*case 4*/
        if (__Ck38_temp == 5) goto _CKL2; /*case 5*/
        if (__Ck38_temp == 0) goto _CKL3; /*case 0*/
        if (__Ck38_temp == 1) goto _CKL4; /*case 1*/
        if (__Ck38_temp == 2) goto _CKL5; /*case 2*/
        if (__Ck38_temp == 3) goto _CKL6; /*case 3*/
        if (__Ck38_temp == 6) goto _CKL7; /*case 6*/
        if (__Ck38_temp == 7) goto _CKL8; /*case 7*/
        if (__Ck38_temp == 8) goto _CKL9; /*case 8*/
        if (__Ck38_temp == 9) goto _CKL10; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 4*/ 
            return object_to_signed_int(a) != object_to_signed_int(b);
            _CKL2: /*case 5*/ 
            return object_to_unsigned_int(a) != object_to_unsigned_int(b);
            _CKL3: /*case 0*/ 
            return object_to_signed_char(a) != object_to_signed_char(b);
            goto _CKL0; /*break*/

            _CKL4: /*case 1*/ 
            return object_to_unsigned_char(a) != object_to_unsigned_char(b);
            _CKL5: /*case 2*/ 
            return object_to_signed_short(a) != object_to_signed_short(b);
            _CKL6: /*case 3*/ 
            return object_to_unsigned_short(a) != object_to_unsigned_short(b);
            _CKL7: /*case 6*/ 
            return object_to_signed_long_long(a) != object_to_signed_long_long(b);
            _CKL8: /*case 7*/ 
            return object_to_unsigned_long_long(a) != object_to_unsigned_long_long(b);
            _CKL9: /*case 8*/ 
            return object_to_float(a) != object_to_float(b);
            _CKL10: /*case 9*/ 
            return object_to_double(a) != object_to_double(b);
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


int ss_fprintf(struct osstream * stream, char * fmt, ...);
char *target_uintN_suffix(int target, int size);
char *target_intN_suffix(int target, int size);

void object_print_value(struct osstream * ss, struct object * a, int target)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int __Ck39_temp = a->value_type;
        if (__Ck39_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck39_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck39_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck39_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck39_temp == 4) goto _CKL5; /*case 4*/
        if (__Ck39_temp == 5) goto _CKL6; /*case 5*/
        if (__Ck39_temp == 6) goto _CKL7; /*case 6*/
        if (__Ck39_temp == 7) goto _CKL8; /*case 7*/
        if (__Ck39_temp == 8) goto _CKL9; /*case 8*/
        if (__Ck39_temp == 9) goto _CKL10; /*case 9*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            ss_fprintf(ss, "%hhi", (int)a->value.signed_int8);
            goto _CKL0; /*break*/

            _CKL2: /*case 1*/ 
            ss_fprintf(ss, "%hhu", (int)a->value.unsigned_int8);
            goto _CKL0; /*break*/

            _CKL3: /*case 2*/ 
            ss_fprintf(ss, "%hi", a->value.signed_int16);
            goto _CKL0; /*break*/

            _CKL4: /*case 3*/ 
            ss_fprintf(ss, "%hu", a->value.unsigned_int16);
            goto _CKL0; /*break*/

            _CKL5: /*case 4*/ 
            ss_fprintf(ss, "%i", a->value.signed_int32);
            goto _CKL0; /*break*/

            _CKL6: /*case 5*/ 
            ss_fprintf(ss, "%u", a->value.unsigned_int32);
            ss_fprintf(ss, target_uintN_suffix(target, 32));
            goto _CKL0; /*break*/

            _CKL7: /*case 6*/ 
            ss_fprintf(ss, "%lli", a->value.signed_int64);
            ss_fprintf(ss, target_intN_suffix(target, 64));
            goto _CKL0; /*break*/

            _CKL8: /*case 7*/ 
            ss_fprintf(ss, "%llu", a->value.unsigned_int64);
            ss_fprintf(ss, target_uintN_suffix(target, 64));
            goto _CKL0; /*break*/

            _CKL9: /*case 8*/ 
            ss_fprintf(ss, "%f", a->value.float32);
            ss_fprintf(ss, "f");
            goto _CKL0; /*break*/

            _CKL10: /*case 9*/ 
            ss_fprintf(ss, "%lf", a->value.float64);
            goto _CKL0; /*break*/

        }
        _CKL0:;
    }
}


