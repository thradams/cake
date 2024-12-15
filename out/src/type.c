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
    struct token * first_token;
    struct token * last_token;
    struct token * else_token_opt;
    struct defer_list  defer_list;
};

struct array_declarator {
    struct direct_declarator * direct_declarator;
    struct expression * assignment_expression;
    struct expression * expression;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * token;
    struct token * static_token_opt;
};

struct pragma_declaration {
    struct token * first_token;
    struct token * last_token;
};

struct designation {
    struct designator_list * designator_list;
    struct token * token;
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

struct typeof_specifier {
    struct token * first_token;
    struct token * last_token;
    struct typeof_specifier_argument * typeof_specifier_argument;
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

struct try_statement {
    struct secondary_block * secondary_block;
    struct secondary_block * catch_secondary_block_opt;
    struct token * first_token;
    struct token * last_token;
    struct token * catch_token_opt;
    int try_catch_block_index;
};

struct labeled_statement {
    struct label * label;
    struct statement * statement;
};

struct attribute_token {
    int   attributes_flags;
    struct token * token;
};

struct member_declarator_list {
    struct member_declarator * head;
    struct member_declarator * tail;
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

struct enumerator {
    unsigned char   has_shared_ownership;
    struct token * token;
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct expression * constant_expression_opt;
    struct enum_specifier * enum_specifier;
    struct enumerator * next;
    struct object  value;
};

struct enumerator_list {
    struct enumerator * head;
    struct enumerator * tail;
};

struct storage_class_specifier {
    int   flags;
    struct token * token;
};

struct generic_assoc_list {
    struct generic_association * head;
    struct generic_association * tail;
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

struct attribute_specifier {
    struct token * first_token;
    struct token * last_token;
    struct attribute_list * attribute_list;
    struct attribute_specifier * next;
};

struct type_name {
    struct token * first_token;
    struct token * last_token;
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct declarator * abstract_declarator;
    struct type  type;
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

struct expression_statement {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct expression * expression_opt;
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

struct flow_objects_view {
    struct flow_object ** data;
    int size;
    int capacity;
};

struct block_item_list {
    struct block_item * head;
    struct block_item * tail;
};

struct diagnostic {
    unsigned long long errors;
    unsigned long long warnings;
    unsigned long long notes;
};

struct compound_statement {
    struct token * first_token;
    struct token * last_token;
    struct block_item_list  block_item_list;
    struct diagnostic  diagnostic_flags;
    struct defer_list  defer_list;
};

struct scope_list {
    struct scope * head;
    struct scope * tail;
};

struct defer_list_item {
    struct declarator * declarator;
    struct defer_statement * defer_statement;
    struct defer_list_item * next;
};

struct parameter_type_list {
    unsigned char   is_var_args;
    unsigned char   is_void;
    struct parameter_list * parameter_list;
};

struct parameter_list {
    struct parameter_declaration * head;
    struct parameter_declaration * tail;
};

struct argument_expression {
    struct expression * expression;
    struct argument_expression * next;
    unsigned char   set_unkown;
};

struct static_assert_declaration {
    struct token * first_token;
    struct token * last_token;
    struct expression * constant_expression;
    struct token * string_literal_opt;
};

struct function_declarator {
    struct direct_declarator * direct_declarator;
    struct scope  parameters_scope;
    struct parameter_type_list * parameter_type_list_opt;
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

struct token_list {
    struct token * head;
    struct token * tail;
};

struct designator_list {
    struct designator * head;
    struct designator * tail;
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

struct initializer_list_item {
    struct designation * designation;
    struct initializer * initializer;
    struct initializer_list_item * next;
};

struct primary_block {
    struct compound_statement * compound_statement;
    struct selection_statement * selection_statement;
    struct iteration_statement * iteration_statement;
    struct defer_statement * defer_statement;
    struct try_statement * try_statement;
};

struct condition {
    struct expression * expression;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator * p_init_declarator;
    struct token * first_token;
    struct token * last_token;
};

struct type_qualifier_list {
    int   flags;
    struct type_qualifier * head;
    struct type_qualifier * tail;
};

struct atomic_type_specifier {
    struct token * token;
    struct type_name * type_name;
};

struct unlabeled_statement {
    struct expression_statement * expression_statement;
    struct primary_block * primary_block;
    struct jump_statement * jump_statement;
};

struct designator {
    struct expression * constant_expression_opt;
    struct token * token;
    struct designator * next;
};

struct pointer {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct pointer * pointer;
};

struct osstream {
    char * c_str;
    int size;
    int capacity;
};

struct switch_value {
    long long value;
    struct label * p_label;
    struct switch_value * next;
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

struct parameter_declaration {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct declaration_specifiers * declaration_specifiers;
    struct declarator * declarator;
    struct parameter_declaration * next;
};

struct direct_declarator {
    struct token * name_opt;
    struct declarator * declarator;
    struct array_declarator * array_declarator;
    struct function_declarator * function_declarator;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
};

struct initializer {
    struct token * first_token;
    struct braced_initializer * braced_initializer;
    struct expression * assignment_expression;
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

struct switch_value_list {
    struct switch_value * head;
    struct switch_value * tail;
    struct switch_value * p_default;
};

struct __crt_locale_data;
struct param {
    struct type  type;
    struct param * next;
};

struct function_specifier {
    int   flags;
    struct token * token;
};

struct declaration_specifier {
    struct storage_class_specifier * storage_class_specifier;
    struct type_specifier_qualifier * type_specifier_qualifier;
    struct function_specifier * function_specifier;
    struct declaration_specifier * next;
};

struct alignment_specifier {
    struct type_name * type_name;
    struct expression * constant_expression;
    struct token * token;
};

struct attribute_list {
    int   attributes_flags;
    struct attribute * head;
    struct attribute * tail;
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
    struct token * p_identifier_opt;
    struct token * p_first_token;
};

struct attribute_argument_clause {
    struct balanced_token_sequence * p_balanced_token_sequence;
    struct token * token;
};

struct balanced_token {
    struct token * token;
    struct balanced_token * next;
};

struct init_statement {
    struct expression_statement * p_expression_statement;
    struct simple_declaration * p_simple_declaration;
};

struct generic_association {
    struct type  type;
    struct type_name * p_type_name;
    struct expression * expression;
    struct token * first_token;
    struct token * last_token;
    struct generic_association * next;
};

struct statement {
    struct labeled_statement * labeled_statement;
    struct unlabeled_statement * unlabeled_statement;
};

struct member_declaration {
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct member_declarator_list * member_declarator_list_opt;
    struct static_assert_declaration * static_assert_declaration;
    struct pragma_declaration * pragma_declaration;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct member_declaration * next;
};

struct flow_object_state {
    char * dbg_name;
    int state_number;
    struct flow_object * pointed;
    int   state;
    struct flow_objects_view  alternatives;
    struct flow_object_state * next;
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

struct jump_statement {
    struct token * label;
    struct token * first_token;
    struct token * last_token;
    struct expression * expression_opt;
    int try_catch_block_index;
    struct defer_list  defer_list;
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

struct __crt_multibyte_data;
struct diagnostic_stack {
    int top_index;
    struct diagnostic stack[10];
};

struct attribute {
    int   attributes_flags;
    struct attribute_token * attribute_token;
    struct attribute_argument_clause * attribute_argument_clause;
    struct attribute * next;
};

struct initializer_list {
    struct token * first_token;
    struct initializer_list_item * head;
    struct initializer_list_item * tail;
    int size;
};

struct init_declarator {
    unsigned char   has_shared_ownership;
    struct declarator * p_declarator;
    struct initializer * initializer;
    struct init_declarator * next;
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

struct braced_initializer {
    struct token * first_token;
    struct token * last_token;
    struct initializer_list * initializer_list;
};

struct type_list {
    struct type * head;
    struct type * tail;
};

struct block_item {
    struct token * first_token;
    struct declaration * declaration;
    struct unlabeled_statement * unlabeled_statement;
    struct label * label;
    struct block_item * next;
};

struct struct_entry;
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

struct defer_statement {
    struct token * first_token;
    struct token * last_token;
    struct secondary_block * secondary_block;
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

struct _iobuf {
    void * _Placeholder;
};

struct macro;
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


int ss_fprintf(struct osstream * stream, char * fmt, ...);

void print_item(struct osstream * ss, unsigned char  * first, char * item)
{
    if ( !( *first))
    {
        ss_fprintf(ss, " ");
    }
    ss_fprintf(ss, "%s", item);
     *first = 0;
}

unsigned char  print_type_specifier_flags(struct osstream * ss, unsigned char  * first, int   e_type_specifier_flags)
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
    return  *first;
}

void print_type_qualifier_flags(struct osstream * ss, unsigned char  * first, int   e_type_qualifier_flags)
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
    if (e_type_qualifier_flags & 32)
    {
        print_item(ss, first, "_Obj_owner");
    }
    if (e_type_qualifier_flags & 64)
    {
        print_item(ss, first, "_View");
    }
    if (e_type_qualifier_flags & 128)
    {
        print_item(ss, first, "_Opt");
    }
}

void print_type_qualifier_specifiers(struct osstream * ss, struct type * type)
{
    unsigned char   first = 1;
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
        a->type_specifier_flags = (8);
    }
}

void type_add_const(struct type * p_type)
{
    p_type->type_qualifier_flags = 1;
}

void type_remove_qualifiers(struct type * p_type)
{
    p_type->type_qualifier_flags = 0;
}

int  type_get_category(struct type * p_type);
struct type type_add_pointer(struct type * p_type, unsigned char   null_checks_enabled);
struct type get_array_item_type(struct type * p_type);
void type_destroy(struct type * p_type);
struct type type_dup(struct type * p_type);

struct type type_lvalue_conversion(struct type * p_type, unsigned char   nullchecks_enabled)
{
    int   category = type_get_category(p_type);
    switch (category)
    {
        case 1 :
        {
            struct type  t = type_add_pointer(p_type, nullchecks_enabled);
            t.type_qualifier_flags =  ~128;
            t.storage_class_specifier_flags =  ~2048;
            t.category = t.category;
            return t;
        }
        break;
        case 2 :
        {
            struct type  t = get_array_item_type(p_type);
            struct type  t2 = type_add_pointer(&t, nullchecks_enabled);
            type_remove_qualifiers(&t2);
            type_destroy(&t);
            t2.storage_class_specifier_flags =  ~2048;
            return t2;
        }
        break;
        case 3 :
        break;
        case 0 :
        default:
        break;
    }
    struct type  t = type_dup(p_type);
    type_remove_qualifiers(&t);
    t.storage_class_specifier_flags =  ~2048;
    t.category = type_get_category(&t);
    return t;
}

unsigned char  type_is_nullptr_t(struct type * p_type);
struct type make_void_ptr_type();
char *strdup(char * src);

struct type type_convert_to(struct type * p_type, int   target)
{
    if (target < 0 && type_is_nullptr_t(p_type))
    {
        struct type  t = make_void_ptr_type();
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
    struct type * p = p_type;
    while (p)
    {
        if (onlydeclarator && p->next == ((void *)0))
        {
            break;
        }
        switch (p->category)
        {
            case 0 :
            {
                struct osstream  local = {0};
                unsigned char   first = 1;
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
                struct osstream  local2 = {0};
                if (ss->c_str)
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
            break;
            case 2 :
            if (printname && p->name_opt)
            {
                ss_fprintf(ss, "%s", p->name_opt);
            }
            ss_fprintf(ss, "[");
            unsigned char   b = 1;
            if (p->static_array)
            {
                ss_fprintf(ss, "static");
                b = 0;
            }
            print_type_qualifier_flags(ss, &b, p->type_qualifier_flags);
            if (p->num_of_elements > 0)
            {
                if ( !b)
                {
                    ss_fprintf(ss, " ");
                }
                ss_fprintf(ss, "%d", p->num_of_elements);
            }
            ss_fprintf(ss, "]");
            break;
            case 1 :
            if (printname && p->name_opt)
            {
                ss_fprintf(ss, "%s", p->name_opt);
            }
            ss_fprintf(ss, "(");
            struct param * pa = p->params.head;
            while (pa)
            {
                struct osstream  sslocal = {0};
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
            break;
            case 3 :
            {
                struct osstream  local = {0};
                if (p->next && ((p->next->category == 1 || p->next->category == 2)))
                {
                    ss_fprintf(&local, "(");
                }
                ss_fprintf(&local, "*");
                unsigned char   first = 0;
                print_type_qualifier_flags(&local, &first, p->type_qualifier_flags);
                if (printname && p->name_opt)
                {
                    if ( !first)
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
            break;
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

void type_print(struct type * a)
{
    struct osstream  ss = {0};
    print_type(&ss, a);
    printf("%s", ss.c_str);
    ss_close(&ss);
}

int puts(char * _Buffer);

void type_println(struct type * a)
{
    type_print(a);
    puts("\n");
}

int  type_get_category(struct type * p_type)
{
    return p_type->category;
}

void param_list_add(struct param_list * list, struct param * p_item)
{
    if (list->head == ((void *)0))
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
    struct param * item = p->head;
    while (item)
    {
        struct param * next = item->next;
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
    free((void *)p_type->name_opt);
    param_list_destroy(&p_type->params);
    struct type * item = p_type->next;
    while (item)
    {
        struct type * next = item->next;
        item->next = ((void *)0);
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

unsigned char  type_has_attribute(struct type * p_type, int   attributes)
{
    if (p_type->attributes_flags & attributes)
    {
        return 1;
    }
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt = ((void *)0);
    if (p_type->struct_or_union_specifier)
    {
        p_attribute_specifier_sequence_opt = p_type->struct_or_union_specifier->attribute_specifier_sequence_opt;
        struct struct_or_union_specifier * p_complete = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
        if (p_attribute_specifier_sequence_opt == ((void *)0) && p_complete)
        {
            p_attribute_specifier_sequence_opt = p_complete->attribute_specifier_sequence_opt;
        }
    }
    else
    {
        if (p_type->enum_specifier)
        {
            struct enum_specifier * p_complete_enum_specifier = get_complete_enum_specifier(p_type->enum_specifier);
            p_attribute_specifier_sequence_opt = p_type->enum_specifier->attribute_specifier_sequence_opt;
            if (p_attribute_specifier_sequence_opt == ((void *)0) && p_complete_enum_specifier)
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
    return type_get_category(p_type) == 2;
}

unsigned char  type_is_owner(struct type * p_type);

unsigned char  type_is_any_owner(struct type * p_type)
{
    if (type_is_owner(p_type))
    {
        return 1;
    }
    return p_type->type_qualifier_flags & 32;
}

unsigned char  type_is_pointer_to_owner(struct type * p_type)
{
    if (p_type->next == ((void *)0))
    {
        return 0;
    }
    return type_is_owner(p_type->next);
}

unsigned char  type_is_obj_owner(struct type * p_type)
{
    return p_type->type_qualifier_flags & 32;
}

unsigned char  type_is_owner(struct type * p_type)
{
    if (p_type->struct_or_union_specifier)
    {
        if (p_type->type_qualifier_flags & 64)
        {
            return 0;
        }
        struct struct_or_union_specifier * p_complete = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
        if (p_complete && p_complete->is_owner)
        {
            return 1;
        }
    }
    return p_type->type_qualifier_flags & 16;
}

unsigned char  type_is_nullable(struct type * p_type, unsigned char   nullable_enabled)
{
    if (nullable_enabled)
    {
        return p_type->type_qualifier_flags & 128;
    }
    return 1;
}

unsigned char  type_is_view(struct type * p_type)
{
    return p_type->type_qualifier_flags & 64;
}

unsigned char  type_is_out(struct type * p_type)
{
    return p_type->type_qualifier_flags & 512;
}

unsigned char  type_is_const(struct type * p_type)
{
    return p_type->type_qualifier_flags & 1;
}

unsigned char  type_is_pointer_to_const(struct type * p_type)
{
    if (p_type->category == 3)
    {
        if (p_type->next)
        {
            return p_type->next->type_qualifier_flags & 1;
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
            return p_type->next->type_specifier_flags & 1;
        }
    }
    return 0;
}

unsigned char  type_is_void(struct type * p_type)
{
    if (p_type->category == 0)
    {
        return p_type->type_specifier_flags & 1;
    }
    return 0;
}

unsigned char  type_is_nullptr_t(struct type * p_type)
{
    if (p_type->category == 0)
    {
        return p_type->type_specifier_flags & 16777216;
    }
    return 0;
}

unsigned char  type_is_pointer_to_out(struct type * p_type)
{
    if (p_type->next == ((void *)0))
    {
        return 0;
    }
    if (p_type->category == 3)
    {
        return p_type->next->type_qualifier_flags & 512;
    }
    return 0;
}

unsigned char  type_is_pointer(struct type * p_type)
{
    return p_type->category == 3;
}

unsigned char  type_is_essential_bool(struct type * p_type)
{
    return p_type->attributes_flags & 33554432;
}

unsigned char  type_is_essential_char(struct type * p_type)
{
    return p_type->attributes_flags & 67108864;
}

unsigned char  type_is_enum(struct type * p_type)
{
    return type_get_category(p_type) == 0 && p_type->type_specifier_flags & 65536;
}

unsigned char  type_is_struct_or_union(struct type * p_type)
{
    return type_get_category(p_type) == 0 && p_type->type_specifier_flags & 32768;
}

unsigned char  type_is_union(struct type * p_type)
{
    if (type_get_category(p_type) != 0)
    {
        return 0;
    }
    if (p_type->struct_or_union_specifier == ((void *)0))
    {
        return 0;
    }
    return p_type->struct_or_union_specifier->first_token->type == 9038;
}

unsigned char  type_is_character(struct type * p_type)
{
    return type_get_category(p_type) == 0 && p_type->type_specifier_flags & 2;
}

unsigned char  object_has_constant_value(struct object * a);

unsigned char  type_is_vla(struct type * p_type)
{
    struct type * it = p_type;
    while (it && type_is_array(it))
    {
        if (it->array_num_elements_expression)
        {
            if ( !object_has_constant_value(&it->array_num_elements_expression->object))
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
    return type_get_category(p_type) == 0 && p_type->type_specifier_flags & 8192;
}

unsigned char  type_is_decimal64(struct type * p_type)
{
    return type_get_category(p_type) == 0 && p_type->type_specifier_flags & 4096;
}

unsigned char  type_is_decimal32(struct type * p_type)
{
    return type_get_category(p_type) == 0 && p_type->type_specifier_flags & 2048;
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
        if ( !(p_type->type_specifier_flags & 16))
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
    if ((p_type->type_specifier_flags == (8 | 128)) || (p_type->type_specifier_flags == 8))
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
    if (p_type->type_specifier_flags == (8 | 256))
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
    return type_get_category(p_type) == 0 && p_type->type_specifier_flags & 512;
}

unsigned char  type_is_floating_point(struct type * p_type)
{
    if (type_get_category(p_type) != 0)
    {
        return 0;
    }
    return p_type->type_specifier_flags & (64 | 32);
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
    if (type_is_integer(p_type) &&  !(p_type->type_specifier_flags & 256))
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
    return p_type->next->type_specifier_flags & 2;
}

unsigned char  type_is_char(struct type * p_type)
{
    if (type_get_category(p_type) != 0)
    {
        return 0;
    }
    return p_type->type_specifier_flags & 2;
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
    return p_type->type_specifier_flags & (2 | 4 | 8 | 524288 | 1048576 | 2097152 | 8 | 16 | 128 | 256 | 262144 | 524288 | 2097152 | 4194304 | 512);
}

unsigned char  type_is_arithmetic(struct type * p_type)
{
    return type_is_integer(p_type) || type_is_floating_point(p_type);
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
    return ((void *)0);
}

unsigned char  compiler_diagnostic_message(int   w, struct parser_ctx * ctx, struct token * p_token, struct marker * p_marker, char * fmt, ...);
unsigned char  expression_is_null_pointer_constant(struct expression * expression);
struct type type_remove_pointer(struct type * p_type);

void check_ownership_qualifiers_of_argument_and_parameter(struct parser_ctx * ctx, struct argument_expression * current_argument, struct type * paramer_type, int param_num)
{
    unsigned char   paramer_is_obj_owner = type_is_obj_owner(paramer_type);
    unsigned char   paramer_is_owner = type_is_owner(paramer_type);
    unsigned char   paramer_is_view =  !paramer_is_obj_owner &&  !paramer_is_owner;
    struct type * argument_type = &current_argument->expression->type;
    unsigned char   argument_is_owner = type_is_owner(&current_argument->expression->type);
    unsigned char   argument_is_obj_owner = type_is_obj_owner(&current_argument->expression->type);
    unsigned char   argument_is_view =  !argument_is_owner &&  !argument_is_obj_owner;
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
                    compiler_diagnostic_message(22, ctx, current_argument->expression->first_token, ((void *)0), "passing a temporary owner to a view");
                }
            }
            else
            {
                if (argument_is_obj_owner && paramer_is_owner)
                {
                    compiler_diagnostic_message(23, ctx, current_argument->expression->first_token, ((void *)0), "cannot move _Obj_owner to _Owner");
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
                                compiler_diagnostic_message(22, ctx, current_argument->expression->first_token, ((void *)0), "passing a temporary owner to a view");
                            }
                        }
                        else
                        {
                            if (argument_is_view && paramer_is_owner)
                            {
                                if ( !expression_is_null_pointer_constant(current_argument->expression))
                                {
                                    compiler_diagnostic_message(23, ctx, current_argument->expression->first_token, ((void *)0), "passing a _View argument to a _Owner parameter");
                                }
                            }
                            else
                            {
                                if (argument_is_view && paramer_is_obj_owner)
                                {
                                    if (type_is_pointer(argument_type))
                                    {
                                        struct type  t2 = type_remove_pointer(argument_type);
                                        if ( !type_is_owner(&t2))
                                        {
                                            compiler_diagnostic_message(23, ctx, current_argument->expression->first_token, ((void *)0), "pointed object is not _Owner");
                                        }
                                        else
                                        {
                                            if ( !argument_type->address_of)
                                            {
                                                compiler_diagnostic_message(18, ctx, current_argument->expression->first_token, ((void *)0), "_Obj_owner pointer must be created using address of operator &");
                                            }
                                        }
                                        type_destroy(&t2);
                                    }
                                    else
                                    {
                                        if ( !expression_is_null_pointer_constant(current_argument->expression))
                                        {
                                            compiler_diagnostic_message(23, ctx, current_argument->expression->first_token, ((void *)0), "passing a _View argument to a _Obj_owner parameter");
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
    if (type_is_any_owner(paramer_type))
    {
        if (type_is_obj_owner(paramer_type))
        {
            if (current_argument->expression->type.category == 3)
            {
                if (type_is_pointer(&current_argument->expression->type) &&  !type_is_pointer_to_owner(&current_argument->expression->type))
                {
                    compiler_diagnostic_message(21, ctx, current_argument->expression->first_token, ((void *)0), "parameter %d requires a pointer to _Owner object", param_num);
                }
            }
            else
            {
                compiler_diagnostic_message(21, ctx, current_argument->expression->first_token, ((void *)0), "parameter %d requires a pointer to _Owner type", param_num);
            }
        }
    }
    struct type * argument_type = &current_argument->expression->type;
    unsigned char   is_null_pointer_constant = expression_is_null_pointer_constant(current_argument->expression);
    struct type  parameter_type_converted = (type_is_array(paramer_type)) ? type_lvalue_conversion(paramer_type, ctx->options.null_checks_enabled) : type_dup(paramer_type);
    struct type  argument_type_converted = expression_is_subjected_to_lvalue_conversion(current_argument->expression) ? type_lvalue_conversion(argument_type, ctx->options.null_checks_enabled) : type_dup(argument_type);
    if (type_is_enum(argument_type) && type_is_enum(paramer_type))
    {
        if ( !type_is_same(argument_type, paramer_type, 0))
        {
            compiler_diagnostic_message(950, ctx, current_argument->expression->first_token, ((void *)0), " incompatible types at argument %d", param_num);
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
        compiler_diagnostic_message(27, ctx, current_argument->expression->first_token, ((void *)0), " passing null as array");
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
            int parameter_array_size = paramer_type->num_of_elements;
            if (type_is_array(argument_type))
            {
                int argument_array_size = argument_type->num_of_elements;
                if (parameter_array_size != 0 && argument_array_size < parameter_array_size)
                {
                    compiler_diagnostic_message(1130, ctx, current_argument->expression->first_token, ((void *)0), " argument of size [%d] is smaller than parameter of size [%d]", argument_array_size, parameter_array_size);
                }
            }
            else
            {
                if (is_null_pointer_constant || type_is_nullptr_t(argument_type))
                {
                    compiler_diagnostic_message(38, ctx, current_argument->expression->first_token, ((void *)0), " passing null as array");
                }
            }
        }
        if ( !type_is_same(&argument_type_converted, &parameter_type_converted, 0))
        {
            type_print(&argument_type_converted);
            type_print(&parameter_type_converted);
            compiler_diagnostic_message(950, ctx, current_argument->expression->first_token, ((void *)0), " incompatible types at argument %d", param_num);
        }
        if (type_is_pointer(&argument_type_converted) && type_is_pointer(&parameter_type_converted))
        {
            struct type  argument_pointer_to = type_remove_pointer(&argument_type_converted);
            struct type  parameter_pointer_to = type_remove_pointer(&parameter_type_converted);
            if (type_is_const(&argument_pointer_to) &&  !type_is_const(&parameter_pointer_to) &&  !type_is_any_owner(&parameter_pointer_to))
            {
                compiler_diagnostic_message(14, ctx, current_argument->expression->first_token, ((void *)0), " discarding const at argument %d", param_num);
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
    return type_get_category(p_type) == 1;
}

unsigned char  type_is_function_or_function_pointer(struct type * p_type)
{
    if (type_is_function(p_type))
    {
        return 1;
    }
    if (type_is_pointer(p_type))
    {
        struct type  t = type_remove_pointer(p_type);
        unsigned char   r = type_is_function(&t);
        type_destroy(&t);
        return r;
    }
    return 0;
}

unsigned char  type_is_empty(struct type * p_type)
{
    return p_type->type_specifier_flags == 0;
}

void *calloc(int nmemb, unsigned int size);

struct type type_add_pointer(struct type * p_type, unsigned char   null_checks_enabled)
{
    struct type  r = type_dup(p_type);
    if (1)
    {
        struct type * p = calloc(1, sizeof (struct type));
        if (p == ((void *)0))
        {
            goto _catch_label_1;
        }
         *p = r;
        struct type  __cmp_lt_0 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        r = __cmp_lt_0;
        r.next = p;
        r.category = 3;
        r.storage_class_specifier_flags = p_type->storage_class_specifier_flags;
    }
    else _catch_label_1:
    {
    }
    return r;
}

struct type type_remove_pointer(struct type * p_type)
{
    struct type  r = type_dup(p_type);
    if ( !type_is_pointer(p_type))
    {
        return r;
    }
    if (r.next)
    {
        struct type  next =  *r.next;
        free(r.next);
        r.next = ((void *)0);
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
    struct type  r = type_dup(p_type);
    if (r.next)
    {
        struct type  r2 =  *r.next;
        free(r.next);
        free((void *)r.name_opt);
        param_list_destroy(&r.params);
        return r2;
    }
    return r;
}

struct type type_param_array_to_pointer(struct type * p_type, unsigned char   null_checks_enabled)
{
    ;
    struct type  t = get_array_item_type(p_type);
    struct type  t2 = type_add_pointer(&t, null_checks_enabled);
    if (p_type->type_qualifier_flags & 1)
    {
        t2.type_qualifier_flags = 1;
    }
    type_destroy(&t);
    t2.storage_class_specifier_flags =  ~2048;
    return t2;
}

unsigned char  type_is_pointer_or_array(struct type * p_type)
{
    int   category = type_get_category(p_type);
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
    struct type  r = type_make_int();
    return r;
}

void type_swap(struct type * a, struct type * b);
unsigned int type_get_sizeof(struct type * p_type);

struct type type_common(struct type * p_type1, struct type * p_type2)
{
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
    struct type  promoted_a = {0};
    struct type  promoted_b = {0};
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
    struct type * p_signed_promoted = type_is_signed_integer(&promoted_a) ? &promoted_a : &promoted_b;
    struct type * p_unsigned_promoted = type_is_unsigned_integer(&promoted_a) ? &promoted_a : &promoted_b;
    ;
    if (type_get_integer_rank(p_unsigned_promoted) >= type_get_integer_rank(p_signed_promoted))
    {
        struct type  r = {0};
        type_swap(&r, p_unsigned_promoted);
        type_destroy(&promoted_a);
        type_destroy(&promoted_b);
        return r;
    }
    if (type_get_sizeof(p_signed_promoted) > type_get_sizeof(p_unsigned_promoted))
    {
        struct type  r = {0};
        type_swap(&r, p_signed_promoted);
        type_destroy(&promoted_a);
        type_destroy(&promoted_b);
        return r;
    }
    struct type  r = {0};
    type_swap(&r, p_signed_promoted);
    r.type_specifier_flags = 256;
    type_destroy(&promoted_a);
    type_destroy(&promoted_b);
    return r;
}

void type_set(struct type * a, struct type * b)
{
    struct type  t = type_dup(b);
    type_swap(&t, a);
    type_destroy(&t);
}

void type_list_destroy(struct type_list * p_type_list);
void type_list_push_back(struct type_list * books, struct type * new_book);

struct type type_dup(struct type * p_type)
{
    if (1)
    {
        struct type_list  l = {0};
        struct type * p = p_type;
        while (p)
        {
            struct type * p_new = calloc(1, sizeof (struct type));
            if (p_new == ((void *)0))
            {
                type_list_destroy(&l);
                goto _catch_label_1;
            }
             *p_new =  *p;
            p_new->next = ((void *)0);
            if (p->name_opt)
            {
                p_new->name_opt = strdup(p->name_opt);
            }
            if (p->category == 1)
            {
                p_new->params.head = ((void *)0);
                p_new->params.tail = ((void *)0);
                struct param * p_param = p->params.head;
                while (p_param)
                {
                    struct param * p_new_param = calloc(1, sizeof  *p_new_param);
                    if (p_new_param == ((void *)0))
                    {
                        type_list_destroy(&l);
                        type_delete(p_new);
                        goto _catch_label_1;
                    }
                    p_new_param->type = type_dup(&p_param->type);
                    param_list_add(&p_new->params, p_new_param);
                    p_param = p_param->next;
                }
            }
            type_list_push_back(&l, p_new);
            p = p->next;
        }
        if (l.head == ((void *)0))
        {
            goto _catch_label_1;
        }
        struct type  r =  *l.head;
        free(l.head);
        return r;
    }
    else _catch_label_1:
    {
    }
    struct type  empty = {0};
    return empty;
}

unsigned int type_get_alignof(struct type * p_type);

int get_sizeof_struct(struct struct_or_union_specifier * complete_struct_or_union_specifier)
{
    unsigned char   is_union = (complete_struct_or_union_specifier->first_token->type == 9038);
    int maxalign = 0;
    int size = 0;
    struct member_declaration * d = complete_struct_or_union_specifier->member_declaration_list.head;
    while (d)
    {
        if (d->member_declarator_list_opt)
        {
            struct member_declarator * md = d->member_declarator_list_opt->head;
            while (md)
            {
                int align = 1;
                if (md->declarator)
                {
                    align = type_get_alignof(&md->declarator->type);
                    if (align > maxalign)
                    {
                        maxalign = align;
                    }
                    if (size % align != 0)
                    {
                        size = align - (size % align);
                    }
                    int item_size = type_get_sizeof(&md->declarator->type);
                    if (is_union)
                    {
                        if (item_size > size)
                        {
                            size = item_size;
                        }
                    }
                    else
                    {
                        size = item_size;
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
                if (d->specifier_qualifier_list->struct_or_union_specifier)
                {
                    struct type  t = {0};
                    t.category = 0;
                    t.struct_or_union_specifier = d->specifier_qualifier_list->struct_or_union_specifier;
                    t.type_specifier_flags = 32768;
                    int align = type_get_alignof(&t);
                    if (align > maxalign)
                    {
                        maxalign = align;
                    }
                    if (size % align != 0)
                    {
                        size = align - (size % align);
                    }
                    int item_size = type_get_sizeof(&t);
                    if (is_union)
                    {
                        if (item_size > size)
                        {
                            size = item_size;
                        }
                    }
                    else
                    {
                        size = item_size;
                    }
                    type_destroy(&t);
                }
                else
                {
                    ;
                }
            }
        }
        d = d->next;
    }
    if (maxalign != 0)
    {
        if (size % maxalign != 0)
        {
            size = maxalign - (size % maxalign);
        }
    }
    else
    {
        ;
    }
    return size;
}

unsigned int get_alignof_struct(struct struct_or_union_specifier * complete_struct_or_union_specifier)
{
    int align = 0;
    struct member_declaration * d = complete_struct_or_union_specifier->member_declaration_list.head;
    while (d)
    {
        if (d->member_declarator_list_opt)
        {
            struct member_declarator * md = d->member_declarator_list_opt->head;
            while (md)
            {
                if (md->declarator)
                {
                    int temp_align = type_get_alignof(&md->declarator->type);
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
                struct type  type = {0};
                type.type_specifier_flags = d->specifier_qualifier_list->type_specifier_flags;
                type.enum_specifier = d->specifier_qualifier_list->enum_specifier;
                type.struct_or_union_specifier = d->specifier_qualifier_list->struct_or_union_specifier;
                int temp_align = type_get_alignof(&type);
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
    int align = 0;
    int   category = type_get_category(p_type);
    if (category == 3)
    {
        align = 4U;
    }
    else
    {
        if (category == 1)
        {
            align =  -1;
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
                                                                            struct struct_or_union_specifier * p_complete = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
                                                                            align = 1;
                                                                            if (p_complete)
                                                                            {
                                                                                align = get_alignof_struct(p_complete);
                                                                            }
                                                                            else
                                                                            {
                                                                                align =  -2;
                                                                            }
                                                                        }
                                                                        else
                                                                        {
                                                                            align =  -2;
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
                                                                                align =  -3;
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
                    struct type  type = get_array_item_type(p_type);
                    align = type_get_alignof(&type);
                    type_destroy(&type);
                }
            }
        }
    }
    ;
    return align;
}

unsigned int type_get_sizeof(struct type * p_type)
{
    int   category = type_get_category(p_type);
    if (category == 3)
    {
        return (int)sizeof (void *);
    }
    if (category == 1)
    {
        return (unsigned int) -1;
    }
    if (category == 2)
    {
        if (p_type->storage_class_specifier_flags & 2048)
        {
            return sizeof (void *);
        }
        else
        {
            if (type_is_vla(p_type))
            {
                return (unsigned int) -3;
            }
            int arraysize = p_type->num_of_elements;
            struct type  type = get_array_item_type(p_type);
            int sz = type_get_sizeof(&type);
            int size = sz * arraysize;
            type_destroy(&type);
            return size;
        }
    }
    ;
    if (p_type->type_specifier_flags & 2)
    {
        return (int)sizeof (char);
    }
    if (p_type->type_specifier_flags & 512)
    {
        return (int)sizeof (unsigned char );
    }
    if (p_type->type_specifier_flags & 4)
    {
        return (int)sizeof (short);
    }
    if (p_type->type_specifier_flags & 65536)
    {
        return (int)sizeof (int);
    }
    if (p_type->type_specifier_flags & 16)
    {
        return (int)sizeof (long);
    }
    if (p_type->type_specifier_flags & 4194304)
    {
        return (int)sizeof (long long);
    }
    if (p_type->type_specifier_flags & 8)
    {
        return (int)sizeof (int);
    }
    if (p_type->type_specifier_flags & 2097152)
    {
        return (int)sizeof (long long);
    }
    if (p_type->type_specifier_flags & 1048576)
    {
        return 4;
    }
    if (p_type->type_specifier_flags & 524288)
    {
        return 2;
    }
    if (p_type->type_specifier_flags & 262144)
    {
        return 1;
    }
    if (p_type->type_specifier_flags & 32)
    {
        return (int)sizeof (float);
    }
    if (p_type->type_specifier_flags & 64)
    {
        return (int)sizeof (double);
    }
    if (p_type->type_specifier_flags & 32768)
    {
        if (p_type->struct_or_union_specifier == ((void *)0))
        {
            return (unsigned int) -2;
        }
        struct struct_or_union_specifier * p_complete = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
        if (p_complete == ((void *)0))
        {
            return (unsigned int) -2;
        }
        return get_sizeof_struct(p_complete);
    }
    if (p_type->type_specifier_flags & 65536)
    {
        return (int)sizeof (int);
    }
    if (p_type->type_specifier_flags == 0)
    {
        return (unsigned int) -3;
    }
    if (p_type->type_specifier_flags == 1)
    {
        return 1;
    }
    if (p_type->type_specifier_flags == 16777216)
    {
        return sizeof (void *);
    }
    if (p_type->type_specifier_flags == 2048)
    {
        return 4;
    }
    if (p_type->type_specifier_flags == 4096)
    {
        return 8;
    }
    if (p_type->type_specifier_flags == 8192)
    {
        return 16;
    }
    ;
    return (unsigned int) -1;
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

struct type get_function_return_type(struct type * p_type)
{
    if (1)
    {
        if (p_type->next == ((void *)0))
        {
            goto _catch_label_1;
        }
        if (type_is_pointer(p_type))
        {
            if (p_type->next->next == ((void *)0))
            {
                goto _catch_label_1;
            }
            struct type  r = type_dup(p_type->next->next);
            return r;
        }
        struct type  r = type_dup(p_type->next);
        return r;
    }
    else _catch_label_1:
    {
    }
    struct type  empty = {0};
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
    struct type  t = {0};
    t.type_specifier_flags = 65536;
    t.enum_specifier = enum_specifier;
    t.category = 0;
    return t;
}

struct type type_get_enum_type(struct type * p_type)
{
    if (1)
    {
        if (p_type->enum_specifier == ((void *)0))
        {
            goto _catch_label_1;
        }
        struct enum_specifier * p_complete_enum_specifier = get_complete_enum_specifier(p_type->enum_specifier);
        if (p_complete_enum_specifier && p_complete_enum_specifier->specifier_qualifier_list)
        {
            struct type  t = {0};
            t.type_qualifier_flags = p_complete_enum_specifier->specifier_qualifier_list->type_qualifier_flags;
            t.type_specifier_flags = p_complete_enum_specifier->specifier_qualifier_list->type_specifier_flags;
            return t;
        }
        struct type  t = {0};
        t.type_specifier_flags = 8;
        return t;
    }
    else _catch_label_1:
    {
    }
    struct type  empty = {0};
    return empty;
}

struct type type_make_long_double()
{
    struct type  t = {0};
    t.type_specifier_flags = 16 | 64;
    t.category = 0;
    return t;
}

struct type type_make_double()
{
    struct type  t = {0};
    t.type_specifier_flags = 64;
    t.category = 0;
    return t;
}

struct type type_make_float()
{
    struct type  t = {0};
    t.type_specifier_flags = 32;
    t.category = 0;
    return t;
}

struct type type_make_size_t()
{
    struct type  t = {0};
    t.type_specifier_flags = 256 | 8;
    t.category = 0;
    return t;
}

struct type make_void_ptr_type()
{
    struct type  t = {0};
    if (1)
    {
        struct type * p = calloc(1, sizeof  *p);
        if (p == ((void *)0))
        {
            goto _catch_label_1;
        }
        t.category = 3;
        p->category = 0;
        p->type_specifier_flags = 1;
        t.next = p;
    }
    else _catch_label_1:
    {
    }
    return t;
}

struct type make_void_type()
{
    struct type  t = {0};
    t.type_specifier_flags = 1;
    t.category = 0;
    return t;
}

struct type type_make_int_bool_like()
{
    struct type  t = {0};
    t.type_specifier_flags = 8;
    t.attributes_flags = 33554432;
    t.category = 0;
    return t;
}

struct type make_size_t_type()
{
    struct type  t = {0};
    t.type_specifier_flags = (256 | 8);
    t.category = 0;
    return t;
}

struct type type_make_int()
{
    struct type  t = {0};
    t.type_specifier_flags = 8;
    t.category = 0;
    return t;
}

struct type type_make_literal_string(int size_in_bytes, int   chartype)
{
    struct type  t = {0};
    if (1)
    {
        struct type * p2 = calloc(1, sizeof (struct type));
        if (p2 == ((void *)0))
        {
            goto _catch_label_1;
        }
        struct type  char_type = {0};
        char_type.category = 0;
        char_type.type_specifier_flags = chartype;
        int char_size = type_get_sizeof(&char_type);
        if (char_size == 0)
        {
            char_size = 1;
        }
        type_destroy(&char_type);
        t.category = 2;
        t.num_of_elements = size_in_bytes / char_size;
        p2->category = 0;
        p2->type_specifier_flags = chartype;
        t.next = p2;
    }
    else _catch_label_1:
    {
    }
    return t;
}

int strcmp(char * _Str1, char * _Str2);

unsigned char  struct_or_union_specifier_is_same(struct struct_or_union_specifier * a, struct struct_or_union_specifier * b)
{
    if (a && b)
    {
        struct struct_or_union_specifier * p_complete_a = get_complete_struct_or_union_specifier(a);
        struct struct_or_union_specifier * p_complete_b = get_complete_struct_or_union_specifier(b);
        if (p_complete_a != ((void *)0) && p_complete_b != ((void *)0))
        {
            if (p_complete_a != p_complete_b)
            {
                return 0;
            }
            return 1;
        }
        else
        {
            if (a->tagtoken != ((void *)0) && b->tagtoken != ((void *)0))
            {
                if (strcmp(a->tagtoken->lexeme, b->tagtoken->lexeme) == 0)
                {
                    return 1;
                }
            }
        }
        return p_complete_a == ((void *)0) && p_complete_b == ((void *)0);
    }
    return a == ((void *)0) && b == ((void *)0);
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
        return get_complete_enum_specifier(a) == ((void *)0) && get_complete_enum_specifier(b) == ((void *)0);
    }
    return a == ((void *)0) && b == ((void *)0);
}

unsigned char  type_is_same(struct type * a, struct type * b, unsigned char   compare_qualifiers)
{
    struct type * pa = a;
    struct type * pb = b;
    while (pa && pb)
    {
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
        if (pa->enum_specifier &&  !pb->enum_specifier)
        {
        }
        if ( !pa->enum_specifier && pb->enum_specifier)
        {
        }
        if (pa->static_array != pb->static_array)
        {
            return 0;
        }
        if (pa->category == 1)
        {
            if (pa->params.is_var_args != pb->params.is_var_args)
            {
                return 0;
            }
            if (pa->params.is_void != pb->params.is_void)
            {
                return 0;
            }
            struct param * p_param_a = pa->params.head;
            struct param * p_param_b = pb->params.head;
            while (p_param_a && p_param_b)
            {
                if ( !type_is_same(&p_param_a->type, &p_param_b->type, 1))
                {
                    return 0;
                }
                p_param_a = p_param_a->next;
                p_param_b = p_param_b->next;
            }
            return p_param_a == ((void *)0) && p_param_b == ((void *)0);
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
        if (compare_qualifiers && pa->type_qualifier_flags != pb->type_qualifier_flags)
        {
            return 0;
        }
        if (pa->type_specifier_flags != pb->type_specifier_flags)
        {
            return 0;
        }
        pa = pa->next;
        pb = pb->next;
    }
    return pa == ((void *)0) && pb == ((void *)0);
}

void type_clear(struct type * a)
{
    struct type  tmp = {0};
    type_swap(&tmp, a);
    type_destroy(&tmp);
}

void type_swap(struct type * a, struct type * b)
{
    struct type  temp =  *a;
     *a =  *b;
     *b = temp;
}

void type_visit_to_mark_anonymous(struct type * p_type)
{
    if (p_type->struct_or_union_specifier != ((void *)0) && p_type->struct_or_union_specifier->has_anonymous_tag)
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
    int   type_qualifier_flags = 0;
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

void type_set_qualifiers_using_declarator(struct type * p_type, struct declarator * pdeclarator)
{
    int   type_qualifier_flags = 0;
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
        p_type->storage_class_specifier_flags = pdeclarator->declaration_specifiers->storage_class_specifier_flags;
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
            p_type->storage_class_specifier_flags = 16384;
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
    if (books->head == ((void *)0))
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
    struct type * item = p_type_list->head;
    while (item)
    {
        struct type * next = item->next;
        item->next = ((void *)0);
        type_destroy_one(item);
        free(item);
        item = next;
    }
}

void type_list_push_back(struct type_list * type_list, struct type * new_book)
{
    if (type_list->tail == ((void *)0))
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
unsigned long long array_declarator_get_size(struct array_declarator * p_array_declarator);

void make_type_using_direct_declarator(struct parser_ctx * ctx, struct direct_declarator * pdirectdeclarator, char ** ppname, struct type_list * list)
{
    if (1)
    {
        if (pdirectdeclarator->declarator)
        {
            make_type_using_declarator_core(ctx, pdirectdeclarator->declarator, ppname, list);
        }
        else
        {
            if (pdirectdeclarator->function_declarator)
            {
                if (pdirectdeclarator->function_declarator->direct_declarator)
                {
                    make_type_using_direct_declarator(ctx, pdirectdeclarator->function_declarator->direct_declarator, ppname, list);
                }
                struct type * p_func = calloc(1, sizeof (struct type));
                if (p_func == ((void *)0))
                {
                    goto _catch_label_1;
                }
                p_func->category = 1;
                if (pdirectdeclarator->function_declarator->parameter_type_list_opt && pdirectdeclarator->function_declarator->parameter_type_list_opt->parameter_list)
                {
                    struct parameter_declaration * p = pdirectdeclarator->function_declarator->parameter_type_list_opt->parameter_list->head;
                    p_func->params.is_var_args = pdirectdeclarator->function_declarator->parameter_type_list_opt->is_var_args;
                    p_func->params.is_void = pdirectdeclarator->function_declarator->parameter_type_list_opt->is_void;
                    while (p)
                    {
                        if (p->declarator == ((void *)0))
                        {
                            type_delete(p_func);
                            goto _catch_label_1;
                        }
                        struct param * p_new_param = calloc(1, sizeof (struct param));
                        if (p_new_param == ((void *)0))
                        {
                            type_delete(p_func);
                            goto _catch_label_1;
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
                    if (pdirectdeclarator->array_declarator->direct_declarator)
                    {
                        make_type_using_direct_declarator(ctx, pdirectdeclarator->array_declarator->direct_declarator, ppname, list);
                    }
                    struct type * p = calloc(1, sizeof (struct type));
                    if (p == ((void *)0))
                    {
                        goto _catch_label_1;
                    }
                    p->category = 2;
                    p->num_of_elements = (int)array_declarator_get_size(pdirectdeclarator->array_declarator);
                    p->array_num_elements_expression = pdirectdeclarator->array_declarator->assignment_expression;
                    if (pdirectdeclarator->array_declarator->static_token_opt)
                    {
                        p->static_array = 1;
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
    else _catch_label_1:
    {
    }
}

void make_type_using_declarator_core(struct parser_ctx * ctx, struct declarator * pdeclarator, char ** ppname, struct type_list * list)
{
    if (1)
    {
        struct type_list  pointers = {0};
        struct pointer * pointer = pdeclarator->pointer;
        while (pointer)
        {
            struct type * p_flat = calloc(1, sizeof (struct type));
            if (p_flat == ((void *)0))
            {
                type_list_destroy(&pointers);
                goto _catch_label_1;
            }
            if (pointer->type_qualifier_list_opt)
            {
                p_flat->type_qualifier_flags = pointer->type_qualifier_list_opt->flags;
            }
            if (pointer->attribute_specifier_sequence_opt)
            {
                p_flat->attributes_flags = pointer->attribute_specifier_sequence_opt->attributes_flags;
            }
            p_flat->category = 3;
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
                    pointers.head->storage_class_specifier_flags = 8192;
                }
            }
        }
        while (pointers.head)
        {
            struct type * p = pointers.head;
            pointers.head = p->next;
            p->next = ((void *)0);
            type_list_push_back(list, p);
        }
    }
    else _catch_label_1:
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
    return ((void *)0);
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
    return ((void *)0);
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
    return ((void *)0);
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
    return ((void *)0);
}

struct type make_type_using_declarator(struct parser_ctx * ctx, struct declarator * pdeclarator)
{
    if (1)
    {
        struct type_list  list = {0};
        char * name = ((void *)0);
        make_type_using_declarator_core(ctx, pdeclarator, &name, &list);
        if (declarator_get_typeof_specifier(pdeclarator))
        {
            struct type * p_nt = calloc(1, sizeof (struct type));
            if (p_nt == ((void *)0))
            {
                type_list_destroy(&list);
                goto _catch_label_1;
            }
            struct type  nt = type_dup(&declarator_get_typeof_specifier(pdeclarator)->type);
             *p_nt = nt;
            if (list.head != ((void *)0))
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
                struct declarator * p_typedef_declarator = declarator_get_typedef_declarator(pdeclarator);
                if (p_typedef_declarator == ((void *)0))
                {
                    type_list_destroy(&list);
                    goto _catch_label_1;
                }
                struct type  nt = type_dup(&p_typedef_declarator->type);
                struct type * p_nt = calloc(1, sizeof (struct type));
                if (p_nt == ((void *)0))
                {
                    type_list_destroy(&list);
                    type_destroy(&nt);
                    goto _catch_label_1;
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
                struct type * p = calloc(1, sizeof (struct type));
                if (p == ((void *)0))
                {
                    type_list_destroy(&list);
                    goto _catch_label_1;
                }
                p->category = 0;
                type_set_specifiers_using_declarator(p, pdeclarator);
                type_set_attributes_using_declarator(p, pdeclarator);
                type_set_qualifiers_using_declarator(p, pdeclarator);
                if (list.tail && list.tail->category == 1)
                {
                    p->storage_class_specifier_flags = 8192;
                }
                type_list_push_back(&list, p);
                if (list.head)
                {
                    type_set_storage_specifiers_using_declarator(list.head, pdeclarator);
                }
            }
        }
        if (list.head == ((void *)0))
        {
            goto _catch_label_1;
        }
        if (pdeclarator->name_opt)
        {
            char * temp = strdup(pdeclarator->name_opt->lexeme);
            if (temp == ((void *)0))
            {
                type_list_destroy(&list);
                goto _catch_label_1;
            }
            free((void *)list.head->name_opt);
            list.head->name_opt = temp;
        }
        struct type  r =  *list.head;
        free(list.head);
        type_set_storage_specifiers_using_declarator(&r, pdeclarator);
        return r;
    }
    else _catch_label_1:
    {
    }
    struct type  empty = {0};
    return empty;
}

void type_remove_names(struct type * p_type)
{
    struct type * p = p_type;
    while (p)
    {
        if (p->name_opt)
        {
            free((void *)p->name_opt);
            p->name_opt = ((void *)0);
        }
        p = p->next;
    }
}

struct type *type_get_specifer_part(struct type * p_type)
{
    struct type * p = p_type;
    while (p->next)
    p = p->next;
    return p;
}

