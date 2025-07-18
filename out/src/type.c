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

struct generic_association {
    struct type  type;
    struct type_name * p_type_name;
    struct expression * expression;
    struct token * first_token;
    struct token * last_token;
    struct generic_association * next;
};

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

struct object {
    int state;
    int value_type;
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
    int type;
    union _struct_tag_5  data;
};

struct static_assert_declaration {
    struct token * first_token;
    struct token * last_token;
    struct expression * constant_expression;
    struct token * string_literal_opt;
};

struct defer_list {
    struct defer_list_item * head;
    struct defer_list_item * tail;
};

struct defer_statement {
    struct token * first_token;
    struct token * last_token;
    struct secondary_block * secondary_block;
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

struct case_label_list {
    struct label * head;
    struct label * tail;
};

struct function_specifier {
    int flags;
    struct token * token;
};

struct type_list {
    struct type * head;
    struct type * tail;
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

struct attribute_token {
    int attributes_flags;
    struct token * token;
};

struct try_statement {
    struct secondary_block * secondary_block;
    struct secondary_block * catch_secondary_block_opt;
    struct token * first_token;
    struct token * last_token;
    struct token * catch_token_opt;
    int catch_label_id;
};

struct member_declaration {
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct member_declarator_list * member_declarator_list_opt;
    struct static_assert_declaration * static_assert_declaration;
    struct pragma_declaration * pragma_declaration;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct member_declaration * next;
};

struct labeled_statement {
    struct label * label;
    struct statement * statement;
};

struct initializer_list_item {
    struct designation * designation;
    struct initializer * initializer;
    struct initializer_list_item * next;
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

struct attribute_list {
    int attributes_flags;
    struct attribute * head;
    struct attribute * tail;
};

struct atomic_type_specifier {
    struct token * token;
    struct type_name * type_name;
};

struct init_declarator {
    unsigned char   has_shared_ownership;
    struct declarator * p_declarator;
    struct initializer * initializer;
    struct init_declarator * next;
};

struct type_name {
    struct token * first_token;
    struct token * last_token;
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct declarator * abstract_declarator;
    struct type  type;
};

struct parameter_type_list {
    unsigned char   is_var_args;
    unsigned char   is_void;
    struct parameter_list * parameter_list;
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

struct block_item_list {
    struct block_item * head;
    struct block_item * tail;
};

struct member_declarator {
    struct declarator * declarator;
    struct expression * constant_expression;
    struct member_declarator * next;
};

struct designator_list {
    struct designator * head;
    struct designator * tail;
};

struct designation {
    struct designator_list * designator_list;
    struct token * token;
};

struct argument_expression {
    struct expression * expression;
    struct argument_expression * next;
    unsigned char   set_unkown;
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

struct array_declarator {
    struct direct_declarator * direct_declarator;
    struct expression * assignment_expression;
    struct expression * expression;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * token;
    struct token * static_token_opt;
};

struct _iobuf {
    void * _Placeholder;
};

struct label {
    struct expression * constant_expression;
    struct expression * constant_expression_end;
    struct token * p_identifier_opt;
    struct token * p_first_token;
    struct label * next;
    int label_id;
};

struct braced_initializer {
    struct token * first_token;
    struct token * last_token;
    struct initializer_list * initializer_list;
};

struct block_item {
    struct token * first_token;
    struct declaration * declaration;
    struct unlabeled_statement * unlabeled_statement;
    struct label * label;
    struct block_item * next;
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

struct storage_class_specifier {
    int flags;
    struct token * token;
};

struct statement {
    struct labeled_statement * labeled_statement;
    struct unlabeled_statement * unlabeled_statement;
};

struct secondary_block {
    struct token * first_token;
    struct token * last_token;
    struct statement * statement;
};

struct balanced_token_sequence {
    struct balanced_token * head;
    struct balanced_token * tail;
};

struct osstream {
    char * c_str;
    int size;
    int capacity;
};

struct typeof_specifier {
    struct token * first_token;
    struct token * last_token;
    struct typeof_specifier_argument * typeof_specifier_argument;
    struct type  type;
};

struct jump_statement {
    struct token * label;
    struct token * first_token;
    struct token * last_token;
    struct expression * expression_opt;
    int label_id;
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

struct initializer {
    struct token * first_token;
    struct braced_initializer * braced_initializer;
    struct expression * assignment_expression;
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

struct parameter_list {
    struct parameter_declaration * head;
    struct parameter_declaration * tail;
};

struct member_declarator_list {
    struct member_declarator * head;
    struct member_declarator * tail;
};

struct param {
    struct type  type;
    struct param * next;
};

struct expression_statement {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct expression * expression_opt;
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

struct initializer_list {
    struct token * first_token;
    struct initializer_list_item * head;
    struct initializer_list_item * tail;
    int size;
};

struct label_list {
    struct label_list_item * head;
    struct label_list_item * tail;
};

struct init_statement {
    struct expression_statement * p_expression_statement;
    struct simple_declaration * p_simple_declaration;
};

struct attribute_specifier_sequence {
    struct token * first_token;
    struct token * last_token;
    int attributes_flags;
    struct attribute_specifier * head;
    struct attribute_specifier * tail;
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
    struct enum_specifier * p_complete_enum_specifier;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
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

struct scope_list {
    struct scope * head;
    struct scope * tail;
};

struct parameter_declaration {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct declaration_specifiers * declaration_specifiers;
    struct declarator * declarator;
    struct parameter_declaration * next;
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

struct marker {
    char * file;
    int line;
    int start_col;
    int end_col;
    struct token * p_token_caret;
    struct token * p_token_begin;
    struct token * p_token_end;
};

struct designator {
    struct expression * constant_expression_opt;
    struct token * token;
    struct designator * next;
};

struct simple_declaration {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator_list  init_declarator_list;
    struct token * first_token;
    struct token * last_token;
};

struct pragma_declaration {
    struct token * first_token;
    struct token * last_token;
};

struct attribute_specifier {
    struct token * first_token;
    struct token * last_token;
    struct attribute_list * attribute_list;
    struct attribute_specifier * next;
};

struct token_list {
    struct token * head;
    struct token * tail;
};

struct attribute {
    int attributes_flags;
    struct attribute_token * attribute_token;
    struct attribute_argument_clause * attribute_argument_clause;
    struct attribute * next;
};

struct pointer {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * calling_convention;
    struct pointer * pointer;
};

struct defer_list_item {
    struct declarator * declarator;
    struct defer_statement * defer_statement;
    struct defer_list_item * next;
};

struct compound_statement {
    struct token * first_token;
    struct token * last_token;
    struct block_item_list  block_item_list;
    struct diagnostic  diagnostic_flags;
    struct defer_list  defer_list;
};

struct type_specifier_qualifier {
    struct type_specifier * type_specifier;
    struct type_qualifier * type_qualifier;
    struct alignment_specifier * alignment_specifier;
    struct type_specifier_qualifier * next;
};

struct type_qualifier_list {
    int flags;
    struct type_qualifier * head;
    struct type_qualifier * tail;
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

struct balanced_token {
    struct token * token;
    struct balanced_token * next;
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

struct attribute_argument_clause {
    struct balanced_token_sequence * p_balanced_token_sequence;
    struct token * token;
};

struct typeof_specifier_argument {
    struct expression * expression;
    struct type_name * type_name;
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

struct type_qualifier {
    int flags;
    struct token * token;
    struct type_qualifier * next;
};

struct direct_declarator {
    struct token * name_opt;
    struct token * p_calling_convention;
    struct declarator * declarator;
    struct array_declarator * array_declarator;
    struct function_declarator * function_declarator;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

int _Avx2WmemEnabledWeakValue = 0;
int ss_fprintf(struct osstream * stream, char * fmt, ...);

void print_item(struct osstream * ss, unsigned char  * first, char * item)
{
    if (!(*first))
    {
        ss_fprintf(ss, " ");
    }
    ss_fprintf(ss, "%s", item);
    *first = 0;
}

unsigned char  print_type_specifier_flags(struct osstream * ss, unsigned char  * first, int e_type_specifier_flags)
{
    if (e_type_specifier_flags & 1)
    {
        print_item(ss, first, "void");
    }
    if (e_type_specifier_flags & 128)
    {
        print_item(ss, first, "signed");
    }
    if (e_type_specifier_flags & 256)
    {
        print_item(ss, first, "unsigned");
    }
    if (e_type_specifier_flags & 8)
    {
        print_item(ss, first, "int");
    }
    if (e_type_specifier_flags & 4)
    {
        print_item(ss, first, "short");
    }
    if (e_type_specifier_flags & 16)
    {
        print_item(ss, first, "long");
    }
    if (e_type_specifier_flags & 4194304)
    {
        print_item(ss, first, "long long");
    }
    if (e_type_specifier_flags & 524288)
    {
        print_item(ss, first, "__int16");
    }
    if (e_type_specifier_flags & 1048576)
    {
        print_item(ss, first, "__int32");
    }
    if (e_type_specifier_flags & 2097152)
    {
        print_item(ss, first, "__int64");
    }
    if (e_type_specifier_flags & 2)
    {
        print_item(ss, first, "char");
    }
    if (e_type_specifier_flags & 64)
    {
        print_item(ss, first, "double");
    }
    if (e_type_specifier_flags & 32)
    {
        print_item(ss, first, "float");
    }
    if (e_type_specifier_flags & 512)
    {
        print_item(ss, first, "_Bool");
    }
    if (e_type_specifier_flags & 1024)
    {
        print_item(ss, first, "_Complex");
    }
    if (e_type_specifier_flags & 2048)
    {
        print_item(ss, first, "_Decimal32");
    }
    if (e_type_specifier_flags & 4096)
    {
        print_item(ss, first, "_Decimal64");
    }
    if (e_type_specifier_flags & 8192)
    {
        print_item(ss, first, "_Decimal128");
    }
    if (e_type_specifier_flags & 16777216)
    {
        print_item(ss, first, "nullptr_t");
    }
    return *first;
}

void print_type_qualifier_flags(struct osstream * ss, unsigned char  * first, int e_type_qualifier_flags)
{
    if (e_type_qualifier_flags & 1)
    {
        print_item(ss, first, "const");
    }
    if (e_type_qualifier_flags & 2)
    {
        print_item(ss, first, "restrict");
    }
    if (e_type_qualifier_flags & 4)
    {
        print_item(ss, first, "volatile");
    }
    if (e_type_qualifier_flags & 16)
    {
        print_item(ss, first, "_Owner");
    }
    if (e_type_qualifier_flags & 128)
    {
        print_item(ss, first, "_Dtor");
    }
    if (e_type_qualifier_flags & 32)
    {
        print_item(ss, first, "_View");
    }
    if (e_type_qualifier_flags & 64)
    {
        print_item(ss, first, "_Opt");
    }
}

void print_type_qualifier_specifiers(struct osstream * ss, struct type * type)
{
    unsigned char   first;

    first = 1;
    print_type_qualifier_flags(ss, &first, type->type_qualifier_flags);
    if (type->type_specifier_flags & 32768)
    {
        ;
        print_item(ss, &first, "struct ");
        ss_fprintf(ss, "%s", type->struct_or_union_specifier->tag_name);
    }
    else
    {
        if (type->type_specifier_flags & 65536)
        {
            ;
            print_item(ss, &first, "enum ");
            if (type->enum_specifier->tag_token)
            {
                ss_fprintf(ss, "%s", type->enum_specifier->tag_token->lexeme);
            }
        }
        else
        {
            if (type->type_specifier_flags & 131072)
            {
                ;
            }
            else
            {
                print_type_specifier_flags(ss, &first, type->type_specifier_flags);
            }
        }
    }
}

void type_integer_promotion(struct type * a)
{
    if ((a->type_specifier_flags & 512) || (a->type_specifier_flags & 2) || (a->type_specifier_flags & 4) || (a->type_specifier_flags & 262144) || (a->type_specifier_flags & 524288))
    {
        a->type_specifier_flags = 8;
    }
}

void type_add_const(struct type * p_type)
{
    p_type->type_qualifier_flags |= 1;
}

void type_remove_qualifiers(struct type * p_type)
{
    p_type->type_qualifier_flags = 0;
}

int type_get_category(struct type * p_type);
struct type type_add_pointer(struct type * p_type, unsigned char   null_checks_enabled);
struct type get_array_item_type(struct type * p_type);
void type_destroy(struct type * p_type);
struct type type_dup(struct type * p_type);

struct type type_lvalue_conversion(struct type * p_type, unsigned char   nullchecks_enabled)
{
    int category;
    struct type  t;

    category = type_get_category(p_type);
    /*switch*/
    {
        register int _R0 = category;
        if (_R0 == 1) goto _CKL1; /*case 1*/
        if (_R0 == 2) goto _CKL2; /*case 2*/
        if (_R0 == 3) goto _CKL3; /*case 3*/
        if (_R0 == 0) goto _CKL4; /*case 0*/
        goto /*default*/ _CKL5;

        {
            /*case 1*/ _CKL1:
            {
                struct type  t;

                t = type_add_pointer(p_type, nullchecks_enabled);
                t.type_qualifier_flags &= -65;
                t.storage_class_specifier_flags &= -2049;
                t.category = t.category;
                return t;
            }
            /*break*/ goto _CKL0;

            /*case 2*/ _CKL2:
            {
                struct type  t;
                struct type  t2;

                t = get_array_item_type(p_type);
                t2 = type_add_pointer(&t, nullchecks_enabled);
                type_remove_qualifiers(&t2);
                type_destroy(&t);
                t2.storage_class_specifier_flags &= -2049;
                return t2;
            }
            /*break*/ goto _CKL0;

            /*case 3*/ _CKL3:
            /*break*/ goto _CKL0;

            /*case 0*/ _CKL4:
            /*default*/ _CKL5:
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    t = type_dup(p_type);
    type_remove_qualifiers(&t);
    t.storage_class_specifier_flags &= -2049;
    t.category = type_get_category(&t);
    return t;
}

unsigned char  type_is_nullptr_t(struct type * p_type);
struct type make_void_ptr_type();
char *strdup(char * src);

struct type type_convert_to(struct type * p_type, int target)
{
    if (target < 0 && type_is_nullptr_t(p_type))
    {
        struct type  t;

        t = make_void_ptr_type();
        ;
        if (p_type->name_opt)
        {
            t.name_opt = strdup(p_type->name_opt);
        }
        return t;
    }
    return type_dup(p_type);
}

void ss_swap(struct osstream * a, struct osstream * b);
void ss_close(struct osstream * stream);
void print_type(struct osstream * ss, struct type * type);

void print_type_core(struct osstream * ss, struct type * p_type, unsigned char   onlydeclarator, unsigned char   printname)
{
    struct type * p;

    p = p_type;
    while (p)
    {
        if (onlydeclarator && p->next == 0U)
        {
            break;
        }
        /*switch*/
        {
            register int _R1 = p->category;
            if (_R1 == 0) goto _CKL2; /*case 0*/
            if (_R1 == 2) goto _CKL9; /*case 2*/
            if (_R1 == 1) goto _CKL14; /*case 1*/
            if (_R1 == 3) goto _CKL17; /*case 3*/
            goto _CKL1;

            {
                unsigned char   b;
                struct param * pa;

                /*case 0*/ _CKL2:
                {
                    struct osstream  local;
                    unsigned char   first;
                    struct osstream  local2;

                    _cake_zmem(&local, 12);
                    first = 1;
                    print_type_qualifier_flags(&local, &first, p->type_qualifier_flags);
                    if (p->struct_or_union_specifier)
                    {
                        ss_fprintf(&local, "struct %s", p->struct_or_union_specifier->tag_name);
                    }
                    else
                    {
                        if (p->enum_specifier)
                        {
                            if (p->enum_specifier->tag_token)
                            {
                                ss_fprintf(&local, "enum %s", p->enum_specifier->tag_token->lexeme);
                            }
                            else
                            {
                                ss_fprintf(&local, "enum ");
                            }
                        }
                        else
                        {
                            print_type_specifier_flags(&local, &first, p->type_specifier_flags);
                        }
                    }
                    if (printname && p->name_opt)
                    {
                        if (first)
                        {
                            ss_fprintf(ss, " ");
                            first = 0;
                        }
                        ss_fprintf(ss, "%s", p->name_opt);
                    }
                    _cake_zmem(&local2, 12);
                    if (ss->size > 0)
                    {
                        ss_fprintf(&local2, "%s %s", local.c_str, ss->c_str);
                    }
                    else
                    {
                        ss_fprintf(&local2, "%s", local.c_str);
                    }
                    ss_swap(ss, &local2);
                    ss_close(&local);
                    ss_close(&local2);
                }
                /*break*/ goto _CKL1;

                /*case 2*/ _CKL9:
                if (printname && p->name_opt)
                {
                    ss_fprintf(ss, "%s", p->name_opt);
                }
                ss_fprintf(ss, "[");
                b = 1;
                if (p->has_static_array_size)
                {
                    ss_fprintf(ss, "static");
                    b = 0;
                }
                print_type_qualifier_flags(ss, &b, p->type_qualifier_flags);
                if (p->num_of_elements > 0)
                {
                    if (!b)
                    {
                        ss_fprintf(ss, " ");
                    }
                    ss_fprintf(ss, "%d", p->num_of_elements);
                }
                ss_fprintf(ss, "]");
                /*break*/ goto _CKL1;

                /*case 1*/ _CKL14:
                if (printname && p->name_opt)
                {
                    ss_fprintf(ss, "%s", p->name_opt);
                }
                ss_fprintf(ss, "(");
                pa = p->params.head;
                while (pa)
                {
                    struct osstream  sslocal;

                    _cake_zmem(&sslocal, 12);
                    print_type(&sslocal, &pa->type);
                    ss_fprintf(ss, "%s", sslocal.c_str);
                    if (pa->next)
                    {
                        ss_fprintf(ss, ",");
                    }
                    ss_close(&sslocal);
                    pa = pa->next;
                }
                ss_fprintf(ss, ")");
                /*break*/ goto _CKL1;

                /*case 3*/ _CKL17:
                {
                    struct osstream  local;
                    unsigned char   first;

                    _cake_zmem(&local, 12);
                    if (p->next && ((p->next->category == 1 || p->next->category == 2)))
                    {
                        ss_fprintf(&local, "(");
                    }
                    ss_fprintf(&local, "*");
                    first = 0;
                    print_type_qualifier_flags(&local, &first, p->type_qualifier_flags);
                    if (printname && p->name_opt)
                    {
                        if (!first)
                        {
                            ss_fprintf(ss, " ");
                        }
                        ss_fprintf(ss, "%s", p->name_opt);
                        first = 0;
                    }
                    if (ss->c_str)
                    {
                        ss_fprintf(&local, "%s", ss->c_str);
                    }
                    if (p->next && (p->next->category == 1 || p->next->category == 2))
                    {
                        ss_fprintf(&local, ")", ss->c_str);
                    }
                    ss_swap(ss, &local);
                    ss_close(&local);
                }
                /*break*/ goto _CKL1;

            }
            _CKL1:;
        }
        p = p->next;
    }
}

void print_type(struct osstream * ss, struct type * p_type)
{
    print_type_core(ss, p_type, 0, 1);
}

void print_type_no_names(struct osstream * ss, struct type * p_type)
{
    print_type_core(ss, p_type, 0, 0);
}

void print_type_declarator(struct osstream * ss, struct type * p_type)
{
    print_type_core(ss, p_type, 1, 1);
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

void type_print(struct type * a)
{
    struct osstream  ss;

    _cake_zmem(&ss, 12);
    print_type(&ss, a);
    printf("%s", ss.c_str);
    ss_close(&ss);
}

int __cdecl puts(char * _Buffer);

void type_println(struct type * a)
{
    type_print(a);
    puts("\n");
}

int type_get_category(struct type * p_type)
{
    return p_type->category;
}

void param_list_add(struct param_list * list, struct param * p_item)
{
    if (list->head == 0U)
    {
        list->head = p_item;
    }
    else
    {
        ;
        ;
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void free(void * ptr);

void param_list_destroy(struct param_list * p)
{
    struct param * item;

    item = p->head;
    while (item)
    {
        struct param * next;

        next = item->next;
        type_destroy(&item->type);
        free(item);
        item = next;
    }
}

void type_destroy_one(struct type * p_type)
{
    free((void *)p_type->name_opt);
    param_list_destroy(&p_type->params);
    ;
}

void type_destroy(struct type * p_type)
{
    struct type * item;

    free((void *)p_type->name_opt);
    param_list_destroy(&p_type->params);
    item = p_type->next;
    while (item)
    {
        struct type * next;

        next = item->next;
        item->next = 0U;
        type_destroy_one(item);
        free(item);
        item = next;
    }
}

void type_delete(struct type * p_type)
{
    if (p_type)
    {
        type_destroy(p_type);
        free(p_type);
    }
}

struct struct_or_union_specifier *get_complete_struct_or_union_specifier(struct struct_or_union_specifier * p_struct_or_union_specifier);
struct enum_specifier *get_complete_enum_specifier(struct enum_specifier * p_enum_specifier);

unsigned char  type_has_attribute(struct type * p_type, int attributes)
{
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;

    if (p_type->attributes_flags & attributes)
    {
        return 1;
    }
    p_attribute_specifier_sequence_opt = 0U;
    if (p_type->struct_or_union_specifier)
    {
        struct struct_or_union_specifier * p_complete;

        p_attribute_specifier_sequence_opt = p_type->struct_or_union_specifier->attribute_specifier_sequence_opt;
        p_complete = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
        if (p_attribute_specifier_sequence_opt == 0U && p_complete)
        {
            p_attribute_specifier_sequence_opt = p_complete->attribute_specifier_sequence_opt;
        }
    }
    else
    {
        if (p_type->enum_specifier)
        {
            struct enum_specifier * p_complete_enum_specifier;

            p_complete_enum_specifier = get_complete_enum_specifier(p_type->enum_specifier);
            p_attribute_specifier_sequence_opt = p_type->enum_specifier->attribute_specifier_sequence_opt;
            if (p_attribute_specifier_sequence_opt == 0U && p_complete_enum_specifier)
            {
                p_attribute_specifier_sequence_opt = p_complete_enum_specifier->attribute_specifier_sequence_opt;
            }
        }
    }
    if (p_attribute_specifier_sequence_opt && p_attribute_specifier_sequence_opt->attributes_flags & attributes)
    {
        return 1;
    }
    return 0;
}

unsigned char  type_is_maybe_unused(struct type * p_type)
{
    return type_has_attribute(p_type, 4);
}

unsigned char  type_is_deprecated(struct type * p_type)
{
    return type_has_attribute(p_type, 1);
}

unsigned char  type_is_nodiscard(struct type * p_type)
{
    return type_has_attribute(p_type, 8);
}

unsigned char  type_is_array(struct type * p_type)
{
    return !!(type_get_category(p_type) == 2);
}

unsigned char  type_is_pointed_dtor(struct type * p_type);
unsigned char  type_is_owner(struct type * p_type);

unsigned char  type_is_owner_or_pointer_to_dtor(struct type * p_type)
{
    if (type_is_pointed_dtor(p_type))
    {
        return 1;
    }
    if (type_is_owner(p_type))
    {
        return 1;
    }
    return !!(p_type->type_qualifier_flags & 128);
}

unsigned char  type_is_pointer_to_owner(struct type * p_type)
{
    if (p_type->next == 0U)
    {
        return 0;
    }
    return type_is_owner(p_type->next);
}

unsigned char  type_is_dtor(struct type * p_type)
{
    return !!(p_type->type_qualifier_flags & 128);
}

unsigned char  type_is_pointer(struct type * p_type);

unsigned char  type_is_pointed_dtor(struct type * p_type)
{
    if (!type_is_pointer(p_type))
    {
        return 0;
    }
    ;
    return type_is_dtor(p_type->next);
}

unsigned char  type_is_owner(struct type * p_type)
{
    if (p_type->struct_or_union_specifier)
    {
        struct struct_or_union_specifier * p_complete;

        if (p_type->type_qualifier_flags & 32)
        {
            return 0;
        }
        p_complete = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
        if (p_complete && p_complete->is_owner)
        {
            return 1;
        }
    }
    return !!(p_type->type_qualifier_flags & 16);
}

unsigned char  type_is_opt(struct type * p_type, unsigned char   nullable_enabled)
{
    if (nullable_enabled)
    {
        return !!(p_type->type_qualifier_flags & 64);
    }
    return 1;
}

unsigned char  type_is_view(struct type * p_type)
{
    return !!(p_type->type_qualifier_flags & 32);
}

unsigned char  type_is_ctor(struct type * p_type)
{
    return !!(p_type->type_qualifier_flags & 256);
}

unsigned char  type_is_const(struct type * p_type)
{
    return !!(p_type->type_qualifier_flags & 1);
}

unsigned char  type_is_pointer_to_const(struct type * p_type)
{
    if (p_type->category == 3)
    {
        if (p_type->next)
        {
            return !!(p_type->next->type_qualifier_flags & 1);
        }
    }
    return 0;
}

unsigned char  type_is_void_ptr(struct type * p_type)
{
    if (p_type->category == 3)
    {
        if (p_type->next)
        {
            return !!(p_type->next->type_specifier_flags & 1);
        }
    }
    return 0;
}

unsigned char  type_is_void(struct type * p_type)
{
    if (p_type->category == 0)
    {
        return !!(p_type->type_specifier_flags & 1);
    }
    return 0;
}

unsigned char  type_is_nullptr_t(struct type * p_type)
{
    if (p_type->category == 0)
    {
        return !!(p_type->type_specifier_flags & 16777216);
    }
    return 0;
}

unsigned char  type_is_pointer_to_out(struct type * p_type)
{
    if (p_type->next == 0U)
    {
        return 0;
    }
    if (p_type->category == 3)
    {
        return !!(p_type->next->type_qualifier_flags & 256);
    }
    return 0;
}

unsigned char  type_is_pointer(struct type * p_type)
{
    return !!(p_type->category == 3);
}

unsigned char  type_is_essential_bool(struct type * p_type)
{
    return !!(p_type->attributes_flags & 33554432);
}

unsigned char  type_is_essential_char(struct type * p_type)
{
    return !!(p_type->attributes_flags & 67108864);
}

unsigned char  type_is_enum(struct type * p_type)
{
    return !!(type_get_category(p_type) == 0 && p_type->type_specifier_flags & 65536);
}

unsigned char  type_is_struct_or_union(struct type * p_type)
{
    return !!(type_get_category(p_type) == 0 && p_type->type_specifier_flags & 32768);
}

unsigned char  type_is_union(struct type * p_type)
{
    if (type_get_category(p_type) != 0)
    {
        return 0;
    }
    if (p_type->struct_or_union_specifier == 0U)
    {
        return 0;
    }
    return !!(p_type->struct_or_union_specifier->first_token->type == 9038);
}

unsigned char  type_is_character(struct type * p_type)
{
    return !!(type_get_category(p_type) == 0 && p_type->type_specifier_flags & 2);
}

unsigned char  object_has_constant_value(struct object * a);

unsigned char  type_is_vla(struct type * p_type)
{
    struct type * it;

    it = p_type;
    while (it && type_is_array(it))
    {
        if (it->array_num_elements_expression)
        {
            if (!object_has_constant_value(&it->array_num_elements_expression->object))
            {
                return 1;
            }
        }
        it = it->next;
    }
    return 0;
}

unsigned char  type_is_decimal128(struct type * p_type)
{
    return !!(type_get_category(p_type) == 0 && p_type->type_specifier_flags & 8192);
}

unsigned char  type_is_decimal64(struct type * p_type)
{
    return !!(type_get_category(p_type) == 0 && p_type->type_specifier_flags & 4096);
}

unsigned char  type_is_decimal32(struct type * p_type)
{
    return !!(type_get_category(p_type) == 0 && p_type->type_specifier_flags & 2048);
}

unsigned char  type_is_long_double(struct type * p_type)
{
    if (type_get_category(p_type) != 0)
    {
        return 0;
    }
    if (p_type->type_specifier_flags & 64)
    {
        if (p_type->type_specifier_flags & 16)
        {
            return 1;
        }
    }
    return 0;
}

unsigned char  type_is_double(struct type * p_type)
{
    if (type_get_category(p_type) != 0)
    {
        return 0;
    }
    if (p_type->type_specifier_flags & 64)
    {
        if (!(p_type->type_specifier_flags & 16))
        {
            return 1;
        }
    }
    return 0;
}

unsigned char  type_is_int(struct type * p_type)
{
    if (type_get_category(p_type) != 0)
    {
        return 0;
    }
    if ((p_type->type_specifier_flags == 136) || (p_type->type_specifier_flags == 8))
    {
        return 1;
    }
    return 0;
}

unsigned char  type_is_unsigned_int(struct type * p_type)
{
    if (type_get_category(p_type) != 0)
    {
        return 0;
    }
    if (p_type->type_specifier_flags == 264)
    {
        return 1;
    }
    return 0;
}

unsigned char  type_is_float(struct type * p_type)
{
    if (type_get_category(p_type) != 0)
    {
        return 0;
    }
    if (p_type->type_specifier_flags & 32)
    {
        return 1;
    }
    return 0;
}

unsigned char  type_is_bool(struct type * p_type)
{
    return !!(type_get_category(p_type) == 0 && p_type->type_specifier_flags & 512);
}

unsigned char  type_is_floating_point(struct type * p_type)
{
    if (type_get_category(p_type) != 0)
    {
        return 0;
    }
    return !!(p_type->type_specifier_flags & 96);
}

unsigned char  type_is_integer(struct type * p_type);

unsigned char  type_is_unsigned_integer(struct type * p_type)
{
    if (type_is_integer(p_type) && (p_type->type_specifier_flags & 256))
    {
        return 1;
    }
    return 0;
}

unsigned char  type_is_signed_integer(struct type * p_type)
{
    if (type_is_integer(p_type) && !(p_type->type_specifier_flags & 256))
    {
        return 1;
    }
    return 0;
}

unsigned char  type_is_array_of_char(struct type * p_type)
{
    if (p_type->category != 2)
    {
        return 0;
    }
    ;
    return !!(p_type->next->type_specifier_flags & 2);
}

unsigned char  type_is_char(struct type * p_type)
{
    if (type_get_category(p_type) != 0)
    {
        return 0;
    }
    return !!(p_type->type_specifier_flags & 2);
}

unsigned char  type_is_integer(struct type * p_type)
{
    if (type_get_category(p_type) != 0)
    {
        return 0;
    }
    if (p_type->type_specifier_flags & 64)
    {
        return 0;
    }
    if (p_type->type_specifier_flags & 65536)
    {
        return 1;
    }
    return !!(p_type->type_specifier_flags & 8127390);
}

unsigned char  type_is_arithmetic(struct type * p_type)
{
    return !!(type_is_integer(p_type) || type_is_floating_point(p_type));
}

unsigned char  type_is_scalar(struct type * p_type)
{
    if (type_is_arithmetic(p_type))
    {
        return 1;
    }
    if (type_is_pointer(p_type))
    {
        return 1;
    }
    if (type_get_category(p_type) != 0)
    {
        return 0;
    }
    if (p_type->type_specifier_flags & 65536)
    {
        return 1;
    }
    if (p_type->type_specifier_flags & 16777216)
    {
        return 1;
    }
    if (p_type->type_specifier_flags & 512)
    {
        return 1;
    }
    return 0;
}

struct param_list *type_get_func_or_func_ptr_params(struct type * p_type)
{
    if (p_type->category == 1)
    {
        return &p_type->params;
    }
    else
    {
        if (p_type->category == 3)
        {
            if (p_type->next && p_type->next->category == 1)
            {
                return &p_type->next->params;
            }
        }
    }
    return 0U;
}

unsigned char  compiler_diagnostic(int w, struct parser_ctx * ctx, struct token * p_token, struct marker * p_marker, char * fmt, ...);
unsigned char  expression_is_null_pointer_constant(struct expression * expression);
struct type type_remove_pointer(struct type * p_type);

void check_ownership_qualifiers_of_argument_and_parameter(struct parser_ctx * ctx, struct argument_expression * current_argument, struct type * paramer_type, int param_num)
{
    unsigned char   paramer_is_obj_owner;
    unsigned char   paramer_is_owner;
    unsigned char   paramer_is_view;
    struct type * argument_type;
    unsigned char   argument_is_owner;
    unsigned char   argument_is_obj_owner;
    unsigned char   argument_is_view;

    paramer_is_obj_owner = type_is_pointed_dtor(paramer_type);
    paramer_is_owner = type_is_owner(paramer_type);
    paramer_is_view = !!(!paramer_is_obj_owner && !paramer_is_owner);
    argument_type = &current_argument->expression->type;
    argument_is_owner = type_is_owner(&current_argument->expression->type);
    argument_is_obj_owner = type_is_pointed_dtor(&current_argument->expression->type);
    argument_is_view = !!(!argument_is_owner && !argument_is_obj_owner);
    if (argument_is_owner && paramer_is_owner)
    {
    }
    else
    {
        if (argument_is_owner && paramer_is_obj_owner)
        {
        }
        else
        {
            if (argument_is_owner && paramer_is_view)
            {
                if (current_argument->expression->type.storage_class_specifier_flags & 8192)
                {
                    compiler_diagnostic(22, ctx, current_argument->expression->first_token, 0U, "passing a temporary owner to a view");
                }
            }
            else
            {
                if (argument_is_obj_owner && paramer_is_owner)
                {
                    compiler_diagnostic(23, ctx, current_argument->expression->first_token, 0U, "cannot move _Dtor to _Owner");
                }
                else
                {
                    if (argument_is_obj_owner && paramer_is_obj_owner)
                    {
                    }
                    else
                    {
                        if (argument_is_obj_owner && paramer_is_view)
                        {
                            if (current_argument->expression->type.storage_class_specifier_flags & 8192)
                            {
                                compiler_diagnostic(22, ctx, current_argument->expression->first_token, 0U, "passing a temporary owner to a view");
                            }
                        }
                        else
                        {
                            if (argument_is_view && paramer_is_owner)
                            {
                                if (!expression_is_null_pointer_constant(current_argument->expression))
                                {
                                    compiler_diagnostic(23, ctx, current_argument->expression->first_token, 0U, "passing a _View argument to a _Owner parameter");
                                }
                            }
                            else
                            {
                                if (argument_is_view && paramer_is_obj_owner)
                                {
                                    if (type_is_pointer(argument_type))
                                    {
                                        struct type  t2;

                                        t2 = type_remove_pointer(argument_type);
                                        if (!type_is_owner(&t2))
                                        {
                                            compiler_diagnostic(23, ctx, current_argument->expression->first_token, 0U, "pointed object is not _Owner");
                                        }
                                        else
                                        {
                                            if (!argument_type->address_of)
                                            {
                                                compiler_diagnostic(18, ctx, current_argument->expression->first_token, 0U, "_Dtor pointer must be created using address of operator &");
                                            }
                                        }
                                        type_destroy(&t2);
                                    }
                                    else
                                    {
                                        if (!expression_is_null_pointer_constant(current_argument->expression))
                                        {
                                            compiler_diagnostic(23, ctx, current_argument->expression->first_token, 0U, "passing a _View argument to a _Dtor parameter");
                                        }
                                    }
                                }
                                else
                                {
                                    if (argument_is_view && paramer_is_view)
                                    {
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

unsigned char  expression_is_subjected_to_lvalue_conversion(struct expression *);
unsigned char  type_is_same(struct type * a, struct type * b, unsigned char   compare_qualifiers);
unsigned char  type_is_pointer_or_array(struct type * p_type);

void check_argument_and_parameter(struct parser_ctx * ctx, struct argument_expression * current_argument, struct type * paramer_type, int param_num)
{
    struct type * argument_type;
    unsigned char   is_null_pointer_constant;
    struct type  parameter_type_converted;
    struct type  argument_type_converted;

    if (type_is_owner_or_pointer_to_dtor(paramer_type))
    {
        if (type_is_pointed_dtor(paramer_type))
        {
            if (current_argument->expression->type.category == 3)
            {
                if (type_is_pointer(&current_argument->expression->type) && !type_is_pointer_to_owner(&current_argument->expression->type))
                {
                    compiler_diagnostic(21, ctx, current_argument->expression->first_token, 0U, "parameter %d requires a pointer to _Owner object", param_num);
                }
            }
            else
            {
                compiler_diagnostic(21, ctx, current_argument->expression->first_token, 0U, "parameter %d requires a pointer to _Owner type", param_num);
            }
        }
    }
    argument_type = &current_argument->expression->type;
    is_null_pointer_constant = expression_is_null_pointer_constant(current_argument->expression);
    parameter_type_converted = (type_is_array(paramer_type)) ? type_lvalue_conversion(paramer_type, ctx->options.null_checks_enabled) : type_dup(paramer_type);
    argument_type_converted = expression_is_subjected_to_lvalue_conversion(current_argument->expression) ? type_lvalue_conversion(argument_type, ctx->options.null_checks_enabled) : type_dup(argument_type);
    if (type_is_enum(argument_type) && type_is_enum(paramer_type))
    {
        if (!type_is_same(argument_type, paramer_type, 0))
        {
            compiler_diagnostic(950, ctx, current_argument->expression->first_token, 0U, " incompatible types at argument %d", param_num);
        }
        check_ownership_qualifiers_of_argument_and_parameter(ctx, current_argument, paramer_type, param_num);
        type_destroy(&parameter_type_converted);
        type_destroy(&argument_type_converted);
        return;
    }
    if (type_is_arithmetic(argument_type) && type_is_arithmetic(paramer_type))
    {
        check_ownership_qualifiers_of_argument_and_parameter(ctx, current_argument, paramer_type, param_num);
        type_destroy(&parameter_type_converted);
        type_destroy(&argument_type_converted);
        return;
    }
    if (is_null_pointer_constant && type_is_pointer(paramer_type))
    {
        check_ownership_qualifiers_of_argument_and_parameter(ctx, current_argument, paramer_type, param_num);
        type_destroy(&parameter_type_converted);
        type_destroy(&argument_type_converted);
        return;
    }
    if (is_null_pointer_constant && type_is_array(paramer_type))
    {
        compiler_diagnostic(27, ctx, current_argument->expression->first_token, 0U, " passing null as array");
        check_ownership_qualifiers_of_argument_and_parameter(ctx, current_argument, paramer_type, param_num);
        type_destroy(&parameter_type_converted);
        type_destroy(&argument_type_converted);
        return;
    }
    if (type_is_pointer_or_array(argument_type) && type_is_pointer_or_array(paramer_type))
    {
        if (type_is_void_ptr(argument_type))
        {
            check_ownership_qualifiers_of_argument_and_parameter(ctx, current_argument, paramer_type, param_num);
            type_destroy(&parameter_type_converted);
            type_destroy(&argument_type_converted);
            return;
        }
        if (type_is_void_ptr(paramer_type))
        {
            check_ownership_qualifiers_of_argument_and_parameter(ctx, current_argument, paramer_type, param_num);
            type_destroy(&parameter_type_converted);
            type_destroy(&argument_type_converted);
            return;
        }
        if (type_is_array(paramer_type))
        {
            unsigned long long parameter_array_size;

            parameter_array_size = paramer_type->num_of_elements;
            if (type_is_array(argument_type))
            {
                unsigned long long argument_array_size;

                argument_array_size = argument_type->num_of_elements;
                if (parameter_array_size != 0 && argument_array_size < parameter_array_size)
                {
                    compiler_diagnostic(1130, ctx, current_argument->expression->first_token, 0U, " argument of size [%d] is smaller than parameter of size [%d]", argument_array_size, parameter_array_size);
                }
            }
            else
            {
                if (is_null_pointer_constant || type_is_nullptr_t(argument_type))
                {
                    compiler_diagnostic(38, ctx, current_argument->expression->first_token, 0U, " passing null as array");
                }
            }
        }
        if (!type_is_same(&argument_type_converted, &parameter_type_converted, 0))
        {
            type_print(&argument_type_converted);
            type_print(&parameter_type_converted);
            compiler_diagnostic(950, ctx, current_argument->expression->first_token, 0U, " incompatible types at argument %d", param_num);
        }
        if (type_is_pointer(&argument_type_converted) && type_is_pointer(&parameter_type_converted))
        {
            struct type  argument_pointer_to;
            struct type  parameter_pointer_to;

            argument_pointer_to = type_remove_pointer(&argument_type_converted);
            parameter_pointer_to = type_remove_pointer(&parameter_type_converted);
            if (type_is_const(&argument_pointer_to) && !type_is_const(&parameter_pointer_to) && !type_is_owner_or_pointer_to_dtor(&parameter_pointer_to))
            {
                compiler_diagnostic(14, ctx, current_argument->expression->first_token, 0U, " discarding const at argument %d", param_num);
            }
            type_destroy(&argument_pointer_to);
            type_destroy(&parameter_pointer_to);
        }
    }
    check_ownership_qualifiers_of_argument_and_parameter(ctx, current_argument, paramer_type, param_num);
    type_destroy(&argument_type_converted);
    type_destroy(&parameter_type_converted);
}

unsigned char  type_is_function(struct type * p_type)
{
    return !!(type_get_category(p_type) == 1);
}

unsigned char  type_is_function_or_function_pointer(struct type * p_type)
{
    if (type_is_function(p_type))
    {
        return 1;
    }
    if (type_is_pointer(p_type))
    {
        struct type  t;
        unsigned char   r;

        t = type_remove_pointer(p_type);
        r = type_is_function(&t);
        type_destroy(&t);
        return r;
    }
    return 0;
}

unsigned char  type_is_empty(struct type * p_type)
{
    return !!(p_type->category == 0 && p_type->type_specifier_flags == 0);
}

void *calloc(unsigned int nmemb, unsigned int size);

struct type type_add_pointer(struct type * p_type, unsigned char   null_checks_enabled)
{
    struct type  r;

    r = type_dup(p_type);
    /*try*/ if (1)
    {
        struct type * p;
        struct type  __cmp_lt_2;

        p = calloc(1, 60U);
        if (p == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        *p = r;
        __cmp_lt_2.category = 0;
        __cmp_lt_2.attributes_flags = 0;
        __cmp_lt_2.type_specifier_flags = 0;
        __cmp_lt_2.type_qualifier_flags = 0;
        __cmp_lt_2.storage_class_specifier_flags = 0;
        __cmp_lt_2.name_opt = 0;
        __cmp_lt_2.struct_or_union_specifier = 0;
        __cmp_lt_2.enum_specifier = 0;
        __cmp_lt_2.array_num_elements_expression = 0;
        __cmp_lt_2.num_of_elements = 0;
        __cmp_lt_2.has_static_array_size = 0;
        __cmp_lt_2.address_of = 0;
        __cmp_lt_2.params.is_var_args = 0;
        __cmp_lt_2.params.is_void = 0;
        __cmp_lt_2.params.head = 0;
        __cmp_lt_2.params.tail = 0;
        __cmp_lt_2.next = 0;
        r = __cmp_lt_2;
        r.next = p;
        r.category = 3;
        r.storage_class_specifier_flags = p_type->storage_class_specifier_flags;
    }
    /*catch*/ else _CKL0:
    {
    }
    return r;
}

struct type type_remove_pointer(struct type * p_type)
{
    struct type  r;

    r = type_dup(p_type);
    if (!type_is_pointer(p_type))
    {
        return r;
    }
    if (r.next)
    {
        struct type  next;

        next = *r.next;
        free(r.next);
        r.next = 0U;
        type_destroy_one(&r);
        r = next;
    }
    else
    {
        ;
    }
    ;
    r.storage_class_specifier_flags = p_type->next->storage_class_specifier_flags;
    r.type_qualifier_flags = p_type->next->type_qualifier_flags;
    return r;
}

struct type get_array_item_type(struct type * p_type)
{
    struct type  r;

    r = type_dup(p_type);
    if (r.next)
    {
        struct type  r2;

        r2 = *r.next;
        free(r.next);
        free((void *)r.name_opt);
        param_list_destroy(&r.params);
        return r2;
    }
    return r;
}

struct type type_param_array_to_pointer(struct type * p_type, unsigned char   null_checks_enabled)
{
    struct type  t;
    struct type  t2;

    ;
    t = get_array_item_type(p_type);
    t2 = type_add_pointer(&t, null_checks_enabled);
    if (p_type->type_qualifier_flags & 1)
    {
        t2.type_qualifier_flags |= 1;
    }
    type_destroy(&t);
    t2.storage_class_specifier_flags &= -2049;
    return t2;
}

unsigned char  type_is_pointer_or_array(struct type * p_type)
{
    int category;

    category = type_get_category(p_type);
    if (category == 3 || category == 2)
    {
        return 1;
    }
    if (category == 0 && p_type->type_specifier_flags == 16777216)
    {
        return 1;
    }
    return 0;
}

int type_get_integer_rank(struct type * p_type1)
{
    if (type_is_pointer_or_array(p_type1))
    {
        ;
        return 40;
    }
    if ((p_type1->type_specifier_flags & 4194304) || (p_type1->type_specifier_flags & 2097152))
    {
        return 80;
    }
    else
    {
        if ((p_type1->type_specifier_flags & 16777216))
        {
            return 50;
        }
        else
        {
            if ((p_type1->type_specifier_flags & 16) || (p_type1->type_specifier_flags & 1048576))
            {
                return 50;
            }
            else
            {
                if ((p_type1->type_specifier_flags & 8) || (p_type1->type_specifier_flags & 65536))
                {
                    return 40;
                }
                else
                {
                    if ((p_type1->type_specifier_flags & 4) || (p_type1->type_specifier_flags & 524288))
                    {
                        return 30;
                    }
                    else
                    {
                        if ((p_type1->type_specifier_flags & 2) || (p_type1->type_specifier_flags & 262144))
                        {
                            return 20;
                        }
                        else
                        {
                            if ((p_type1->type_specifier_flags & 512))
                            {
                                return 10;
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

struct type type_make_int();

struct type type_get_enum_underlying_type(struct type * p)
{
    struct type  r;

    r = type_make_int();
    return r;
}

void type_swap(struct type * a, struct type * b);
int type_get_sizeof(struct type * p_type, unsigned int * size);

struct type type_common(struct type * p_type1, struct type * p_type2)
{
    struct type  promoted_a;
    struct type  promoted_b;
    struct type * p_signed_promoted;
    struct type * p_unsigned_promoted;
    unsigned int signed_promoted_sizeof;
    unsigned int unsigned_promoted_sizeof;
    struct type  r;

    if (type_is_decimal128(p_type1))
    {
        return type_dup(p_type1);
    }
    if (type_is_decimal128(p_type2))
    {
        return type_dup(p_type2);
    }
    if (type_is_decimal64(p_type1))
    {
        return type_dup(p_type1);
    }
    if (type_is_decimal64(p_type2))
    {
        return type_dup(p_type2);
    }
    if (type_is_decimal32(p_type1))
    {
        return type_dup(p_type1);
    }
    if (type_is_decimal32(p_type2))
    {
        return type_dup(p_type2);
    }
    if (type_is_long_double(p_type1))
    {
        return type_dup(p_type1);
    }
    if (type_is_long_double(p_type2))
    {
        return type_dup(p_type2);
    }
    if (type_is_double(p_type1))
    {
        return type_dup(p_type1);
    }
    if (type_is_double(p_type2))
    {
        return type_dup(p_type2);
    }
    if (type_is_float(p_type1))
    {
        return type_dup(p_type1);
    }
    if (type_is_float(p_type2))
    {
        return type_dup(p_type2);
    }
    _cake_zmem(&promoted_a, 60);
    _cake_zmem(&promoted_b, 60);
    if (type_is_enum(p_type1))
    {
        promoted_a = type_get_enum_underlying_type(p_type1);
    }
    else
    {
        promoted_a = type_dup(p_type1);
    }
    if (type_is_enum(p_type2))
    {
        promoted_b = type_get_enum_underlying_type(p_type2);
    }
    else
    {
        promoted_b = type_dup(p_type2);
    }
    type_integer_promotion(&promoted_a);
    type_integer_promotion(&promoted_b);
    if (type_is_same(&promoted_a, &promoted_b, 0))
    {
        type_destroy(&promoted_b);
        return promoted_a;
    }
    if (type_is_signed_integer(&promoted_a) == type_is_signed_integer(&promoted_b))
    {
        if (type_get_integer_rank(&promoted_a) > type_get_integer_rank(&promoted_b))
        {
            type_destroy(&promoted_b);
            return promoted_a;
        }
        type_destroy(&promoted_a);
        return promoted_b;
    }
    p_signed_promoted = type_is_signed_integer(&promoted_a) ? &promoted_a : &promoted_b;
    p_unsigned_promoted = type_is_unsigned_integer(&promoted_a) ? &promoted_a : &promoted_b;
    ;
    if (type_get_integer_rank(p_unsigned_promoted) >= type_get_integer_rank(p_signed_promoted))
    {
        struct type  r;

        _cake_zmem(&r, 60);
        type_swap(&r, p_unsigned_promoted);
        type_destroy(&promoted_a);
        type_destroy(&promoted_b);
        return r;
    }
    signed_promoted_sizeof = 0;
    if (type_get_sizeof(p_signed_promoted, &signed_promoted_sizeof) != 0)
    {
        ;
    }
    unsigned_promoted_sizeof = 0;
    if (type_get_sizeof(p_unsigned_promoted, &unsigned_promoted_sizeof) != 0)
    {
        ;
    }
    if (signed_promoted_sizeof > unsigned_promoted_sizeof)
    {
        struct type  r;

        _cake_zmem(&r, 60);
        type_swap(&r, p_signed_promoted);
        type_destroy(&promoted_a);
        type_destroy(&promoted_b);
        return r;
    }
    _cake_zmem(&r, 60);
    type_swap(&r, p_signed_promoted);
    r.type_specifier_flags |= 256;
    type_destroy(&promoted_a);
    type_destroy(&promoted_b);
    return r;
}

void type_set(struct type * a, struct type * b)
{
    struct type  t;

    t = type_dup(b);
    type_swap(&t, a);
    type_destroy(&t);
}

void type_list_destroy(struct type_list * p_type_list);
void type_list_push_back(struct type_list * books, struct type * new_book);

struct type type_dup(struct type * p_type)
{
    struct type  empty;

    /*try*/ if (1)
    {
        struct type_list  l;
        struct type * p;
        struct type  r;

        _cake_zmem(&l, 8);
        p = p_type;
        while (p)
        {
            struct type * p_new;

            p_new = calloc(1, 60U);
            if (p_new == 0U)
            {
                type_list_destroy(&l);
                /*throw*/ goto _CKL0;
            }
            *p_new = *p;
            p_new->next = 0U;
            if (p->name_opt)
            {
                p_new->name_opt = strdup(p->name_opt);
            }
            if (p->category == 1)
            {
                struct param * p_param;

                p_new->params.head = 0U;
                p_new->params.tail = 0U;
                p_param = p->params.head;
                while (p_param)
                {
                    struct param * p_new_param;

                    p_new_param = calloc(1, 64U);
                    if (p_new_param == 0U)
                    {
                        type_list_destroy(&l);
                        type_delete(p_new);
                        /*throw*/ goto _CKL0;
                    }
                    p_new_param->type = type_dup(&p_param->type);
                    param_list_add(&p_new->params, p_new_param);
                    p_param = p_param->next;
                }
            }
            type_list_push_back(&l, p_new);
            p = p->next;
        }
        if (l.head == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        r = *l.head;
        free(l.head);
        return r;
    }
    /*catch*/ else _CKL0:
    {
    }
    _cake_zmem(&empty, 60);
    return empty;
}

unsigned int type_get_alignof(struct type * p_type);

int get_sizeof_struct(struct struct_or_union_specifier * complete_struct_or_union_specifier, unsigned int * sz)
{
    int sizeof_error;
    unsigned char   is_union;
    unsigned int size;

    sizeof_error = 0;
    is_union = !!((complete_struct_or_union_specifier->first_token->type == 9038));
    size = 0;
    /*try*/ if (1)
    {
        int maxalign;
        struct member_declaration * d;

        maxalign = 0;
        d = complete_struct_or_union_specifier->member_declaration_list.head;
        while (d)
        {
            if (d->member_declarator_list_opt)
            {
                struct member_declarator * md;

                md = d->member_declarator_list_opt->head;
                while (md)
                {
                    int align;

                    align = 1;
                    if (md->declarator)
                    {
                        unsigned int item_size;

                        align = type_get_alignof(&md->declarator->type);
                        if (align > maxalign)
                        {
                            maxalign = align;
                        }
                        if (size % align != 0)
                        {
                            size += align - (size % align);
                        }
                        item_size = 0;
                        sizeof_error = type_get_sizeof(&md->declarator->type, &item_size);
                        if (sizeof_error != 0)
                        {
                            /*throw*/ goto _CKL0;
                        }
                        if (is_union)
                        {
                            if (item_size > size)
                            {
                                size = item_size;
                            }
                        }
                        else
                        {
                            size += item_size;
                        }
                    }
                    else
                    {
                        sizeof_error = 3;
                        /*throw*/ goto _CKL0;
                    }
                    md = md->next;
                }
            }
            else
            {
                if (d->specifier_qualifier_list)
                {
                    if (d->specifier_qualifier_list->struct_or_union_specifier)
                    {
                        struct type  t;
                        int align;
                        unsigned int item_size;

                        _cake_zmem(&t, 60);
                        t.category = 0;
                        t.struct_or_union_specifier = d->specifier_qualifier_list->struct_or_union_specifier;
                        t.type_specifier_flags = 32768;
                        align = type_get_alignof(&t);
                        if (align > maxalign)
                        {
                            maxalign = align;
                        }
                        if (size % align != 0)
                        {
                            size += align - (size % align);
                        }
                        item_size = 0;
                        sizeof_error = type_get_sizeof(&t, &item_size);
                        if (sizeof_error != 0)
                        {
                            /*throw*/ goto _CKL0;
                        }
                        if (is_union)
                        {
                            if (item_size > size)
                            {
                                size = item_size;
                            }
                        }
                        else
                        {
                            size += item_size;
                        }
                        type_destroy(&t);
                    }
                    else
                    {
                        sizeof_error = 3;
                        /*throw*/ goto _CKL0;
                    }
                }
            }
            d = d->next;
        }
        if (maxalign != 0)
        {
            if (size % maxalign != 0)
            {
                size += maxalign - (size % maxalign);
            }
        }
        else
        {
            sizeof_error = 3;
            /*throw*/ goto _CKL0;
        }
    }
    /*catch*/ else _CKL0:
    {
        return sizeof_error;
    }
    *sz = size;
    return sizeof_error;
}

unsigned int get_alignof_struct(struct struct_or_union_specifier * complete_struct_or_union_specifier)
{
    int align;
    struct member_declaration * d;

    align = 0;
    d = complete_struct_or_union_specifier->member_declaration_list.head;
    while (d)
    {
        if (d->member_declarator_list_opt)
        {
            struct member_declarator * md;

            md = d->member_declarator_list_opt->head;
            while (md)
            {
                if (md->declarator)
                {
                    int temp_align;

                    temp_align = type_get_alignof(&md->declarator->type);
                    if (temp_align > align)
                    {
                        align = temp_align;
                    }
                }
                else
                {
                    ;
                }
                md = md->next;
            }
        }
        else
        {
            if (d->specifier_qualifier_list)
            {
                struct type  type;
                int temp_align;

                _cake_zmem(&type, 60);
                type.type_specifier_flags = d->specifier_qualifier_list->type_specifier_flags;
                type.enum_specifier = d->specifier_qualifier_list->enum_specifier;
                type.struct_or_union_specifier = d->specifier_qualifier_list->struct_or_union_specifier;
                temp_align = type_get_alignof(&type);
                if (temp_align > align)
                {
                    align = temp_align;
                }
                type_destroy(&type);
            }
            else
            {
            }
        }
        d = d->next;
    }
    ;
    return align;
}

unsigned int type_get_alignof(struct type * p_type)
{
    int align;
    int category;

    align = 0;
    category = type_get_category(p_type);
    if (category == 3)
    {
        align = 4U;
    }
    else
    {
        if (category == 1)
        {
            align = -1;
        }
        else
        {
            if (category == 0)
            {
                if (p_type->type_specifier_flags & 2)
                {
                    align = 1U;
                }
                else
                {
                    if (p_type->type_specifier_flags & 512)
                    {
                        align = 1U;
                    }
                    else
                    {
                        if (p_type->type_specifier_flags & 4)
                        {
                            align = 2U;
                        }
                        else
                        {
                            if (p_type->type_specifier_flags & 65536)
                            {
                                align = 4U;
                            }
                            else
                            {
                                if (p_type->type_specifier_flags & 16)
                                {
                                    align = 4U;
                                }
                                else
                                {
                                    if (p_type->type_specifier_flags & 4194304)
                                    {
                                        align = 8U;
                                    }
                                    else
                                    {
                                        if (p_type->type_specifier_flags & 8)
                                        {
                                            align = 4U;
                                        }
                                        else
                                        {
                                            if (p_type->type_specifier_flags & 2097152)
                                            {
                                                align = 8U;
                                            }
                                            else
                                            {
                                                if (p_type->type_specifier_flags & 1048576)
                                                {
                                                    align = 4U;
                                                }
                                                else
                                                {
                                                    if (p_type->type_specifier_flags & 524288)
                                                    {
                                                        align = 2U;
                                                    }
                                                    else
                                                    {
                                                        if (p_type->type_specifier_flags & 262144)
                                                        {
                                                            align = 1U;
                                                        }
                                                        else
                                                        {
                                                            if (p_type->type_specifier_flags & 32)
                                                            {
                                                                align = 4U;
                                                            }
                                                            else
                                                            {
                                                                if (p_type->type_specifier_flags & 64)
                                                                {
                                                                    align = 8U;
                                                                }
                                                                else
                                                                {
                                                                    if (p_type->type_specifier_flags & 32768)
                                                                    {
                                                                        if (p_type->struct_or_union_specifier)
                                                                        {
                                                                            struct struct_or_union_specifier * p_complete;

                                                                            p_complete = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
                                                                            align = 1;
                                                                            if (p_complete)
                                                                            {
                                                                                align = get_alignof_struct(p_complete);
                                                                            }
                                                                            else
                                                                            {
                                                                                align = -2;
                                                                            }
                                                                        }
                                                                        else
                                                                        {
                                                                            align = -2;
                                                                            ;
                                                                        }
                                                                    }
                                                                    else
                                                                    {
                                                                        if (p_type->type_specifier_flags & 65536)
                                                                        {
                                                                            align = 4U;
                                                                        }
                                                                        else
                                                                        {
                                                                            if (p_type->type_specifier_flags == 0)
                                                                            {
                                                                                align = -3;
                                                                            }
                                                                            else
                                                                            {
                                                                                if (p_type->type_specifier_flags == 1)
                                                                                {
                                                                                    align = 1;
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
            else
            {
                if (category == 2)
                {
                    struct type  type;

                    type = get_array_item_type(p_type);
                    align = type_get_alignof(&type);
                    type_destroy(&type);
                }
            }
        }
    }
    ;
    return align;
}

unsigned char  unsigned_long_long_mul(unsigned long long * result, unsigned long long a, unsigned long long b);
int get_enum_type_specifier_flags(struct enum_specifier * p_enum_specifier);
struct type make_with_type_specifier_flags(int f);

int type_get_sizeof(struct type * p_type, unsigned int * size)
{
    int category;

    *size = 0;
    category = type_get_category(p_type);
    if (category == 3)
    {
        *size = 4U;
        return 0;
    }
    if (category == 1)
    {
        return 4;
    }
    if (category == 2)
    {
        if (p_type->storage_class_specifier_flags & 2048)
        {
            *size = 4U;
            return 0;
        }
        else
        {
            unsigned long long arraysize;
            struct type  type;
            unsigned int sz;
            int er;
            unsigned long long result;

            if (type_is_vla(p_type))
            {
                return 2;
            }
            arraysize = p_type->num_of_elements;
            type = get_array_item_type(p_type);
            sz = 0;
            er = type_get_sizeof(&type, &sz);
            if (er != 0)
            {
                return er;
            }
            type_destroy(&type);
            result = 0;
            if (unsigned_long_long_mul(&result, sz, arraysize))
            {
                if (result > 4294967295U)
                {
                    return 1;
                }
                *size = (unsigned int)result;
            }
            else
            {
                return 1;
            }
            return 0;
        }
    }
    ;
    if (p_type->type_specifier_flags & 2)
    {
        *size = 1U;
        return 0;
    }
    if (p_type->type_specifier_flags & 512)
    {
        *size = 1U;
        return 0;
    }
    if (p_type->type_specifier_flags & 4)
    {
        *size = 2U;
        return 0;
    }
    if (p_type->enum_specifier)
    {
        int enum_type_specifier_flags;
        struct type  t;
        int e;

        ;
        enum_type_specifier_flags = get_enum_type_specifier_flags(p_type->enum_specifier);
        t = make_with_type_specifier_flags(enum_type_specifier_flags);
        e = type_get_sizeof(&t, size);
        type_destroy(&t);
        return e;
    }
    if (p_type->type_specifier_flags & 16)
    {
        *size = 4U;
        return 0;
    }
    if (p_type->type_specifier_flags & 4194304)
    {
        *size = 8U;
        return 0;
    }
    if (p_type->type_specifier_flags & 8)
    {
        *size = 4U;
        return 0;
    }
    if (p_type->type_specifier_flags & 2097152)
    {
        *size = 8U;
        return 0;
    }
    if (p_type->type_specifier_flags & 1048576)
    {
        *size = 4;
        return 0;
    }
    if (p_type->type_specifier_flags & 524288)
    {
        *size = 2;
        return 0;
    }
    if (p_type->type_specifier_flags & 262144)
    {
        *size = 1;
        return 0;
    }
    if (p_type->type_specifier_flags & 32)
    {
        *size = 4U;
        return 0;
    }
    if (p_type->type_specifier_flags & 64)
    {
        *size = 8U;
        return 0;
    }
    if (p_type->type_specifier_flags & 32768)
    {
        struct struct_or_union_specifier * p_complete;

        if (p_type->struct_or_union_specifier == 0U)
        {
            return 3;
        }
        p_complete = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
        if (p_complete == 0U)
        {
            return 3;
        }
        return get_sizeof_struct(p_complete, size);
    }
    if (p_type->type_specifier_flags & 65536)
    {
        *size = 4U;
        return 0;
    }
    if (p_type->type_specifier_flags == 0)
    {
        *size = 0;
        return 3;
    }
    if (p_type->type_specifier_flags == 1)
    {
        *size = 1;
        return 0;
    }
    if (p_type->type_specifier_flags == 16777216)
    {
        *size = 4U;
        return 0;
    }
    if (p_type->type_specifier_flags == 2048)
    {
        *size = 4;
        return 0;
    }
    if (p_type->type_specifier_flags == 4096)
    {
        *size = 8;
        return 0;
    }
    if (p_type->type_specifier_flags == 8192)
    {
        *size = 16;
        return 0;
    }
    return 3;
}

void type_set_attributes(struct type * p_type, struct declarator * pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        p_type->attributes_flags = pdeclarator->declaration_specifiers->attributes_flags;
    }
    else
    {
        if (pdeclarator->specifier_qualifier_list)
        {
        }
    }
}

struct type get_function_return_type(struct type * p_type);

unsigned char  function_returns_void(struct type * p_type)
{
    struct type  t;
    unsigned char   r;

    t = get_function_return_type(p_type);
    r = type_is_void(&t);
    type_destroy(&t);
    return r;
}

struct type get_function_return_type(struct type * p_type)
{
    struct type  empty;

    /*try*/ if (1)
    {
        struct type  r;

        if (p_type->next == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        if (type_is_pointer(p_type))
        {
            struct type  r;

            if (p_type->next->next == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            r = type_dup(p_type->next->next);
            return r;
        }
        r = type_dup(p_type->next);
        return r;
    }
    /*catch*/ else _CKL0:
    {
    }
    _cake_zmem(&empty, 60);
    return empty;
}

void type_set_int(struct type * p_type)
{
    p_type->type_specifier_flags = 8;
    p_type->type_qualifier_flags = 0;
    p_type->category = 0;
}

struct type type_make_enumerator(struct enum_specifier * enum_specifier)
{
    struct type  t;

    _cake_zmem(&t, 60);
    t.type_specifier_flags = 65536;
    t.enum_specifier = enum_specifier;
    t.category = 0;
    return t;
}

struct type type_get_enum_type(struct type * p_type)
{
    struct type  empty;

    /*try*/ if (1)
    {
        struct enum_specifier * p_complete_enum_specifier;
        struct type  t;

        if (p_type->enum_specifier == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_complete_enum_specifier = get_complete_enum_specifier(p_type->enum_specifier);
        if (p_complete_enum_specifier && p_complete_enum_specifier->specifier_qualifier_list)
        {
            struct type  t;

            _cake_zmem(&t, 60);
            t.type_qualifier_flags = p_complete_enum_specifier->specifier_qualifier_list->type_qualifier_flags;
            t.type_specifier_flags = p_complete_enum_specifier->specifier_qualifier_list->type_specifier_flags;
            return t;
        }
        _cake_zmem(&t, 60);
        t.type_specifier_flags = 8;
        return t;
    }
    /*catch*/ else _CKL0:
    {
    }
    _cake_zmem(&empty, 60);
    return empty;
}

struct type type_make_long_double()
{
    struct type  t;

    _cake_zmem(&t, 60);
    t.type_specifier_flags = 80;
    t.category = 0;
    return t;
}

struct type type_make_double()
{
    struct type  t;

    _cake_zmem(&t, 60);
    t.type_specifier_flags = 64;
    t.category = 0;
    return t;
}

struct type type_make_float()
{
    struct type  t;

    _cake_zmem(&t, 60);
    t.type_specifier_flags = 32;
    t.category = 0;
    return t;
}

struct type type_make_size_t()
{
    struct type  t;

    _cake_zmem(&t, 60);
    t.type_specifier_flags = 264;
    t.category = 0;
    return t;
}

struct type make_void_ptr_type()
{
    struct type  t;

    _cake_zmem(&t, 60);
    /*try*/ if (1)
    {
        struct type * p;

        p = calloc(1, 60U);
        if (p == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        t.category = 3;
        p->category = 0;
        p->type_specifier_flags = 1;
        t.next = p;
    }
    /*catch*/ else _CKL0:
    {
    }
    return t;
}

struct type make_void_type()
{
    struct type  t;

    _cake_zmem(&t, 60);
    t.type_specifier_flags = 1;
    t.category = 0;
    return t;
}

struct type type_make_int_bool_like()
{
    struct type  t;

    _cake_zmem(&t, 60);
    t.type_specifier_flags = 8;
    t.attributes_flags = 33554432;
    t.category = 0;
    return t;
}

struct type make_with_type_specifier_flags(int f)
{
    struct type  t;

    _cake_zmem(&t, 60);
    t.type_specifier_flags = f;
    t.category = 0;
    return t;
}

struct type make_size_t_type()
{
    struct type  t;

    _cake_zmem(&t, 60);
    t.type_specifier_flags = 264;
    t.category = 0;
    return t;
}

struct type type_make_int()
{
    struct type  t;

    _cake_zmem(&t, 60);
    t.type_specifier_flags = 8;
    t.category = 0;
    return t;
}

struct type type_make_literal_string(int size_in_bytes, int chartype, int qualifiers)
{
    struct type  t;

    _cake_zmem(&t, 60);
    /*try*/ if (1)
    {
        struct type * p2;
        struct type  char_type;
        unsigned int char_size;

        p2 = calloc(1, 60U);
        if (p2 == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        _cake_zmem(&char_type, 60);
        char_type.category = 0;
        char_type.type_specifier_flags = chartype;
        char_size = 0;
        if (type_get_sizeof(&char_type, &char_size) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        if (char_size == 0)
        {
            char_size = 1;
        }
        type_destroy(&char_type);
        t.category = 2;
        t.num_of_elements = size_in_bytes / char_size;
        p2->category = 0;
        p2->type_specifier_flags = chartype;
        p2->type_qualifier_flags = qualifiers;
        t.next = p2;
    }
    /*catch*/ else _CKL0:
    {
    }
    return t;
}

int __cdecl strcmp(char * _Str1, char * _Str2);

unsigned char  struct_or_union_specifier_is_same(struct struct_or_union_specifier * a, struct struct_or_union_specifier * b)
{
    if (a && b)
    {
        struct struct_or_union_specifier * p_complete_a;
        struct struct_or_union_specifier * p_complete_b;

        p_complete_a = get_complete_struct_or_union_specifier(a);
        p_complete_b = get_complete_struct_or_union_specifier(b);
        if (p_complete_a != 0U && p_complete_b != 0U)
        {
            if (p_complete_a != p_complete_b)
            {
                return 0;
            }
            return 1;
        }
        else
        {
            if (a->tagtoken != 0U && b->tagtoken != 0U)
            {
                if (strcmp(a->tagtoken->lexeme, b->tagtoken->lexeme) == 0)
                {
                    return 1;
                }
            }
        }
        return !!(p_complete_a == 0U && p_complete_b == 0U);
    }
    return !!(a == 0U && b == 0U);
}

unsigned char  enum_specifier_is_same(struct enum_specifier * a, struct enum_specifier * b)
{
    if (a && b)
    {
        if (get_complete_enum_specifier(a) && get_complete_enum_specifier(b))
        {
            if (get_complete_enum_specifier(a) != get_complete_enum_specifier(b))
            {
                return 0;
            }
            return 1;
        }
        return !!(get_complete_enum_specifier(a) == 0U && get_complete_enum_specifier(b) == 0U);
    }
    return !!(a == 0U && b == 0U);
}

static unsigned char  type_is_same_core(struct type * a, struct type * b, unsigned char   compare_qualifiers)
{
    struct type * pa;
    struct type * pb;

    pa = a;
    pb = b;
    while (pa && pb)
    {
        int a_flags;
        int b_flags;

        if (pa->num_of_elements != pb->num_of_elements)
        {
            return 0;
        }
        if (pa->category != pb->category)
        {
            return 0;
        }
        if (pa->enum_specifier && pb->enum_specifier && get_complete_enum_specifier(pa->enum_specifier) != get_complete_enum_specifier(pb->enum_specifier))
        {
            return 0;
        }
        if (pa->enum_specifier && !pb->enum_specifier)
        {
        }
        if (!pa->enum_specifier && pb->enum_specifier)
        {
        }
        if (pa->has_static_array_size != pb->has_static_array_size)
        {
            return 0;
        }
        if (pa->category == 1)
        {
            struct param * p_param_a;
            struct param * p_param_b;

            if (pa->params.is_var_args != pb->params.is_var_args)
            {
                return 0;
            }
            if (pa->params.is_void != pb->params.is_void)
            {
                return 0;
            }
            p_param_a = pa->params.head;
            p_param_b = pb->params.head;
            while (p_param_a && p_param_b)
            {
                if (!type_is_same(&p_param_a->type, &p_param_b->type, compare_qualifiers))
                {
                    return 0;
                }
                p_param_a = p_param_a->next;
                p_param_b = p_param_b->next;
            }
            return !!(p_param_a == 0U && p_param_b == 0U);
        }
        if (pa->struct_or_union_specifier && pb->struct_or_union_specifier)
        {
            if (pa->struct_or_union_specifier->complete_struct_or_union_specifier_indirection != pb->struct_or_union_specifier->complete_struct_or_union_specifier_indirection)
            {
            }
            if (strcmp(pa->struct_or_union_specifier->tag_name, pb->struct_or_union_specifier->tag_name) != 0)
            {
                return 0;
            }
        }
        if (compare_qualifiers)
        {
            int aq;
            int bq;
            unsigned int all;

            aq = pa->type_qualifier_flags;
            bq = pb->type_qualifier_flags;
            all = 496;
            aq = aq & ~all;
            bq = bq & ~all;
            if (aq != bq)
            {
                return 0;
            }
        }
        a_flags = pa->type_specifier_flags;
        b_flags = pb->type_specifier_flags;
        if ((a_flags & 2) == 0)
        {
            a_flags &= -129;
        }
        if ((b_flags & 2) == 0)
        {
            b_flags &= -129;
        }
        if (a_flags != b_flags)
        {
            return 0;
        }
        pa = pa->next;
        pb = pb->next;
    }
    return !!(pa == 0U && pb == 0U);
}

unsigned char  type_is_same(struct type * a, struct type * b, unsigned char   compare_qualifiers)
{
    return type_is_same_core(a, b, compare_qualifiers);
}

unsigned char  type_is_compatible(struct type * a, struct type * b)
{
    return type_is_same_core(a, b, 0);
}

void type_clear(struct type * a)
{
    struct type  tmp;

    _cake_zmem(&tmp, 60);
    type_swap(&tmp, a);
    type_destroy(&tmp);
}

void type_swap(struct type * a, struct type * b)
{
    struct type  temp;

    temp = *a;
    *a = *b;
    *b = temp;
}

void type_visit_to_mark_anonymous(struct type * p_type)
{
    if (p_type->struct_or_union_specifier != 0U && p_type->struct_or_union_specifier->has_anonymous_tag)
    {
        if (p_type->struct_or_union_specifier->complete_struct_or_union_specifier_indirection)
        {
            p_type->struct_or_union_specifier->complete_struct_or_union_specifier_indirection->show_anonymous_tag = 1;
        }
        p_type->struct_or_union_specifier->show_anonymous_tag = 1;
    }
}

void type_merge_qualifiers_using_declarator(struct type * p_type, struct declarator * pdeclarator)
{
    int type_qualifier_flags;

    type_qualifier_flags = 0;
    if (pdeclarator->declaration_specifiers)
    {
        type_qualifier_flags = pdeclarator->declaration_specifiers->type_qualifier_flags;
    }
    else
    {
        if (pdeclarator->specifier_qualifier_list)
        {
            type_qualifier_flags = pdeclarator->specifier_qualifier_list->type_qualifier_flags;
        }
    }
    p_type->type_qualifier_flags |= type_qualifier_flags;
}

void type_set_qualifiers_using_declarator(struct type * p_type, struct declarator * pdeclarator)
{
    int type_qualifier_flags;

    type_qualifier_flags = 0;
    if (pdeclarator->declaration_specifiers)
    {
        type_qualifier_flags = pdeclarator->declaration_specifiers->type_qualifier_flags;
    }
    else
    {
        if (pdeclarator->specifier_qualifier_list)
        {
            type_qualifier_flags = pdeclarator->specifier_qualifier_list->type_qualifier_flags;
        }
    }
    p_type->type_qualifier_flags = type_qualifier_flags;
}

void type_set_storage_specifiers_using_declarator(struct type * p_type, struct declarator * pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        p_type->storage_class_specifier_flags |= pdeclarator->declaration_specifiers->storage_class_specifier_flags;
    }
    else
    {
    }
}

void type_set_specifiers_using_declarator(struct type * p_type, struct declarator * pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        p_type->type_specifier_flags = pdeclarator->declaration_specifiers->type_specifier_flags;
        p_type->enum_specifier = pdeclarator->declaration_specifiers->enum_specifier;
        p_type->struct_or_union_specifier = pdeclarator->declaration_specifiers->struct_or_union_specifier;
    }
    else
    {
        if (pdeclarator->specifier_qualifier_list)
        {
            p_type->type_specifier_flags = pdeclarator->specifier_qualifier_list->type_specifier_flags;
            p_type->enum_specifier = pdeclarator->specifier_qualifier_list->enum_specifier;
            p_type->struct_or_union_specifier = pdeclarator->specifier_qualifier_list->struct_or_union_specifier;
        }
    }
}

void type_set_attributes_using_declarator(struct type * p_type, struct declarator * pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        if (pdeclarator->declaration_specifiers->attributes_flags & 8)
        {
            p_type->storage_class_specifier_flags |= 16384;
        }
        p_type->attributes_flags = pdeclarator->declaration_specifiers->attributes_flags;
    }
    else
    {
        if (pdeclarator->specifier_qualifier_list)
        {
        }
    }
}

void type_list_push_front(struct type_list * books, struct type * new_book)
{
    ;
    if (books->head == 0U)
    {
        books->head = new_book;
        books->tail = new_book;
    }
    else
    {
        new_book->next = books->head;
        books->head = new_book;
    }
}

void type_list_destroy(struct type_list * p_type_list)
{
    struct type * item;

    item = p_type_list->head;
    while (item)
    {
        struct type * next;

        next = item->next;
        item->next = 0U;
        type_destroy_one(item);
        free(item);
        item = next;
    }
}

void type_list_push_back(struct type_list * type_list, struct type * new_book)
{
    if (type_list->tail == 0U)
    {
        ;
        type_list->head = new_book;
    }
    else
    {
        ;
        type_list->tail->next = new_book;
    }
    type_list->tail = new_book;
}

void make_type_using_declarator_core(struct parser_ctx * ctx, struct declarator * pdeclarator, char ** ppname, struct type_list * list);
unsigned int array_declarator_get_size(struct array_declarator * p_array_declarator);

void make_type_using_direct_declarator(struct parser_ctx * ctx, struct direct_declarator * pdirectdeclarator, char ** ppname, struct type_list * list)
{
    /*try*/ if (1)
    {
        if (pdirectdeclarator->declarator)
        {
            make_type_using_declarator_core(ctx, pdirectdeclarator->declarator, ppname, list);
        }
        else
        {
            if (pdirectdeclarator->function_declarator)
            {
                struct type * p_func;

                if (pdirectdeclarator->function_declarator->direct_declarator)
                {
                    make_type_using_direct_declarator(ctx, pdirectdeclarator->function_declarator->direct_declarator, ppname, list);
                }
                p_func = calloc(1, 60U);
                if (p_func == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
                p_func->category = 1;
                ;
                if (pdirectdeclarator->function_declarator->direct_declarator->p_calling_convention)
                {
                    char * calling_convention_lexeme;

                    calling_convention_lexeme = pdirectdeclarator->function_declarator->direct_declarator->p_calling_convention->lexeme;
                    if (strcmp(calling_convention_lexeme, "__fastcall") == 0)
                    {
                        p_func->attributes_flags |= 134217728;
                    }
                    else
                    {
                        if (strcmp(calling_convention_lexeme, "__stdcall") == 0)
                        {
                            p_func->attributes_flags |= 268435456;
                        }
                        else
                        {
                            if (strcmp(calling_convention_lexeme, "__cdecl") == 0)
                            {
                                p_func->attributes_flags |= 536870912;
                            }
                            else
                            {
                                type_delete(p_func);
                                /*throw*/ goto _CKL0;
                            }
                        }
                    }
                }
                if (pdirectdeclarator->function_declarator->parameter_type_list_opt && pdirectdeclarator->function_declarator->parameter_type_list_opt->parameter_list)
                {
                    struct parameter_declaration * p;

                    p = pdirectdeclarator->function_declarator->parameter_type_list_opt->parameter_list->head;
                    p_func->params.is_var_args = pdirectdeclarator->function_declarator->parameter_type_list_opt->is_var_args;
                    p_func->params.is_void = pdirectdeclarator->function_declarator->parameter_type_list_opt->is_void;
                    while (p)
                    {
                        struct param * p_new_param;

                        if (p->declarator == 0U)
                        {
                            type_delete(p_func);
                            /*throw*/ goto _CKL0;
                        }
                        p_new_param = calloc(1, 64U);
                        if (p_new_param == 0U)
                        {
                            type_delete(p_func);
                            /*throw*/ goto _CKL0;
                        }
                        p_new_param->type = type_dup(&p->declarator->type);
                        param_list_add(&p_func->params, p_new_param);
                        p = p->next;
                    }
                }
                type_list_push_back(list, p_func);
            }
            else
            {
                if (pdirectdeclarator->array_declarator)
                {
                    struct type * p;

                    if (pdirectdeclarator->array_declarator->direct_declarator)
                    {
                        make_type_using_direct_declarator(ctx, pdirectdeclarator->array_declarator->direct_declarator, ppname, list);
                    }
                    p = calloc(1, 60U);
                    if (p == 0U)
                    {
                        /*throw*/ goto _CKL0;
                    }
                    p->category = 2;
                    p->num_of_elements = array_declarator_get_size(pdirectdeclarator->array_declarator);
                    p->array_num_elements_expression = pdirectdeclarator->array_declarator->assignment_expression;
                    if (pdirectdeclarator->array_declarator->static_token_opt)
                    {
                        p->has_static_array_size = 1;
                    }
                    if (pdirectdeclarator->array_declarator->type_qualifier_list_opt)
                    {
                        p->type_qualifier_flags = pdirectdeclarator->array_declarator->type_qualifier_list_opt->flags;
                    }
                    type_list_push_back(list, p);
                }
            }
        }
        if (pdirectdeclarator->name_opt)
        {
            *ppname = pdirectdeclarator->name_opt->lexeme;
        }
    }
    /*catch*/ else _CKL0:
    {
    }
}

void make_type_using_declarator_core(struct parser_ctx * ctx, struct declarator * pdeclarator, char ** ppname, struct type_list * list)
{
    /*try*/ if (1)
    {
        struct type_list  pointers;
        struct pointer * pointer;

        _cake_zmem(&pointers, 8);
        pointer = pdeclarator->pointer;
        while (pointer)
        {
            struct type * p_flat;

            p_flat = calloc(1, 60U);
            if (p_flat == 0U)
            {
                type_list_destroy(&pointers);
                /*throw*/ goto _CKL0;
            }
            if (pointer->type_qualifier_list_opt)
            {
                p_flat->type_qualifier_flags = pointer->type_qualifier_list_opt->flags;
            }
            if (pointer->attribute_specifier_sequence_opt)
            {
                p_flat->attributes_flags |= pointer->attribute_specifier_sequence_opt->attributes_flags;
            }
            p_flat->category = 3;
            if (pointer->calling_convention)
            {
                char * calling_convention_lexeme;

                calling_convention_lexeme = pointer->calling_convention->lexeme;
                if (strcmp(calling_convention_lexeme, "__fastcall") == 0)
                {
                    p_flat->attributes_flags |= 134217728;
                }
                else
                {
                    if (strcmp(calling_convention_lexeme, "__stdcall") == 0)
                    {
                        p_flat->attributes_flags |= 268435456;
                    }
                    else
                    {
                        if (strcmp(calling_convention_lexeme, "__cdecl") == 0)
                        {
                            p_flat->attributes_flags |= 536870912;
                        }
                        else
                        {
                            type_list_destroy(&pointers);
                            type_delete(p_flat);
                            /*throw*/ goto _CKL0;
                        }
                    }
                }
            }
            type_list_push_front(&pointers, p_flat);
            pointer = pointer->pointer;
        }
        if (pdeclarator->direct_declarator)
        {
            make_type_using_direct_declarator(ctx, pdeclarator->direct_declarator, ppname, list);
            if (list->head && list->head->category == 1)
            {
                if (pointers.head)
                {
                    pointers.head->storage_class_specifier_flags |= 8192;
                }
            }
        }
        while (pointers.head)
        {
            struct type * p;

            p = pointers.head;
            pointers.head = p->next;
            p->next = 0U;
            type_list_push_back(list, p);
        }
    }
    /*catch*/ else _CKL0:
    {
    }
}

struct enum_specifier *declarator_get_enum_specifier(struct declarator * pdeclarator)
{
    if (pdeclarator->declaration_specifiers && pdeclarator->declaration_specifiers->enum_specifier)
    {
        return pdeclarator->declaration_specifiers->enum_specifier;
    }
    if (pdeclarator->specifier_qualifier_list && pdeclarator->specifier_qualifier_list->enum_specifier)
    {
        return pdeclarator->specifier_qualifier_list->enum_specifier;
    }
    return 0U;
}

struct struct_or_union_specifier *declarator_get_struct_or_union_specifier(struct declarator * pdeclarator)
{
    if (pdeclarator->declaration_specifiers && pdeclarator->declaration_specifiers->struct_or_union_specifier)
    {
        return pdeclarator->declaration_specifiers->struct_or_union_specifier;
    }
    if (pdeclarator->specifier_qualifier_list && pdeclarator->specifier_qualifier_list->struct_or_union_specifier)
    {
        return pdeclarator->specifier_qualifier_list->struct_or_union_specifier;
    }
    return 0U;
}

struct typeof_specifier *declarator_get_typeof_specifier(struct declarator * pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        return pdeclarator->declaration_specifiers->typeof_specifier;
    }
    else
    {
        if (pdeclarator->specifier_qualifier_list)
        {
            return pdeclarator->specifier_qualifier_list->typeof_specifier;
        }
    }
    return 0U;
}

struct declarator *declarator_get_typedef_declarator(struct declarator * pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        return pdeclarator->declaration_specifiers->typedef_declarator;
    }
    else
    {
        if (pdeclarator->specifier_qualifier_list)
        {
            return pdeclarator->specifier_qualifier_list->typedef_declarator;
        }
    }
    return 0U;
}

static unsigned char  is_valid_type(struct parser_ctx * ctx, struct token * p_token, struct type * p_type)
{
    struct type * p;

    if (p_token == 0U)
    {
        p_token = ctx->current;
    }
    p = p_type;
    while (p)
    {
        if (p->category == 1)
        {
            if (p->next && p->next->category == 1)
            {
                compiler_diagnostic(1410, ctx, p_token, 0U, "function returning function");
                return 0;
            }
            else
            {
                if (p->next && p->next->category == 2)
                {
                    compiler_diagnostic(1420, ctx, p_token, 0U, "function returning array");
                    return 0;
                }
            }
        }
        else
        {
            if (p->category == 0 && p->type_specifier_flags == 0)
            {
                compiler_diagnostic(860, ctx, p_token, 0U, "invalid type");
                return 0;
            }
        }
        p = p->next;
    }
    return 1;
}

struct type make_type_using_declarator(struct parser_ctx * ctx, struct declarator * pdeclarator)
{
    struct type  empty;

    /*try*/ if (1)
    {
        struct type_list  list;
        char * name;
        struct type  r;

        _cake_zmem(&list, 8);
        name = 0U;
        make_type_using_declarator_core(ctx, pdeclarator, &name, &list);
        if (declarator_get_typeof_specifier(pdeclarator))
        {
            struct type * p_nt;
            struct type  nt;

            p_nt = calloc(1, 60U);
            if (p_nt == 0U)
            {
                type_list_destroy(&list);
                /*throw*/ goto _CKL0;
            }
            nt = type_dup(&declarator_get_typeof_specifier(pdeclarator)->type);
            *p_nt = nt;
            if (list.head != 0U)
            {
                type_set_qualifiers_using_declarator(list.head, pdeclarator);
            }
            if (list.tail)
            {
                ;
                list.tail->next = p_nt;
            }
            else
            {
                type_list_push_back(&list, p_nt);
            }
        }
        else
        {
            if (declarator_get_typedef_declarator(pdeclarator))
            {
                struct declarator * p_typedef_declarator;
                struct type  nt;
                struct type * p_nt;

                p_typedef_declarator = declarator_get_typedef_declarator(pdeclarator);
                if (p_typedef_declarator == 0U)
                {
                    type_list_destroy(&list);
                    /*throw*/ goto _CKL0;
                }
                nt = type_dup(&p_typedef_declarator->type);
                p_nt = calloc(1, 60U);
                if (p_nt == 0U)
                {
                    type_list_destroy(&list);
                    type_destroy(&nt);
                    /*throw*/ goto _CKL0;
                }
                *p_nt = nt;
                type_merge_qualifiers_using_declarator(p_nt, pdeclarator);
                if (list.tail)
                {
                    ;
                    list.tail->next = p_nt;
                }
                else
                {
                    type_list_push_back(&list, p_nt);
                }
            }
            else
            {
                struct type * p;

                p = calloc(1, 60U);
                if (p == 0U)
                {
                    type_list_destroy(&list);
                    /*throw*/ goto _CKL0;
                }
                p->category = 0;
                type_set_specifiers_using_declarator(p, pdeclarator);
                type_set_attributes_using_declarator(p, pdeclarator);
                type_set_qualifiers_using_declarator(p, pdeclarator);
                if (list.tail && list.tail->category == 1)
                {
                    p->storage_class_specifier_flags |= 8192;
                }
                type_list_push_back(&list, p);
                if (list.head)
                {
                    type_set_storage_specifiers_using_declarator(list.head, pdeclarator);
                }
            }
        }
        if (list.head == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        if (pdeclarator->name_opt)
        {
            char * temp;

            temp = strdup(pdeclarator->name_opt->lexeme);
            if (temp == 0U)
            {
                type_list_destroy(&list);
                /*throw*/ goto _CKL0;
            }
            free((void *)list.head->name_opt);
            list.head->name_opt = temp;
        }
        r = *list.head;
        free(list.head);
        type_set_storage_specifiers_using_declarator(&r, pdeclarator);
        if (!is_valid_type(ctx, pdeclarator->first_token_opt, &r))
        {
            struct type  empty;

            type_destroy(&r);
            _cake_zmem(&empty, 60);
            return empty;
        }
        return r;
    }
    /*catch*/ else _CKL0:
    {
    }
    _cake_zmem(&empty, 60);
    return empty;
}

void type_remove_names(struct type * p_type)
{
    struct type * p;

    p = p_type;
    while (p)
    {
        if (p->name_opt)
        {
            free((void *)p->name_opt);
            p->name_opt = 0U;
        }
        p = p->next;
    }
}

struct type *type_get_specifer_part(struct type * p_type)
{
    struct type * p;

    p = p_type;
    while (p->next)
    p = p->next;
    return p;
}

