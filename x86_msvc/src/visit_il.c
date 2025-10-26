/* Cake 0.12.26 x86_msvc */
struct token;

struct token_list {
    struct token * head;
    struct token * tail;
};

struct expression_statement;
struct simple_declaration;

struct init_statement {
    struct expression_statement * p_expression_statement;
    struct simple_declaration * p_simple_declaration;
};

struct storage_class_specifier {
    int flags;
    struct token * token;
};

struct type_name;

struct atomic_type_specifier {
    struct token * token;
    struct type_name * type_name;
};

struct expression;
struct attribute_specifier_sequence;
struct declaration_specifiers;
struct init_declarator;

struct condition {
    struct expression * expression;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator * p_init_declarator;
    struct token * first_token;
    struct token * last_token;
};

struct generic_association;

struct generic_assoc_list {
    struct generic_association * head;
    struct generic_association * tail;
};

struct param;

struct param_list {
    unsigned char  is_var_args;
    unsigned char  is_void;
    struct param * head;
    struct param * tail;
};

struct struct_or_union_specifier;
struct enum_specifier;

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
    unsigned char  has_static_array_size;
    unsigned char  address_of;
    struct param_list  params;
    struct type * next;
};

struct specifier_qualifier_list;
struct declarator;

struct type_name {
    struct token * first_token;
    struct token * last_token;
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct declarator * abstract_declarator;
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

struct secondary_block;

struct try_statement {
    struct secondary_block * secondary_block;
    struct secondary_block * catch_secondary_block_opt;
    struct token * first_token;
    struct token * last_token;
    struct token * catch_token_opt;
    int catch_label_id;
};

struct defer_statement;

struct defer_list_item {
    struct declarator * declarator;
    struct defer_statement * defer_statement;
    struct defer_list_item * next;
};

struct statement;

struct secondary_block {
    struct token * first_token;
    struct token * last_token;
    struct statement * statement;
};

struct osstream {
    char * c_str;
    int size;
    int capacity;
};

struct flow_object;

struct pragma_declaration {
    struct token * first_token;
    struct token * last_token;
};

struct platform {
    char * name;
    char * thread_local_attr;
    char * alignas_fmt_must_have_one_percent_d;
    int bool_n_bits;
    int bool_aligment;
    int bool_type;
    int char_n_bits;
    int char_t_type;
    int char_aligment;
    int short_n_bits;
    int short_aligment;
    int int_n_bits;
    int int_aligment;
    int long_n_bits;
    int long_aligment;
    int long_long_n_bits;
    int long_long_aligment;
    int float_n_bits;
    int float_aligment;
    int double_n_bits;
    int double_aligment;
    int long_double_n_bits;
    int long_double_aligment;
    int pointer_n_bits;
    int pointer_aligment;
    int wchar_t_type;
    int int8_type;
    int int16_type;
    int int32_type;
    int int64_type;
    int size_t_type;
    int ptrdiff_type;
};

struct map_entry;

struct hash_map {
    struct map_entry ** table;
    int capacity;
    int size;
};

struct diagnostic {
    unsigned long long errors;
    unsigned long long warnings;
    unsigned long long notes;
};

struct enumerator;
struct macro;
struct struct_entry;

struct hash_item_set {
    unsigned int number;
    struct enum_specifier * p_enum_specifier;
    struct enumerator * p_enumerator;
    struct struct_or_union_specifier * p_struct_or_union_specifier;
    struct declarator * p_declarator;
    struct init_declarator * p_init_declarator;
    struct macro * p_macro;
    struct struct_entry * p_struct_entry;
};

struct diagnostic_stack {
    int top_index;
    struct diagnostic stack[10];
};

struct parameter_list;

struct parameter_type_list {
    unsigned char  is_var_args;
    unsigned char  is_void;
    struct parameter_list * parameter_list;
};

struct type_qualifier_list;

struct pointer {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * calling_convention;
    struct pointer * pointer;
};

struct generic_selection {
    struct expression * expression;
    struct type_name * type_name;
    struct expression * p_view_selected_expression;
    struct generic_assoc_list  generic_assoc_list;
    struct token * first_token;
    struct token * last_token;
};

struct enumerator_list {
    struct enumerator * head;
    struct enumerator * tail;
};

struct enum_specifier {
    unsigned char  has_shared_ownership;
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct specifier_qualifier_list * specifier_qualifier_list;
    char tag_name[200];
    struct enumerator_list  enumerator_list;
    struct token * tag_token;
    struct token * first_token;
    struct enum_specifier * p_complete_enum_specifier;
};

struct type_qualifier {
    int flags;
    struct token * token;
    struct type_qualifier * next;
};

struct designator {
    struct expression * constant_expression_opt;
    struct token * token;
    struct designator * next;
};

struct parameter_declaration;

struct parameter_list {
    struct parameter_declaration * head;
    struct parameter_declaration * tail;
};

struct label;

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

struct initializer;

struct init_declarator {
    unsigned char  has_shared_ownership;
    struct declarator * p_declarator;
    struct initializer * initializer;
    struct init_declarator * next;
};

struct type_specifier_qualifier;
struct function_specifier;
struct alignment_specifier;

struct declaration_specifier {
    struct storage_class_specifier * storage_class_specifier;
    struct type_specifier_qualifier * type_specifier_qualifier;
    struct function_specifier * function_specifier;
    struct alignment_specifier * alignment_specifier;
    struct declaration_specifier * next;
};

struct balanced_token_sequence;

struct attribute_argument_clause {
    struct balanced_token_sequence * p_balanced_token_sequence;
    struct token * token;
};

struct struct_entry_list {
    struct struct_entry ** data;
    int size;
    int capacity;
};

struct options {
    int input;
    int target;
    struct diagnostic_stack  diagnostic_stack;
    int style;
    unsigned char  show_includes;
    unsigned char  disable_assert;
    unsigned char  flow_analysis;
    unsigned char  test_mode;
    unsigned char  null_checks_enabled;
    unsigned char  ownership_enabled;
    unsigned char  preprocess_only;
    unsigned char  preprocess_def_macro;
    unsigned char  clear_error_at_end;
    unsigned char  sarif_output;
    unsigned char  no_output;
    unsigned char  const_literal;
    unsigned char  visual_studio_ouput_format;
    unsigned char  disable_colors;
    unsigned char  dump_tokens;
    unsigned char  dump_pptokens;
    unsigned char  auto_config;
    unsigned char  do_static_debug;
    int static_debug_lines;
    char output[200];
    char sarifpath[200];
};

union __tag17 {
    signed long long host_long_long;
    unsigned long long host_u_long_long;
    float host_float;
    double host_double;
    long double long_double_val;
};

struct object;

struct object_list {
    struct object * head;
    struct object * tail;
    unsigned int count;
};

struct object {
    int state;
    int value_type;
    struct type  type;
    char * member_designator;
    union __tag17  value;
    struct object * parent;
    struct object * p_ref;
    struct expression * p_init_expression;
    struct object_list  members;
    struct object * next;
};

struct attribute_list;

struct attribute_specifier {
    int ack;
    struct token * first_token;
    struct token * last_token;
    struct attribute_list * attribute_list;
    struct attribute_specifier * next;
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

struct member_declarator_list;
struct static_assert_declaration;

struct member_declaration {
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct member_declarator_list * member_declarator_list_opt;
    struct static_assert_declaration * static_assert_declaration;
    struct pragma_declaration * pragma_declaration;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
    struct member_declaration * next;
};

struct attribute {
    int msvc_declspec_flags;
    int attributes_flags;
    struct attribute_argument_clause * attribute_argument_clause;
    struct token * attribute_token;
    struct attribute * next;
};

struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct argument_expression;

struct argument_expression_list {
    struct argument_expression * head;
    struct argument_expression * tail;
};

struct typeof_specifier;

struct type_specifier {
    int flags;
    struct token * token;
    struct struct_or_union_specifier * struct_or_union_specifier;
    struct typeof_specifier * typeof_specifier;
    struct enum_specifier * enum_specifier;
    struct declarator * typedef_declarator;
    struct atomic_type_specifier * atomic_type_specifier;
};

struct param {
    struct type  type;
    struct param * next;
};

struct member_declaration_list {
    struct token * first_token;
    struct token * last_token;
    struct member_declaration * head;
    struct member_declaration * tail;
};

struct labeled_statement {
    struct label * label;
    struct statement * statement;
};

struct braced_initializer;

struct initializer {
    struct token * first_token;
    struct braced_initializer * braced_initializer;
    struct expression * assignment_expression;
};

struct unlabeled_statement;

struct statement {
    struct labeled_statement * labeled_statement;
    struct unlabeled_statement * unlabeled_statement;
};

struct struct_entry {
    unsigned char  definition_was_printed;
    unsigned char  declaration_was_printed;
    struct struct_or_union_specifier * p_struct_or_union_specifier;
    struct struct_entry_list  hard_dependencies;
    struct struct_entry_list  soft_dependencies;
    struct struct_entry * next;
};

struct attribute_list {
    int attributes_flags;
    struct attribute * head;
    struct attribute * tail;
};

struct scope {
    int scope_level;
    struct hash_map  tags;
    struct hash_map  variables;
    struct scope * next;
    struct scope * previous;
};

struct macro;

struct static_assert_declaration {
    struct token * first_token;
    struct token * last_token;
    struct expression * constant_expression;
    struct token * string_literal_opt;
};

struct array_declarator;
struct function_declarator;

struct direct_declarator {
    struct token * name_opt;
    struct token * p_calling_convention;
    struct declarator * declarator;
    struct array_declarator * array_declarator;
    struct function_declarator * function_declarator;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
};

struct function_declarator {
    struct direct_declarator * direct_declarator;
    struct scope  parameters_scope;
    struct parameter_type_list * parameter_type_list_opt;
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

struct init_declarator_list {
    struct init_declarator * head;
    struct init_declarator * tail;
};

struct compound_statement;

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

struct block_item;

struct block_item_list {
    struct block_item * head;
    struct block_item * tail;
};

struct iteration_statement;

struct primary_block {
    struct compound_statement * compound_statement;
    struct selection_statement * selection_statement;
    struct iteration_statement * iteration_statement;
    struct defer_statement * defer_statement;
    struct try_statement * try_statement;
};

struct balanced_token {
    struct token * token;
    struct balanced_token * next;
};

struct balanced_token_sequence {
    struct balanced_token * head;
    struct balanced_token * tail;
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

struct break_reference {
    struct selection_statement * p_selection_statement;
    struct iteration_statement * p_iteration_statement;
};

struct typeof_specifier_argument;

struct typeof_specifier {
    struct token * first_token;
    struct token * last_token;
    struct typeof_specifier_argument * typeof_specifier_argument;
    struct type  type;
};

struct typeof_specifier_argument {
    struct expression * expression;
    struct type_name * type_name;
};

struct function_specifier {
    int flags;
    struct token * token;
};

struct alignment_specifier {
    int flags;
    struct type_name * type_name;
    struct expression * constant_expression;
    struct token * token;
};

struct initializer_list;

struct braced_initializer {
    struct token * first_token;
    struct token * last_token;
    struct initializer_list * initializer_list;
};

struct argument_expression {
    struct expression * expression;
    struct argument_expression * next;
    unsigned char  set_unkown;
};

struct simple_declaration {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
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

struct attribute_specifier_sequence {
    struct token * first_token;
    struct token * last_token;
    int msvc_declspec_flags;
    int attributes_flags;
    struct attribute_specifier * head;
    struct attribute_specifier * tail;
};

struct declaration_list {
    struct declaration * head;
    struct declaration * tail;
};

struct ast {
    struct token_list  token_list;
    struct declaration_list  declaration_list;
};

struct designator_list;

struct designation {
    struct designator_list * designator_list;
    struct token * token;
};

struct type_qualifier_list {
    int flags;
    struct type_qualifier * head;
    struct type_qualifier * tail;
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
    unsigned char  is_assignment_expression;
};

struct type_specifier_qualifier {
    struct type_specifier * type_specifier;
    struct type_qualifier * type_qualifier;
    struct alignment_specifier * alignment_specifier;
    struct type_specifier_qualifier * next;
};

struct struct_or_union_specifier {
    unsigned char  has_shared_ownership;
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct member_declaration_list  member_declaration_list;
    struct token * first_token;
    struct token * last_token;
    unsigned char  is_owner;
    struct token * tagtoken;
    unsigned int unique_id;
    char tag_name[200];
    unsigned char  has_anonymous_tag;
    unsigned char  show_anonymous_tag;
    int scope_level;
    int visit_moved;
    struct struct_or_union_specifier * complete_struct_or_union_specifier_indirection;
};

struct defer_statement {
    struct token * first_token;
    struct token * last_token;
    struct secondary_block * secondary_block;
};

struct jump_statement;

struct unlabeled_statement {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
    struct expression_statement * expression_statement;
    struct primary_block * primary_block;
    struct jump_statement * jump_statement;
};

struct expression_statement {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
    struct expression * expression_opt;
};

struct jump_statement {
    struct token * label;
    struct token * first_token;
    struct token * last_token;
    struct expression * expression_opt;
    int label_id;
    struct defer_list  defer_list;
};

union __tag5 {
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
    union __tag5  data;
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

struct enumerator {
    unsigned char  has_shared_ownership;
    struct token * token;
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct expression * constant_expression_opt;
    struct enum_specifier * enum_specifier;
    struct enumerator * next;
    struct object  value;
};

struct array_declarator {
    struct direct_declarator * direct_declarator;
    struct expression * assignment_expression;
    struct expression * expression;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * token;
    struct token * static_token_opt;
};

struct member_declarator;

struct member_declarator_list {
    struct member_declarator * head;
    struct member_declarator * tail;
};

struct member_declarator {
    struct declarator * declarator;
    struct expression * constant_expression;
    struct member_declarator * next;
};

struct compound_statement {
    struct token * first_token;
    struct token * last_token;
    struct block_item_list  block_item_list;
    struct diagnostic  diagnostic_flags;
    struct defer_list  defer_list;
};

struct initializer_list_item;

struct initializer_list {
    struct token * first_token;
    struct initializer_list_item * head;
    struct initializer_list_item * tail;
    int size;
};

struct initializer_list_item {
    struct designation * designation;
    struct initializer * initializer;
    struct initializer_list_item * next;
};

struct d_visit_ctx {
    struct options  options;
    int indentation;
    unsigned char  print_qualifiers;
    unsigned int cake_file_scope_declarator_number;
    unsigned int cake_local_declarator_number;
    struct hash_map  tag_names;
    struct hash_map  structs_map;
    struct hash_map  file_scope_declarator_map;
    struct hash_map  instantiated_function_literals;
    struct osstream  block_scope_declarators;
    struct osstream  add_this_before;
    struct osstream  add_this_before_external_decl;
    struct osstream  add_this_after_external_decl;
    unsigned char  is_local;
    unsigned char  zero_mem_used;
    unsigned char  memcpy_used;
    unsigned char  define_nullptr;
    unsigned char  null_pointer_constant_used;
    unsigned char  address_of_argument;
    struct declarator * p_current_function_opt;
    struct break_reference  break_reference;
    unsigned char  is__func__predefined_identifier_added;
    struct ast  ast;
};

struct parameter_declaration {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct declaration_specifiers * declaration_specifiers;
    struct declarator * declarator;
    struct parameter_declaration * next;
};

struct designator_list {
    struct designator * head;
    struct designator * tail;
};

struct declarator {
    unsigned char  has_shared_ownership;
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
    unsigned char  declarator_renamed;
};


static void _cake_zmem(void *dest, unsigned int len)
{
  unsigned char *ptr;

  ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

__declspec(selectany) int _Avx2WmemEnabledWeakValue = 0;
void hashmap_destroy(struct hash_map * map);
void ss_close(struct osstream * stream);

void d_visit_ctx_destroy(struct d_visit_ctx * ctx)
{
    hashmap_destroy(&ctx->tag_names);
    hashmap_destroy(&ctx->structs_map);
    hashmap_destroy(&ctx->file_scope_declarator_map);
    hashmap_destroy(&ctx->instantiated_function_literals);
    ss_close(&ctx->block_scope_declarators);
    ss_close(&ctx->add_this_before);
    ss_close(&ctx->add_this_before_external_decl);
    ss_close(&ctx->add_this_after_external_decl);
}


void free(void * ptr);

void struct_entry_delete(struct struct_entry * p)
{
    if (p)
    {
        free(p->hard_dependencies.data);
        free(p->soft_dependencies.data);
        free(p);
    }
}


void *realloc(void * ptr, unsigned int size);

int struct_entry_list_reserve(struct struct_entry_list * p, int n)
{
    if (n > p->capacity)
    {
        void * pnew;

        if ((unsigned int)n > 1073741823)
        {
            return 132;
        }
        pnew = realloc(p->data, n * 4);
        if (pnew == 0)
        {
            return 12;
        }
        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}


int struct_entry_list_push_back(struct struct_entry_list * p, struct struct_entry * book)
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
        error = struct_entry_list_reserve(p, new_capacity);
        if (error != 0)
        {
            return error;
        }
    }
    p->data[p->size] = book;
    p->size++;
    return 0;
}


unsigned char struct_or_union_specifier_is_union(struct struct_or_union_specifier * p);
int ss_fprintf(struct osstream * stream, char * fmt, ...);
static void d_print_struct(struct d_visit_ctx * ctx, struct osstream * ss, struct struct_or_union_specifier * p_struct_or_union_specifier);
static struct struct_or_union_specifier *get_complete_struct_or_union_specifier2(struct struct_or_union_specifier * p_struct_or_union_specifier);
struct struct_or_union_specifier *get_complete_struct_or_union_specifier(struct struct_or_union_specifier * p_struct_or_union_specifier);
unsigned char type_is_array(struct type * p_type);
static void d_print_type(struct d_visit_ctx * ctx, struct osstream * ss, struct type * p_type, char * name_opt, unsigned char  print_storage_qualifier);
static void register_struct_types_and_functions(struct d_visit_ctx * ctx, struct type * p_type0, struct struct_entry * p_struct_entry0);
static int __cdecl snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...);
static int __cdecl vsnprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, char * _ArgList);
int __cdecl __stdio_common_vsprintf(unsigned long long _Options, char * _Buffer, unsigned int _BufferCount, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct map_entry *hashmap_find(struct hash_map * map, char * key);
int hashmap_set(struct hash_map * map, char * key, struct hash_item_set * item);
void hash_item_set_destroy(struct hash_item_set * p);
void *calloc(unsigned int nmemb, unsigned int size);
unsigned char type_is_struct_or_union(struct type * p_type);
struct type get_array_item_type(struct type * p_type);
void type_destroy(struct type * p_type);
static void d_print_type_core(struct d_visit_ctx * ctx, struct osstream * ss, struct type * p_type0, char * name_opt);
static void d_print_type_qualifier_flags(struct osstream * ss, unsigned char * first, int e_type_qualifier_flags);
void print_item(struct osstream * ss, unsigned char * first, char * item);
int get_enum_type_specifier_flags(struct enum_specifier * p_enum_specifier);
unsigned char print_type_specifier_flags(struct osstream * ss, unsigned char * first, int e_type_specifier_flags);
int object_type_to_type_specifier(int type);
struct platform *get_platform(int target);
unsigned char print_type_alignment_flags(struct osstream * ss, unsigned char * first, int flags, int target);
void print_msvc_declspec(struct osstream * ss, unsigned char * first, int msvc_declspec_flags);
void ss_swap(struct osstream * a, struct osstream * b);

void d_print_structs(struct d_visit_ctx * ctx, struct osstream * ss, struct struct_entry * p_struct_entry)
{
    unsigned char  some_declartions_printed;

    if (p_struct_entry->definition_was_printed)
    {
        return;
    }
    {
        int i;

        i = 0;
        for (; i < p_struct_entry->hard_dependencies.size; i++)
        {
            struct struct_entry * p_struct_entry_item;

            p_struct_entry_item = p_struct_entry->hard_dependencies.data[i];
            d_print_structs(ctx, ss, p_struct_entry_item);
        }
    }
    some_declartions_printed = 0;
    {
        int i;

        i = 0;
        for (; i < p_struct_entry->soft_dependencies.size; i++)
        {
            struct struct_entry * p_struct_entry_item;

            p_struct_entry_item = p_struct_entry->soft_dependencies.data[i];
            if (!p_struct_entry_item->definition_was_printed && !p_struct_entry_item->declaration_was_printed)
            {
                some_declartions_printed = 1;
                p_struct_entry_item->declaration_was_printed = 1;
                if (struct_or_union_specifier_is_union(p_struct_entry_item->p_struct_or_union_specifier))
                {
                    ss_fprintf(ss, "union %s;\n", p_struct_entry_item->p_struct_or_union_specifier->tag_name);
                }
                else
                {
                    ss_fprintf(ss, "struct %s;\n", p_struct_entry_item->p_struct_or_union_specifier->tag_name);
                }
            }
        }
    }
    if (some_declartions_printed)
    {
        ss_fprintf(ss, "\n");
    }
    if (!p_struct_entry->definition_was_printed)
    {
        struct osstream  local;

        p_struct_entry->definition_was_printed = 1;
        _cake_zmem(&local, 12);
        d_print_struct(ctx, &local, p_struct_entry->p_struct_or_union_specifier);
        if (local.c_str)
        {
            ss_fprintf(ss, local.c_str);
        }
        ss_close(&local);
    }
}



static struct struct_or_union_specifier *get_complete_struct_or_union_specifier2(struct struct_or_union_specifier * p_struct_or_union_specifier)
{
    struct struct_or_union_specifier * p_complete;

    p_complete = get_complete_struct_or_union_specifier(p_struct_or_union_specifier);
    if (p_complete == 0)
    {
        p_complete = p_struct_or_union_specifier->complete_struct_or_union_specifier_indirection;
    }
    return p_complete;
}

static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &__c0__OptionsStorage;
}

static int __cdecl vsnprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, char * _ArgList)
{
    int _Result;

    _Result = __stdio_common_vsprintf((*__local_stdio_printf_options()) | 2ULL, _Buffer, _BufferCount, _Format, 0, _ArgList);
    return _Result < 0 ? -1 : _Result;
}

static int __cdecl snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4));
    _Result = vsnprintf(_Buffer, _BufferCount, _Format, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}

static void register_struct_types_and_functions(struct d_visit_ctx * ctx, struct type * p_type0, struct struct_entry * p_struct_entry0)
{
    if (1) /*try*/
    {
        struct type * p_type;

        p_type = p_type0;
        while (p_type)
        {
            /*switch*/
            {
                int __v0 = p_type->category;
                if (__v0 == 0) goto __L2; /*case 0*/
                if (__v0 == 1) goto __L27; /*case 1*/
                if (__v0 == 2) goto __L28; /*case 2*/
                if (__v0 == 3) goto __L29; /*case 3*/
                goto __L1;

                {
                    __L2: /*case 0*/ 
                    {
                        if (p_type->struct_or_union_specifier)
                        {
                            struct struct_or_union_specifier * p_complete;

                            p_complete = get_complete_struct_or_union_specifier2(p_type->struct_or_union_specifier);
                            if (p_complete == 0)
                            {
                                p_complete = p_type->struct_or_union_specifier;
                            }
                            if (p_complete)
                            {
                                char unique_id[100];
                                struct map_entry * p;

                                _cake_zmem(&unique_id, 100);
                                snprintf(unique_id, 100, "%d", p_complete->unique_id);
                                p = hashmap_find(&ctx->structs_map, unique_id);
                                if (p == 0)
                                {
                                    struct map_entry * p_name;
                                    struct struct_entry * p_struct_entry;
                                    struct member_declaration * member_declaration;

                                    p_name = hashmap_find(&ctx->tag_names, p_complete->tag_name);
                                    if (p_name != 0)
                                    {
                                        char new_name[100];
                                        struct hash_item_set  i;

                                        _cake_zmem(&new_name, 100);
                                        snprintf(new_name, 100, "__c%s%d", p_complete->tag_name, p_complete->unique_id);
                                        snprintf(p_complete->tag_name, 200, "%s", new_name);
                                        _cake_zmem(&i, 32);
                                        i.number = 1;
                                        hashmap_set(&ctx->tag_names, new_name, &i);
                                        hash_item_set_destroy(&i);
                                    }
                                    else
                                    {
                                        struct hash_item_set  i;

                                        _cake_zmem(&i, 32);
                                        i.number = 1;
                                        hashmap_set(&ctx->tag_names, p_complete->tag_name, &i);
                                        hash_item_set_destroy(&i);
                                    }
                                    p_struct_entry = calloc(1, 36);
                                    if (p_struct_entry == 0)
                                    {
                                        goto __L0; /* throw */
                                    }
                                    p_struct_entry->p_struct_or_union_specifier = p_complete;
                                    {
                                        struct hash_item_set  i;

                                        _cake_zmem(&i, 32);
                                        i.p_struct_entry = p_struct_entry;
                                        hashmap_set(&ctx->structs_map, unique_id, &i);
                                        hash_item_set_destroy(&i);
                                    }
                                    if (p_struct_entry0 && p_struct_entry0 != p_struct_entry)
                                    {
                                        struct_entry_list_push_back(&p_struct_entry0->soft_dependencies, p_struct_entry);
                                    }
                                    member_declaration = p_complete->member_declaration_list.head;
                                    while (member_declaration)
                                    {
                                        struct member_declarator * member_declarator;

                                        member_declarator = 0;
                                        if (member_declaration->member_declarator_list_opt)
                                        {
                                            member_declarator = member_declaration->member_declarator_list_opt->head;
                                            while (member_declarator)
                                            {
                                                if (member_declarator->declarator)
                                                {
                                                    if (type_is_struct_or_union(&member_declarator->declarator->type))
                                                    {
                                                        struct struct_or_union_specifier * p_complete_member;
                                                        char name2[100];
                                                        struct map_entry * p2;

                                                        ;
                                                        p_complete_member = get_complete_struct_or_union_specifier(member_declarator->declarator->type.struct_or_union_specifier);
                                                        if (p_complete_member == 0)
                                                        {
                                                            goto __L0; /* throw */
                                                        }
                                                        _cake_zmem(&name2, 100);
                                                        snprintf(name2, 100, "%d", p_complete_member->unique_id);
                                                        register_struct_types_and_functions(ctx, &member_declarator->declarator->type, 0);
                                                        p2 = hashmap_find(&ctx->structs_map, name2);
                                                        if (p2 != 0)
                                                        {
                                                            ;
                                                            struct_entry_list_push_back(&p_struct_entry->hard_dependencies, p2->data.p_struct_entry);
                                                        }
                                                    }
                                                    if (type_is_array(&member_declarator->declarator->type))
                                                    {
                                                        struct type  t;

                                                        t = get_array_item_type(&member_declarator->declarator->type);
                                                        if (type_is_struct_or_union(&t))
                                                        {
                                                            struct struct_or_union_specifier * p_complete_member;
                                                            char name2[100];
                                                            struct map_entry * p2;

                                                            ;
                                                            p_complete_member = p_complete_member = get_complete_struct_or_union_specifier(t.struct_or_union_specifier);
                                                            _cake_zmem(&name2, 100);
                                                            snprintf(name2, 100, "%d", p_complete_member->unique_id);
                                                            register_struct_types_and_functions(ctx, &t, 0);
                                                            p2 = hashmap_find(&ctx->structs_map, name2);
                                                            if (p2 != 0)
                                                            {
                                                                ;
                                                                struct_entry_list_push_back(&p_struct_entry->hard_dependencies, p2->data.p_struct_entry);
                                                            }
                                                        }
                                                        else
                                                        {
                                                            register_struct_types_and_functions(ctx, &member_declarator->declarator->type, p_struct_entry);
                                                        }
                                                        type_destroy(&t);
                                                    }
                                                    else
                                                    {
                                                        register_struct_types_and_functions(ctx, &member_declarator->declarator->type, p_struct_entry);
                                                    }
                                                }
                                                member_declarator = member_declarator->next;
                                            }
                                        }
                                        else
                                        {
                                            if (member_declaration->specifier_qualifier_list != 0)
                                            {
                                                if (member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                                                {
                                                    struct type  t;

                                                    _cake_zmem(&t, 68);
                                                    t.category = 0;
                                                    t.struct_or_union_specifier = member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                                                    t.type_specifier_flags = 32768;
                                                    if (type_is_struct_or_union(&t))
                                                    {
                                                        struct struct_or_union_specifier * p_complete_member;
                                                        char name2[100];
                                                        struct map_entry * p2;

                                                        p_complete_member = p_complete_member = get_complete_struct_or_union_specifier(t.struct_or_union_specifier);
                                                        _cake_zmem(&name2, 100);
                                                        snprintf(name2, 100, "%d", p_complete_member->unique_id);
                                                        register_struct_types_and_functions(ctx, &t, p_struct_entry);
                                                        p2 = hashmap_find(&ctx->structs_map, name2);
                                                        if (p2 != 0)
                                                        {
                                                            ;
                                                            struct_entry_list_push_back(&p_struct_entry->hard_dependencies, p2->data.p_struct_entry);
                                                        }
                                                    }
                                                    if (type_is_array(&t))
                                                    {
                                                        struct type  t2;

                                                        t2 = get_array_item_type(&t);
                                                        if (type_is_struct_or_union(&t2))
                                                        {
                                                            struct struct_or_union_specifier * p_complete_member;
                                                            char name2[100];
                                                            struct map_entry * p2;

                                                            p_complete_member = p_complete_member = get_complete_struct_or_union_specifier(t.struct_or_union_specifier);
                                                            if (p_complete_member == 0)
                                                            {
                                                                type_destroy(&t2);
                                                                goto __L0; /* throw */
                                                            }
                                                            _cake_zmem(&name2, 100);
                                                            snprintf(name2, 100, "%d", p_complete_member->unique_id);
                                                            register_struct_types_and_functions(ctx, &t, p_struct_entry);
                                                            p2 = hashmap_find(&ctx->structs_map, name2);
                                                            if (p2 != 0)
                                                            {
                                                                ;
                                                                struct_entry_list_push_back(&p_struct_entry->hard_dependencies, p2->data.p_struct_entry);
                                                            }
                                                        }
                                                        else
                                                        {
                                                            register_struct_types_and_functions(ctx, &t, p_struct_entry);
                                                        }
                                                        type_destroy(&t2);
                                                    }
                                                    else
                                                    {
                                                        register_struct_types_and_functions(ctx, &t, p_struct_entry);
                                                    }
                                                    type_destroy(&t);
                                                }
                                            }
                                        }
                                        member_declaration = member_declaration->next;
                                    }
                                }
                                else
                                {
                                    if (p_struct_entry0 && p_struct_entry0 != p->data.p_struct_entry)
                                    {
                                        struct_entry_list_push_back(&p_struct_entry0->soft_dependencies, p->data.p_struct_entry);
                                    }
                                }
                            }
                        }
                        else
                        {
                        }
                    }
                    goto __L1; /* break */

                    __L27: /*case 1*/ 
                    {
                        struct param * pa;

                        pa = p_type->params.head;
                        while (pa)
                        {
                            register_struct_types_and_functions(ctx, &pa->type, p_struct_entry0);
                            pa = pa->next;
                        }
                    }
                    goto __L1; /* break */

                    __L28: /*case 2*/ 
                    __L29: /*case 3*/ 
                    goto __L1; /* break */

                }
                __L1:;
            }
            p_type = p_type->next;
        }
    }
    else __L0: /*catch*/ 
    {
    }
}

static void d_print_type_qualifier_flags(struct osstream * ss, unsigned char * first, int e_type_qualifier_flags)
{
    if (e_type_qualifier_flags & 1)
    {
        print_item(ss, first, "const");
    }
    if (e_type_qualifier_flags & 4)
    {
        print_item(ss, first, "volatile");
    }
}

static void d_print_type_core(struct d_visit_ctx * ctx, struct osstream * ss, struct type * p_type0, char * name_opt)
{
    struct type * p_type;

    p_type = p_type0;
    while (p_type)
    {
        /*switch*/
        {
            int __v0 = p_type->category;
            if (__v0 == 0) goto __L1; /*case 0*/
            if (__v0 == 2) goto __L14; /*case 2*/
            if (__v0 == 1) goto __L19; /*case 1*/
            if (__v0 == 3) goto __L27; /*case 3*/
            goto __L0;

            {
                unsigned char  b;
                struct param * pa;

                __L1: /*case 0*/ 
                {
                    struct osstream  local;
                    unsigned char  first;
                    struct osstream  local2;

                    _cake_zmem(&local, 12);
                    first = 1;
                    if (ctx->print_qualifiers)
                    {
                        d_print_type_qualifier_flags(&local, &first, p_type->type_qualifier_flags);
                    }
                    if (!first)
                    {
                        ss_fprintf(&local, " ");
                    }
                    if (p_type->struct_or_union_specifier)
                    {
                        struct struct_or_union_specifier * p_complete;

                        p_complete = get_complete_struct_or_union_specifier2(p_type->struct_or_union_specifier);
                        if (p_complete)
                        {
                            if (struct_or_union_specifier_is_union(p_type->struct_or_union_specifier))
                            {
                                ss_fprintf(&local, "union %s", p_complete->tag_name);
                            }
                            else
                            {
                                ss_fprintf(&local, "struct %s", p_complete->tag_name);
                            }
                        }
                        else
                        {
                            if (struct_or_union_specifier_is_union(p_type->struct_or_union_specifier))
                            {
                                ss_fprintf(&local, "union %s", p_type->struct_or_union_specifier->tag_name);
                            }
                            else
                            {
                                ss_fprintf(&local, "struct %s", p_type->struct_or_union_specifier->tag_name);
                            }
                        }
                    }
                    else
                    {
                        if (p_type->enum_specifier)
                        {
                            int enum_type_specifier_flags;

                            enum_type_specifier_flags = get_enum_type_specifier_flags(p_type->enum_specifier);
                            print_type_specifier_flags(&local, &first, enum_type_specifier_flags);
                        }
                        else
                        {
                            if (p_type->type_specifier_flags & 512)
                            {
                                unsigned char  first0;

                                first0 = 1;
                                print_type_specifier_flags(&local, &first0, object_type_to_type_specifier(get_platform(ctx->options.target)->bool_type));
                            }
                            else
                            {
                                print_type_alignment_flags(&local, &first, p_type->alignment_specifier_flags, ctx->options.target);
                                print_msvc_declspec(&local, &first, p_type->msvc_declspec_flags);
                                if (p_type->type_qualifier_flags & 4)
                                {
                                    print_item(&local, &first, "volatile");
                                }
                                print_type_specifier_flags(&local, &first, p_type->type_specifier_flags);
                            }
                        }
                    }
                    if (name_opt)
                    {
                        if (first)
                        {
                            ss_fprintf(ss, " ");
                            first = 0;
                        }
                        ss_fprintf(ss, "%s", name_opt);
                        name_opt = 0;
                    }
                    _cake_zmem(&local2, 12);
                    if (ss->c_str)
                    {
                        ss_fprintf(&local2, "%s %s", local.c_str, ss->c_str);
                    }
                    else
                    {
                        ss_fprintf(&local2, "%s", local.c_str);
                    }
                    ss_swap(ss, &local2);
                    ss_close(&local2);
                    ss_close(&local);
                }
                goto __L0; /* break */

                __L14: /*case 2*/ 
                if (name_opt)
                {
                    ss_fprintf(ss, "%s", name_opt);
                    name_opt = 0;
                }
                ss_fprintf(ss, "[");
                b = 1;
                if (ctx->print_qualifiers)
                {
                    d_print_type_qualifier_flags(ss, &b, p_type->type_qualifier_flags);
                }
                if (p_type->num_of_elements > 0)
                {
                    if (!b)
                    {
                        ss_fprintf(ss, " ");
                    }
                    ss_fprintf(ss, "%zu", p_type->num_of_elements);
                }
                else
                {
                }
                ss_fprintf(ss, "]");
                goto __L0; /* break */

                __L19: /*case 1*/ 
                if (name_opt)
                {
                    if (p_type->attributes_flags & 268435456)
                    {
                        ss_fprintf(ss, "__stdcall ");
                    }
                    else
                    {
                        if (p_type->attributes_flags & 134217728)
                        {
                            ss_fprintf(ss, "__fastcall ");
                        }
                        else
                        {
                            if (p_type->attributes_flags & 536870912)
                            {
                                ss_fprintf(ss, "__cdecl ");
                            }
                        }
                    }
                    ss_fprintf(ss, "%s", name_opt);
                    name_opt = 0;
                }
                ss_fprintf(ss, "(");
                pa = p_type->params.head;
                while (pa)
                {
                    struct osstream  sslocal;
                    struct osstream  local2;

                    _cake_zmem(&sslocal, 12);
                    _cake_zmem(&local2, 12);
                    d_print_type_core(ctx, &local2, &pa->type, pa->type.name_opt);
                    ss_fprintf(&sslocal, "%s", local2.c_str);
                    ss_fprintf(ss, "%s", sslocal.c_str);
                    if (pa->next)
                    {
                        ss_fprintf(ss, ", ");
                    }
                    ss_close(&sslocal);
                    pa = pa->next;
                    ss_close(&local2);
                }
                if (p_type->params.is_var_args)
                {
                    if (p_type->params.head)
                    {
                        ss_fprintf(ss, ", ");
                    }
                    ss_fprintf(ss, "...");
                }
                ss_fprintf(ss, ")");
                goto __L0; /* break */

                __L27: /*case 3*/ 
                {
                    struct osstream  local;
                    unsigned char  first;

                    _cake_zmem(&local, 12);
                    if (p_type->next && (p_type->next->category == 1 || p_type->next->category == 2))
                    {
                        ss_fprintf(&local, "(");
                    }
                    if (p_type->attributes_flags & 268435456)
                    {
                        ss_fprintf(&local, "__stdcall ");
                    }
                    else
                    {
                        if (p_type->attributes_flags & 134217728)
                        {
                            ss_fprintf(&local, "__fastcall ");
                        }
                        else
                        {
                            if (p_type->attributes_flags & 536870912)
                            {
                                ss_fprintf(&local, "__cdecl ");
                            }
                        }
                    }
                    ss_fprintf(&local, "*");
                    first = 0;
                    if (ctx->print_qualifiers)
                    {
                        d_print_type_qualifier_flags(ss, &b, p_type->type_qualifier_flags);
                    }
                    if (name_opt)
                    {
                        if (!first)
                        {
                            ss_fprintf(ss, " ");
                        }
                        ss_fprintf(ss, "%s", name_opt);
                        name_opt = 0;
                        first = 0;
                    }
                    if (ss->c_str)
                    {
                        ss_fprintf(&local, "%s", ss->c_str);
                    }
                    if (p_type->next && (p_type->next->category == 1 || p_type->next->category == 2))
                    {
                        ss_fprintf(&local, ")", ss->c_str);
                    }
                    ss_swap(ss, &local);
                    ss_close(&local);
                }
                goto __L0; /* break */

            }
            __L0:;
        }
        p_type = p_type->next;
    }
}

static void d_print_type(struct d_visit_ctx * ctx, struct osstream * ss, struct type * p_type, char * name_opt, unsigned char  print_storage_qualifiers)
{
    struct osstream  local;

    register_struct_types_and_functions(ctx, p_type, 0);
    _cake_zmem(&local, 12);
    d_print_type_core(ctx, &local, p_type, name_opt);
    if (print_storage_qualifiers && p_type->storage_class_specifier_flags & 2)
    {
        ss_fprintf(ss, "extern ");
    }
    if (print_storage_qualifiers && p_type->storage_class_specifier_flags & 4)
    {
        ss_fprintf(ss, "static ");
    }
    if (p_type->storage_class_specifier_flags & 8)
    {
        char * ta;

        ta = get_platform(ctx->options.target)->thread_local_attr;
        ss_fprintf(ss, "%s ", ta);
    }
    ss_fprintf(ss, "%s", local.c_str);
    ss_close(&local);
}

static void d_print_struct(struct d_visit_ctx * ctx, struct osstream * ss, struct struct_or_union_specifier * p_struct_or_union_specifier)
{
    struct struct_or_union_specifier * p_complete;
    struct member_declaration * member_declaration;
    int no_name_index;

    p_complete = get_complete_struct_or_union_specifier2(p_struct_or_union_specifier);
    if (p_complete == 0)
    {
        if (struct_or_union_specifier_is_union(p_struct_or_union_specifier))
        {
            ss_fprintf(ss, "union %s;\n\n", p_struct_or_union_specifier->tag_name);
        }
        else
        {
            ss_fprintf(ss, "struct %s;\n\n", p_struct_or_union_specifier->tag_name);
        }
        return;
    }
    member_declaration = p_complete->member_declaration_list.head;
    if (struct_or_union_specifier_is_union(p_complete))
    {
        ss_fprintf(ss, "union %s", p_complete->tag_name);
    }
    else
    {
        ss_fprintf(ss, "struct %s", p_complete->tag_name);
    }
    if (p_complete->member_declaration_list.head)
    {
        ss_fprintf(ss, " ");
        ss_fprintf(ss, "{\n");
    }
    no_name_index = 0;
    while (member_declaration)
    {
        struct member_declarator * member_declarator;

        member_declarator = 0;
        if (member_declaration->member_declarator_list_opt)
        {
            member_declarator = member_declaration->member_declarator_list_opt->head;
            while (member_declarator)
            {
                if (member_declarator->declarator && member_declarator->declarator->name_opt)
                {
                    ss_fprintf(ss, "    ");
                    if (type_is_array(&member_declarator->declarator->type) && member_declarator->declarator->type.num_of_elements == 0)
                    {
                        member_declarator->declarator->type.num_of_elements = 1;
                        d_print_type(ctx, ss, &member_declarator->declarator->type, member_declarator->declarator->name_opt->lexeme, 0);
                        member_declarator->declarator->type.num_of_elements = 0;
                    }
                    else
                    {
                        d_print_type(ctx, ss, &member_declarator->declarator->type, member_declarator->declarator->name_opt->lexeme, 0);
                    }
                    ss_fprintf(ss, ";\n");
                }
                member_declarator = member_declarator->next;
            }
        }
        else
        {
            if (member_declaration->specifier_qualifier_list != 0)
            {
                if (member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                {
                    struct type  t;
                    char name[100];

                    _cake_zmem(&t, 68);
                    t.category = 0;
                    t.struct_or_union_specifier = member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                    t.type_specifier_flags = 32768;
                    _cake_zmem(&name, 100);
                    snprintf(name, 100, "__m%d", no_name_index++);
                    ss_fprintf(ss, "    ");
                    d_print_type(ctx, ss, &t, name, 0);
                    ss_fprintf(ss, ";\n");
                    type_destroy(&t);
                }
            }
        }
        member_declaration = member_declaration->next;
    }
    if (p_complete->member_declaration_list.head)
    {
        ss_fprintf(ss, "};\n\n");
    }
    else
    {
        ss_fprintf(ss, ";\n");
    }
}
void ss_clear(struct osstream * stream);
static void d_visit_declaration(struct d_visit_ctx * ctx, struct osstream * oss, struct declaration * p_declaration);
static void d_visit_init_declarator_list(struct d_visit_ctx * ctx, struct osstream * oss, struct init_declarator_list * p_init_declarator_list, int function_specifier_flags, int storage_class_specifier_flags);
static void d_visit_init_declarator(struct d_visit_ctx * ctx, struct osstream * oss, struct init_declarator * p_init_declarator, int function_specifier_flags, int storage_class_specifier_flags);
unsigned char type_is_function(struct type * p_type);
static void print_initializer(struct d_visit_ctx * ctx, struct osstream * oss, struct init_declarator * p_init_declarator, unsigned char  bstatic);
static void print_identation_core(struct osstream * ss, int indentation);
static void d_visit_expression(struct d_visit_ctx * ctx, struct osstream * oss, struct expression * p_expression);
unsigned char object_has_constant_value(struct object * a);
unsigned char type_is_void_ptr(struct type * p_type);
unsigned char type_is_nullptr_t(struct type * p_type);
unsigned char object_is_zero(struct object * p_object);
unsigned char type_is_arithmetic(struct type * p_type);
void object_print_value(struct osstream * ss, struct object * a, int target);
struct declarator *declarator_get_function_definition(struct declarator * p);
static void d_visit_function_body(struct d_visit_ctx * ctx, struct osstream * oss, struct declarator * function_definition);
static void d_visit_compound_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct compound_statement * p_compound_statement);
static void d_visit_block_item_list(struct d_visit_ctx * ctx, struct osstream * oss, struct block_item_list * p_block_item_list);
static void d_visit_block_item(struct d_visit_ctx * ctx, struct osstream * oss, struct block_item * p_block_item);
static void d_visit_unlabeled_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct unlabeled_statement * p_unlabeled_statement);
static void d_visit_primary_block(struct d_visit_ctx * ctx, struct osstream * oss, struct primary_block * p_primary_block);
static void d_visit_iteration_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct iteration_statement * p_iteration_statement);
static void print_identation(struct d_visit_ctx * ctx, struct osstream * oss);
static void d_visit_secondary_block(struct d_visit_ctx * ctx, struct osstream * oss, struct secondary_block * p_secondary_block);
static void d_visit_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct statement * p_statement);
static void d_visit_labeled_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct labeled_statement * p_labeled_statement);
static void d_visit_label(struct d_visit_ctx * ctx, struct osstream * oss, struct label * p_label);
int object_to_str(struct object * a, int n, char str[]);
static void d_visit_selection_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct selection_statement * p_selection_statement);
static void d_visit_condition(struct d_visit_ctx * ctx, struct osstream * oss, struct condition * p_condition);
static void d_visit_init_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct init_statement * p_init_statement);
static void d_visit_expression_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct expression_statement * p_expression_statement);
static void d_visit_simple_declaration(struct d_visit_ctx * ctx, struct osstream * oss, struct simple_declaration * p_simple_declaration);
static unsigned char is_compound_statement(struct secondary_block * p_secondary_block);
static void d_visit_try_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct try_statement * p_try_statement);
static void d_visit_jump_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct jump_statement * p_jump_statement);
static void il_print_defer_list(struct d_visit_ctx * ctx, struct osstream * oss, struct defer_list * p_defer_list);
static void il_print_defer_new(struct d_visit_ctx * ctx, struct osstream * oss, struct defer_list_item * p_item);
struct type get_function_return_type(struct type * p_type);
static int il_defer_count(struct defer_list * p_defer_list);
unsigned char type_is_bool(struct type * p_type);
static void expression_to_bool_value(struct d_visit_ctx * ctx, struct osstream * oss, struct expression * p_expression);
unsigned char object_is_true(struct object * a);
unsigned char type_is_essential_bool(struct type * p_type);
static void il_print_string(struct token * first_token, struct token * last_token, struct osstream * oss);
static int il_visit_literal_string2(struct token * current, struct osstream * oss);
static int find_member_name(struct type * p_type, int index, char name[100]);
static struct member_declarator *find_member_declarator_name_by_index(struct member_declaration_list * list, int member_index, char name[100], int * p_count);
struct type type_remove_pointer(struct type * p_type);
static void object_print_non_constant_initialization(struct d_visit_ctx * ctx, struct osstream * ss, struct object * object, char * declarator_name, unsigned char  all, unsigned char  initialize_objects_that_does_not_have_initializer);
unsigned char object_is_reference(struct object * p_object);
struct object *object_get_referenced(struct object * p_object);
unsigned char type_is_union(struct type * p_type);
int type_get_sizeof(struct type * p_type, unsigned int * size, int target);
static void object_print_constant_initialization(struct d_visit_ctx * ctx, struct osstream * ss, struct object * object, unsigned char * first);
static char *get_op_by_expression_type(int type);
static unsigned char is_all_zero(struct object * object);
char *strdup(char * src);

void d_visit(struct d_visit_ctx * ctx, struct osstream * oss)
{
    struct osstream  declarations;
    struct declaration * p_declaration;

    _cake_zmem(&declarations, 12);
    ctx->print_qualifiers = 0;
    ss_fprintf(oss, "/* Cake %s %s */\n", "0.12.26", get_platform(ctx->options.target)->name);
    ctx->indentation = 0;
    p_declaration = ctx->ast.declaration_list.head;
    while (p_declaration)
    {
        struct osstream  declaration;

        ss_clear(&ctx->add_this_after_external_decl);
        ss_clear(&ctx->add_this_before_external_decl);
        _cake_zmem(&declaration, 12);
        d_visit_declaration(ctx, &declaration, p_declaration);
        if (ctx->add_this_before.size > 0)
        {
            ss_fprintf(oss, "%s", ctx->add_this_before.c_str);
            ss_clear(&ctx->add_this_before);
        }
        if (ctx->add_this_before_external_decl.size > 0)
        {
            ss_fprintf(&declarations, "%s\n", ctx->add_this_before_external_decl.c_str);
            ss_clear(&ctx->add_this_before_external_decl);
        }
        if (declaration.size > 0)
        {
            ss_fprintf(&declarations, "%s", declaration.c_str);
        }
        if (ctx->add_this_after_external_decl.c_str)
        {
            ss_fprintf(&declarations, "%s", ctx->add_this_after_external_decl.c_str);
        }
        ss_close(&declaration);
        p_declaration = p_declaration->next;
    }
    {
        int i;

        i = 0;
        for (; i < ctx->structs_map.capacity; i++)
        {
            struct map_entry * entry;

            entry = ctx->structs_map.table[i];
            while (entry)
            {
                ;
                d_print_structs(ctx, oss, entry->data.p_struct_entry);
                entry = entry->next;
            }
        }
    }
    ss_fprintf(oss, "\n");
    if (ctx->define_nullptr && ctx->null_pointer_constant_used)
    {
        ss_fprintf(oss, "static const void* NULL = 0;\n");
    }
    if (ctx->zero_mem_used)
    {
        struct osstream  local;
        unsigned char  first;

        _cake_zmem(&local, 12);
        first = 1;
        print_type_specifier_flags(&local, &first, object_type_to_type_specifier(get_platform(ctx->options.target)->size_t_type));
        ss_fprintf(oss, "static void _cake_zmem(void *dest, %s len)\n{\n  unsigned char *ptr;\n\n  ptr = (unsigned char*)dest;\n  while (len-- > 0) *ptr++ = 0;\n}\n\n", local.c_str);
        ss_close(&local);
    }
    if (ctx->memcpy_used)
    {
        struct osstream  local;
        unsigned char  first;

        _cake_zmem(&local, 12);
        first = 1;
        print_type_specifier_flags(&local, &first, object_type_to_type_specifier(get_platform(ctx->options.target)->size_t_type));
        ss_fprintf(oss, "static void _cake_memcpy(void * dest, const void * src, %s n)\n{\n  char *csrc;\n  char *cdest;\n  %s i; \n\n  csrc = (char *)src;\n  cdest = (char *)dest;\n  for (i = 0; i < n; i++) cdest[i] = csrc[i]; \n}\n\n", local.c_str, local.c_str);
        ss_close(&local);
    }
    if (declarations.c_str)
    {
        ss_fprintf(oss, "%s", declarations.c_str);
    }
    ss_close(&declarations);
}



static void print_identation_core(struct osstream * ss, int indentation)
{
    {
        int i;

        i = 0;
        for (; i < indentation; i++)
        {
            ss_fprintf(ss, "    ");
        }
    }
}

static void print_identation(struct d_visit_ctx * ctx, struct osstream * oss)
{
    {
        int i;

        i = 0;
        for (; i < ctx->indentation; i++)
        {
            ss_fprintf(oss, "    ");
        }
    }
}

static void d_visit_label(struct d_visit_ctx * ctx, struct osstream * oss, struct label * p_label)
{
    if (p_label->p_first_token->type == 9001)
    {
        char str[50];

        print_identation(ctx, oss);
        ss_fprintf(oss, "__L%d:", p_label->label_id);
        _cake_zmem(&str, 50);
        object_to_str(&p_label->constant_expression->object, 50, str);
        if (p_label->constant_expression_end == 0)
        {
            ss_fprintf(oss, " /*case %s*/ ", str);
        }
        else
        {
            char str2[50];

            _cake_zmem(&str2, 50);
            object_to_str(&p_label->constant_expression_end->object, 50, str2);
            ss_fprintf(oss, " /*case %s ... %s*/ ", str, str2);
        }
        ss_fprintf(oss, "\n");
    }
    else
    {
        if (p_label->p_first_token->type == 8996)
        {
            print_identation(ctx, oss);
            ss_fprintf(oss, "%s:\n", p_label->p_first_token->lexeme);
        }
        else
        {
            if (p_label->p_first_token->type == 9007)
            {
                print_identation(ctx, oss);
                ss_fprintf(oss, "__L%d: /* default */ \n", p_label->label_id);
            }
        }
    }
}

static void d_visit_labeled_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct labeled_statement * p_labeled_statement)
{
    ;
    d_visit_label(ctx, oss, p_labeled_statement->label);
    d_visit_statement(ctx, oss, p_labeled_statement->statement);
}

static void d_visit_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct statement * p_statement)
{
    if (p_statement->labeled_statement)
    {
        d_visit_labeled_statement(ctx, oss, p_statement->labeled_statement);
    }
    else
    {
        if (p_statement->unlabeled_statement)
        {
            d_visit_unlabeled_statement(ctx, oss, p_statement->unlabeled_statement);
        }
    }
}

static void d_visit_secondary_block(struct d_visit_ctx * ctx, struct osstream * oss, struct secondary_block * p_secondary_block)
{
    d_visit_statement(ctx, oss, p_secondary_block->statement);
}

static void d_visit_iteration_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct iteration_statement * p_iteration_statement)
{
    struct break_reference  old;

    old = ctx->break_reference;
    ctx->break_reference.p_iteration_statement = p_iteration_statement;
    ctx->break_reference.p_selection_statement = 0;
    print_identation(ctx, oss);
    if (p_iteration_statement->first_token->type == 9042)
    {
        ss_fprintf(oss, "while (");
        if (p_iteration_statement->expression1)
        {
            d_visit_expression(ctx, oss, p_iteration_statement->expression1);
        }
        ss_fprintf(oss, ")\n");
        d_visit_secondary_block(ctx, oss, p_iteration_statement->secondary_block);
    }
    else
    {
        if (p_iteration_statement->first_token->type == 9008)
        {
            ss_fprintf(oss, "do\n");
            d_visit_secondary_block(ctx, oss, p_iteration_statement->secondary_block);
            print_identation(ctx, oss);
            ss_fprintf(oss, "while (");
            ;
            d_visit_expression(ctx, oss, p_iteration_statement->expression1);
            ss_fprintf(oss, ");\n");
        }
        else
        {
            if (p_iteration_statement->first_token->type == 9015)
            {
                if (p_iteration_statement->declaration)
                {
                    struct osstream  block_scope_declarators;
                    struct osstream  local;

                    ss_fprintf(oss, "{\n");
                    ctx->indentation++;
                    _cake_zmem(&block_scope_declarators, 12);
                    ss_swap(&block_scope_declarators, &ctx->block_scope_declarators);
                    _cake_zmem(&local, 12);
                    d_visit_declaration(ctx, &local, p_iteration_statement->declaration);
                    if (ctx->block_scope_declarators.c_str)
                    {
                        ss_fprintf(oss, "%s", ctx->block_scope_declarators.c_str);
                        ss_fprintf(oss, "\n");
                    }
                    if (local.c_str)
                    {
                        ss_fprintf(oss, "%s", local.c_str);
                    }
                    ss_swap(&block_scope_declarators, &ctx->block_scope_declarators);
                    print_identation(ctx, oss);
                    ss_fprintf(oss, "for (");
                    ss_close(&block_scope_declarators);
                    ss_close(&local);
                }
                else
                {
                    if (p_iteration_statement->expression0)
                    {
                        ss_fprintf(oss, "for (");
                        d_visit_expression(ctx, oss, p_iteration_statement->expression0);
                    }
                    else
                    {
                        ss_fprintf(oss, "for (");
                    }
                }
                ss_fprintf(oss, "; ");
                if (p_iteration_statement->expression1)
                {
                    d_visit_expression(ctx, oss, p_iteration_statement->expression1);
                }
                ss_fprintf(oss, "; ");
                if (p_iteration_statement->expression2)
                {
                    d_visit_expression(ctx, oss, p_iteration_statement->expression2);
                }
                ss_fprintf(oss, ")\n");
                d_visit_secondary_block(ctx, oss, p_iteration_statement->secondary_block);
                if (p_iteration_statement->declaration)
                {
                    ctx->indentation--;
                    print_identation(ctx, oss);
                    ss_fprintf(oss, "}\n");
                }
            }
        }
    }
    ctx->break_reference = old;
}

static void d_visit_condition(struct d_visit_ctx * ctx, struct osstream * oss, struct condition * p_condition)
{
    if (p_condition->p_init_declarator)
    {
        d_visit_init_declarator(ctx, oss, p_condition->p_init_declarator, 0, 0);
    }
    if (p_condition->expression)
    {
        d_visit_expression(ctx, oss, p_condition->expression);
    }
}

static void d_visit_expression_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct expression_statement * p_expression_statement)
{
    struct osstream  local;

    ss_clear(&ctx->add_this_before);
    _cake_zmem(&local, 12);
    print_identation(ctx, &local);
    if (p_expression_statement->expression_opt)
    {
        d_visit_expression(ctx, &local, p_expression_statement->expression_opt);
    }
    if (ctx->add_this_before.size > 0)
    {
        ss_fprintf(oss, "%s", ctx->add_this_before.c_str);
        ss_clear(&ctx->add_this_before);
    }
    ss_fprintf(oss, "%s;\n", local.c_str);
    ss_close(&local);
}

static void d_visit_simple_declaration(struct d_visit_ctx * ctx, struct osstream * oss, struct simple_declaration * p_simple_declaration)
{
    d_visit_init_declarator_list(ctx, oss, &p_simple_declaration->init_declarator_list, 0, 0);
}

static void d_visit_init_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct init_statement * p_init_statement)
{
    if (p_init_statement->p_expression_statement)
    {
        d_visit_expression_statement(ctx, oss, p_init_statement->p_expression_statement);
    }
    if (p_init_statement->p_simple_declaration)
    {
        d_visit_simple_declaration(ctx, oss, p_init_statement->p_simple_declaration);
    }
}

static unsigned char is_compound_statement(struct secondary_block * p_secondary_block)
{
    ;
    if (p_secondary_block->statement->unlabeled_statement && p_secondary_block->statement->unlabeled_statement->primary_block && p_secondary_block->statement->unlabeled_statement->primary_block->compound_statement)
    {
        return 1;
    }
    return 0;
}

static void d_visit_selection_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct selection_statement * p_selection_statement)
{
    print_identation(ctx, oss);
    if (p_selection_statement->first_token->type == 9034)
    {
        struct break_reference  old;
        struct osstream  ss;
        char name[100];
        struct label * p_label;
        struct label * p_label_default;

        old = ctx->break_reference;
        ctx->break_reference.p_iteration_statement = 0;
        ctx->break_reference.p_selection_statement = p_selection_statement;
        ;
        _cake_zmem(&ss, 12);
        ss_fprintf(&ss, "/*switch*/\n");
        print_identation(ctx, &ss);
        ss_fprintf(&ss, "{\n");
        ctx->indentation++;
        _cake_zmem(&name, 100);
        snprintf(name, 100, "__v%d", ctx->cake_local_declarator_number++);
        print_identation(ctx, &ss);
        d_print_type(ctx, &ss, &p_selection_statement->condition->expression->type, name, 0);
        ss_fprintf(&ss, " = ");
        ;
        d_visit_condition(ctx, &ss, p_selection_statement->condition);
        ss_fprintf(&ss, ";\n");
        p_label = p_selection_statement->label_list.head;
        p_label_default = 0;
        while (p_label)
        {
            if (p_label->p_first_token->type == 9007)
            {
                p_label_default = p_label;
            }
            else
            {
                print_identation(ctx, &ss);
                if (p_label->constant_expression_end == 0)
                {
                    char str[50];

                    _cake_zmem(&str, 50);
                    object_to_str(&p_label->constant_expression->object, 50, str);
                    ss_fprintf(&ss, "if (%s == %s) goto __L%d; /*case %s*/\n", name, str, p_label->label_id, str);
                }
                else
                {
                    char str_begin[50];
                    char str_end[50];

                    _cake_zmem(&str_begin, 50);
                    object_to_str(&p_label->constant_expression->object, 50, str_begin);
                    _cake_zmem(&str_end, 50);
                    object_to_str(&p_label->constant_expression_end->object, 50, str_end);
                    ss_fprintf(&ss, "if (%s >= %s && %s <= %s) goto __L%d; /*case %s ... %s*/\n", name, str_begin, name, str_end, p_label->label_id, str_begin, str_end);
                }
            }
            p_label = p_label->next;
        }
        print_identation(ctx, &ss);
        if (p_label_default)
        {
            ss_fprintf(&ss, "goto __L%d; /* default */\n", p_label_default->label_id);
        }
        else
        {
            ss_fprintf(&ss, "goto __L%d;\n", p_selection_statement->label_id);
        }
        ss_fprintf(&ss, "\n");
        d_visit_secondary_block(ctx, &ss, p_selection_statement->secondary_block);
        print_identation(ctx, &ss);
        ss_fprintf(&ss, "__L%d:;\n", ctx->break_reference.p_selection_statement->label_id);
        ctx->indentation--;
        print_identation(ctx, &ss);
        ss_fprintf(&ss, "}\n");
        ss_fprintf(oss, "%s", ss.c_str);
        ss_close(&ss);
        ctx->break_reference = old;
    }
    else
    {
        if (p_selection_statement->first_token->type == 9017)
        {
            unsigned char  addclose;

            addclose = 0;
            if (p_selection_statement->p_init_statement)
            {
                struct osstream  block_scope_declarators;
                struct osstream  local2;

                ss_fprintf(oss, "{\n");
                addclose = 1;
                ctx->indentation++;
                _cake_zmem(&block_scope_declarators, 12);
                ss_swap(&block_scope_declarators, &ctx->block_scope_declarators);
                _cake_zmem(&local2, 12);
                print_identation(ctx, &local2);
                d_visit_init_statement(ctx, &local2, p_selection_statement->p_init_statement);
                if (ctx->block_scope_declarators.c_str)
                {
                    ss_fprintf(oss, "%s", ctx->block_scope_declarators.c_str);
                    ss_fprintf(oss, "\n");
                }
                ss_fprintf(oss, "%s", local2.c_str);
                ss_close(&local2);
                print_identation(ctx, oss);
                ss_swap(&block_scope_declarators, &ctx->block_scope_declarators);
                ss_close(&block_scope_declarators);
            }
            if (p_selection_statement->condition)
            {
                if (p_selection_statement->condition->p_init_declarator)
                {
                    struct osstream  block_scope_declarators;
                    struct osstream  local2;

                    ss_fprintf(oss, "{\n");
                    ctx->indentation++;
                    addclose = 1;
                    _cake_zmem(&block_scope_declarators, 12);
                    ss_swap(&block_scope_declarators, &ctx->block_scope_declarators);
                    _cake_zmem(&local2, 12);
                    print_identation(ctx, &local2);
                    d_visit_init_declarator(ctx, &local2, p_selection_statement->condition->p_init_declarator, 0, 0);
                    if (ctx->block_scope_declarators.c_str)
                    {
                        ss_fprintf(oss, "%s", ctx->block_scope_declarators.c_str);
                        ss_fprintf(oss, "\n");
                    }
                    ss_fprintf(oss, "%s", local2.c_str);
                    ss_close(&local2);
                    ss_swap(&block_scope_declarators, &ctx->block_scope_declarators);
                    print_identation(ctx, oss);
                    ss_fprintf(oss, "if (");
                    ss_fprintf(oss, "%s", p_selection_statement->condition->p_init_declarator->p_declarator->name_opt->lexeme);
                    ss_fprintf(oss, ")\n");
                    ss_close(&block_scope_declarators);
                }
                else
                {
                    if (p_selection_statement->condition->expression)
                    {
                        ss_fprintf(oss, "if (");
                        d_visit_expression(ctx, oss, p_selection_statement->condition->expression);
                        ss_fprintf(oss, ")\n");
                    }
                }
            }
            ;
            if (is_compound_statement(p_selection_statement->secondary_block))
            {
                d_visit_secondary_block(ctx, oss, p_selection_statement->secondary_block);
            }
            else
            {
                print_identation(ctx, oss);
                ss_fprintf(oss, "{\n");
                ctx->indentation++;
                d_visit_secondary_block(ctx, oss, p_selection_statement->secondary_block);
                ctx->indentation--;
                print_identation(ctx, oss);
                ss_fprintf(oss, "}\n");
            }
            if (p_selection_statement->else_secondary_block_opt)
            {
                print_identation(ctx, oss);
                ss_fprintf(oss, "else\n");
                if (is_compound_statement(p_selection_statement->else_secondary_block_opt))
                {
                    d_visit_secondary_block(ctx, oss, p_selection_statement->else_secondary_block_opt);
                }
                else
                {
                    print_identation(ctx, oss);
                    ss_fprintf(oss, "{\n");
                    ctx->indentation++;
                    d_visit_secondary_block(ctx, oss, p_selection_statement->else_secondary_block_opt);
                    ctx->indentation--;
                    print_identation(ctx, oss);
                    ss_fprintf(oss, "}\n");
                }
            }
            if (addclose)
            {
                ctx->indentation--;
                print_identation(ctx, oss);
                ss_fprintf(oss, "}\n");
            }
        }
    }
}

static void d_visit_try_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct try_statement * p_try_statement)
{
    print_identation(ctx, oss);
    ss_fprintf(oss, "if (1) /*try*/\n");
    d_visit_secondary_block(ctx, oss, p_try_statement->secondary_block);
    print_identation(ctx, oss);
    ss_fprintf(oss, "else __L%d: /*catch*/ \n", p_try_statement->catch_label_id);
    if (p_try_statement->catch_secondary_block_opt)
    {
        d_visit_secondary_block(ctx, oss, p_try_statement->catch_secondary_block_opt);
    }
}

static void d_visit_primary_block(struct d_visit_ctx * ctx, struct osstream * oss, struct primary_block * p_primary_block)
{
    if (p_primary_block->defer_statement)
    {
    }
    else
    {
        if (p_primary_block->compound_statement)
        {
            d_visit_compound_statement(ctx, oss, p_primary_block->compound_statement);
        }
        else
        {
            if (p_primary_block->iteration_statement)
            {
                d_visit_iteration_statement(ctx, oss, p_primary_block->iteration_statement);
            }
            else
            {
                if (p_primary_block->selection_statement)
                {
                    d_visit_selection_statement(ctx, oss, p_primary_block->selection_statement);
                }
                else
                {
                    if (p_primary_block->try_statement)
                    {
                        d_visit_try_statement(ctx, oss, p_primary_block->try_statement);
                    }
                }
            }
        }
    }
}

static void il_print_defer_new(struct d_visit_ctx * ctx, struct osstream * oss, struct defer_list_item * p_item)
{
    if (p_item->defer_statement == 0)
    {
        return;
    }
    d_visit_secondary_block(ctx, oss, p_item->defer_statement->secondary_block);
}

static void il_print_defer_list(struct d_visit_ctx * ctx, struct osstream * oss, struct defer_list * p_defer_list)
{
    struct defer_list_item * p_item;

    p_item = p_defer_list->head;
    while (p_item)
    {
        il_print_defer_new(ctx, oss, p_item);
        p_item = p_item->next;
    }
}

static int il_defer_count(struct defer_list * p_defer_list)
{
    int count;
    struct defer_list_item * p_item;

    count = 0;
    p_item = p_defer_list->head;
    while (p_item)
    {
        if (p_item->defer_statement)
        {
            count++;
        }
        p_item = p_item->next;
    }
    return count;
}

static void expression_to_bool_value(struct d_visit_ctx * ctx, struct osstream * oss, struct expression * p_expression)
{
    if (object_has_constant_value(&p_expression->object))
    {
        if (object_is_true(&p_expression->object))
        {
            ss_fprintf(oss, "1");
        }
        else
        {
            ss_fprintf(oss, "0");
        }
    }
    else
    {
        if (type_is_bool(&p_expression->type) || type_is_essential_bool(&p_expression->type))
        {
            d_visit_expression(ctx, oss, p_expression);
        }
        else
        {
            /*switch*/
            {
                int __v0 = p_expression->expression_type;
                if (__v0 == 53) goto __L4; /*case 53*/
                if (__v0 == 54) goto __L5; /*case 54*/
                if (__v0 == 55) goto __L6; /*case 55*/
                if (__v0 == 56) goto __L7; /*case 56*/
                if (__v0 == 57) goto __L8; /*case 57*/
                if (__v0 == 58) goto __L9; /*case 58*/
                if (__v0 == 59) goto __L10; /*case 59*/
                if (__v0 == 60) goto __L11; /*case 60*/
                if (__v0 == 61) goto __L12; /*case 61*/
                if (__v0 == 62) goto __L13; /*case 62*/
                if (__v0 == 63) goto __L14; /*case 63*/
                if (__v0 == 64) goto __L15; /*case 64*/
                if (__v0 == 65) goto __L16; /*case 65*/
                if (__v0 == 66) goto __L17; /*case 66*/
                if (__v0 == 67) goto __L18; /*case 67*/
                if (__v0 == 68) goto __L19; /*case 68*/
                if (__v0 == 69) goto __L20; /*case 69*/
                if (__v0 == 70) goto __L21; /*case 70*/
                if (__v0 == 71) goto __L22; /*case 71*/
                if (__v0 == 72) goto __L23; /*case 72*/
                goto __L24; /* default */

                {
                    __L4: /*case 53*/ 
                    __L5: /*case 54*/ 
                    __L6: /*case 55*/ 
                    __L7: /*case 56*/ 
                    __L8: /*case 57*/ 
                    __L9: /*case 58*/ 
                    __L10: /*case 59*/ 
                    __L11: /*case 60*/ 
                    __L12: /*case 61*/ 
                    __L13: /*case 62*/ 
                    __L14: /*case 63*/ 
                    __L15: /*case 64*/ 
                    __L16: /*case 65*/ 
                    __L17: /*case 66*/ 
                    __L18: /*case 67*/ 
                    __L19: /*case 68*/ 
                    __L20: /*case 69*/ 
                    __L21: /*case 70*/ 
                    __L22: /*case 71*/ 
                    __L23: /*case 72*/ 
                    ss_fprintf(oss, "((");
                    d_visit_expression(ctx, oss, p_expression);
                    ss_fprintf(oss, ") != 0)");
                    goto __L3; /* break */

                    __L24: /* default */ 
                    ss_fprintf(oss, "(");
                    d_visit_expression(ctx, oss, p_expression);
                    ss_fprintf(oss, " != 0)");
                    goto __L3; /* break */

                }
                __L3:;
            }
        }
    }
}

static void d_visit_jump_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct jump_statement * p_jump_statement)
{
    if (p_jump_statement->first_token->type == 9037)
    {
        il_print_defer_list(ctx, oss, &p_jump_statement->defer_list);
        print_identation(ctx, oss);
        ss_fprintf(oss, "goto __L%d; /* throw */\n", p_jump_statement->label_id);
    }
    else
    {
        if (p_jump_statement->first_token->type == 9027)
        {
            struct type  return_type;
            unsigned char  constant_expression_or_void;

            return_type = get_function_return_type(&ctx->p_current_function_opt->type);
            constant_expression_or_void = p_jump_statement->expression_opt == 0 || object_has_constant_value(&p_jump_statement->expression_opt->object) || il_defer_count(&p_jump_statement->defer_list) == 0;
            if (constant_expression_or_void)
            {
                il_print_defer_list(ctx, oss, &p_jump_statement->defer_list);
                print_identation(ctx, oss);
                ss_fprintf(oss, "return");
                if (p_jump_statement->expression_opt)
                {
                    ss_fprintf(oss, " ");
                    if (type_is_bool(&return_type))
                    {
                        expression_to_bool_value(ctx, oss, p_jump_statement->expression_opt);
                    }
                    else
                    {
                        d_visit_expression(ctx, oss, p_jump_statement->expression_opt);
                    }
                }
                ss_fprintf(oss, ";\n");
            }
            else
            {
                char name[100];

                _cake_zmem(&name, 100);
                snprintf(name, 100, "__v%d", ctx->cake_local_declarator_number++);
                {
                    struct osstream  local;

                    _cake_zmem(&local, 12);
                    print_identation(ctx, &local);
                    d_print_type(ctx, &local, &return_type, name, 0);
                    ss_fprintf(&local, ";\n", name);
                    ss_fprintf(&ctx->block_scope_declarators, "%s", local.c_str);
                    ss_close(&local);
                }
                print_identation(ctx, oss);
                ss_fprintf(oss, "%s = ", name);
                if (p_jump_statement->expression_opt)
                {
                    if (type_is_bool(&return_type))
                    {
                        expression_to_bool_value(ctx, oss, p_jump_statement->expression_opt);
                    }
                    else
                    {
                        d_visit_expression(ctx, oss, p_jump_statement->expression_opt);
                    }
                }
                ss_fprintf(oss, ";\n");
                il_print_defer_list(ctx, oss, &p_jump_statement->defer_list);
                print_identation(ctx, oss);
                ss_fprintf(oss, "return %s;\n", name);
            }
            type_destroy(&return_type);
        }
        else
        {
            if (p_jump_statement->first_token->type == 9000 || p_jump_statement->first_token->type == 9005)
            {
                il_print_defer_list(ctx, oss, &p_jump_statement->defer_list);
                print_identation(ctx, oss);
                if (p_jump_statement->first_token->type == 9000)
                {
                    if (ctx->break_reference.p_selection_statement)
                    {
                        ss_fprintf(oss, "goto __L%d; /* break */\n\n", ctx->break_reference.p_selection_statement->label_id);
                    }
                    else
                    {
                        ss_fprintf(oss, "break;\n");
                    }
                }
                else
                {
                    ss_fprintf(oss, "continue;\n");
                }
            }
            else
            {
                if (p_jump_statement->first_token->type == 9016)
                {
                    il_print_defer_list(ctx, oss, &p_jump_statement->defer_list);
                    print_identation(ctx, oss);
                    ss_fprintf(oss, "goto %s;\n", p_jump_statement->label->lexeme);
                }
                else
                {
                    ;
                }
            }
        }
    }
}

static void d_visit_unlabeled_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct unlabeled_statement * p_unlabeled_statement)
{
    if (p_unlabeled_statement->primary_block)
    {
        d_visit_primary_block(ctx, oss, p_unlabeled_statement->primary_block);
    }
    else
    {
        if (p_unlabeled_statement->expression_statement)
        {
            d_visit_expression_statement(ctx, oss, p_unlabeled_statement->expression_statement);
        }
        else
        {
            if (p_unlabeled_statement->jump_statement)
            {
                d_visit_jump_statement(ctx, oss, p_unlabeled_statement->jump_statement);
            }
            else
            {
                ;
            }
        }
    }
}

static void d_visit_block_item(struct d_visit_ctx * ctx, struct osstream * oss, struct block_item * p_block_item)
{
    struct osstream  ss0;

    _cake_zmem(&ss0, 12);
    ss_swap(oss, &ss0);
    ss_clear(&ctx->add_this_before);
    if (p_block_item->declaration)
    {
        d_visit_declaration(ctx, oss, p_block_item->declaration);
    }
    else
    {
        if (p_block_item->unlabeled_statement)
        {
            d_visit_unlabeled_statement(ctx, oss, p_block_item->unlabeled_statement);
        }
        else
        {
            if (p_block_item->label)
            {
                d_visit_label(ctx, oss, p_block_item->label);
            }
        }
    }
    if (ctx->add_this_before.size > 0)
    {
        ss_fprintf(&ss0, "%s", ctx->add_this_before.c_str);
        ss_clear(&ctx->add_this_before);
    }
    if (oss->c_str)
    {
        ss_fprintf(&ss0, "%s", oss->c_str);
    }
    ss_swap(oss, &ss0);
    ss_close(&ss0);
}

static void d_visit_block_item_list(struct d_visit_ctx * ctx, struct osstream * oss, struct block_item_list * p_block_item_list)
{
    struct block_item * p_block_item;

    p_block_item = p_block_item_list->head;
    while (p_block_item)
    {
        d_visit_block_item(ctx, oss, p_block_item);
        p_block_item = p_block_item->next;
    }
}

static void d_visit_compound_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct compound_statement * p_compound_statement)
{
    unsigned char  is_local;
    struct osstream  block_scope_declarators;
    struct osstream  local;
    unsigned char  ends_with_jump;

    is_local = ctx->is_local;
    ctx->is_local = 1;
    _cake_zmem(&block_scope_declarators, 12);
    ss_swap(&ctx->block_scope_declarators, &block_scope_declarators);
    _cake_zmem(&local, 12);
    ctx->indentation++;
    d_visit_block_item_list(ctx, &local, &p_compound_statement->block_item_list);
    ends_with_jump = 0;
    if (p_compound_statement->block_item_list.tail && p_compound_statement->block_item_list.tail->unlabeled_statement && p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement != 0)
    {
        ends_with_jump = 1;
    }
    if (!ends_with_jump)
    {
        il_print_defer_list(ctx, &local, &p_compound_statement->defer_list);
    }
    ctx->indentation--;
    print_identation(ctx, oss);
    ss_fprintf(oss, "{\n");
    if (ctx->block_scope_declarators.c_str)
    {
        ss_fprintf(oss, "%s", ctx->block_scope_declarators.c_str);
        ss_fprintf(oss, "\n");
    }
    if (local.c_str)
    {
        ss_fprintf(oss, "%s", local.c_str);
    }
    print_identation(ctx, oss);
    ss_fprintf(oss, "}\n");
    ctx->is_local = is_local;
    ss_swap(&ctx->block_scope_declarators, &block_scope_declarators);
    ss_close(&block_scope_declarators);
    ss_close(&local);
}

static void d_visit_function_body(struct d_visit_ctx * ctx, struct osstream * oss, struct declarator * function_definition)
{
    int indentation;
    struct declarator * previous_func;

    if (function_definition->function_body == 0)
    {
        ;
        return;
    }
    ctx->cake_local_declarator_number = 0;
    indentation = ctx->indentation;
    ctx->indentation = 0;
    previous_func = ctx->p_current_function_opt;
    ctx->p_current_function_opt = function_definition;
    d_visit_compound_statement(ctx, oss, function_definition->function_body);
    ctx->p_current_function_opt = previous_func;
    ctx->indentation = indentation;
}

static int il_visit_literal_string2(struct token * current, struct osstream * oss)
{
    unsigned char  has_u8_prefix;
    unsigned char  has_wide_prefix;
    unsigned char * psz;

    has_u8_prefix = current->lexeme[0] == 117 && current->lexeme[1] == 56;
    has_wide_prefix = current->lexeme[0] == 76;
    psz = (unsigned char *)(current->lexeme + 0);
    if (has_u8_prefix)
    {
        psz = (unsigned char *)(current->lexeme + 2 + 1);
    }
    else
    {
        if (has_wide_prefix)
        {
            psz = (unsigned char *)(current->lexeme + 1 + 1);
        }
        else
        {
            psz = (unsigned char *)(current->lexeme + 1);
        }
    }
    while (*psz)
    {
        if (*(psz + 1) == 0)
        {
            break;
        }
        if (*psz >= 128)
        {
            ss_fprintf(oss, "\\x%x", *psz);
        }
        else
        {
            ss_fprintf(oss, "%c", *psz);
        }
        psz++;
    }
    return 0;
}

static void il_print_string(struct token * first_token, struct token * last_token, struct osstream * oss)
{
    unsigned char  opened;
    struct token * ptk;

    opened = 0;
    ptk = first_token;
    do
    {
        if (ptk == 0)
        {
            break;
        }
        if ((ptk->flags & 1) && ptk->type == 130)
        {
            if (!opened)
            {
                if (ptk->lexeme[0] == 76)
                {
                    ss_fprintf(oss, "L");
                }
                ss_fprintf(oss, "\"");
                opened = 1;
            }
            il_visit_literal_string2(ptk, oss);
        }
        if (ptk == last_token)
        {
            break;
        }
        ptk = ptk->next;
    }
    while (ptk);
    ss_fprintf(oss, "\"");
}

static struct member_declarator *find_member_declarator_name_by_index(struct member_declaration_list * list, int member_index, char name[100], int * p_count)
{
    int no_name_index;
    struct member_declaration * p_member_declaration;

    if (list->head == 0)
    {
        return 0;
    }
    no_name_index = 0;
    p_member_declaration = list->head;
    while (p_member_declaration)
    {
        struct member_declarator * p_member_declarator;

        p_member_declarator = 0;
        if (p_member_declaration->member_declarator_list_opt)
        {
            p_member_declarator = p_member_declaration->member_declarator_list_opt->head;
            while (p_member_declarator)
            {
                if (p_member_declarator->declarator)
                {
                    if (member_index == *p_count)
                    {
                        snprintf(name, 100, "%s", p_member_declarator->declarator->name_opt->lexeme);
                        return p_member_declarator;
                    }
                }
                (*p_count)++;
                p_member_declarator = p_member_declarator->next;
            }
        }
        else
        {
            if (p_member_declaration->specifier_qualifier_list)
            {
                if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                {
                    struct struct_or_union_specifier * p_complete;

                    p_complete = get_complete_struct_or_union_specifier(p_member_declaration->specifier_qualifier_list->struct_or_union_specifier);
                    if (p_complete)
                    {
                        char mname[100];

                        _cake_zmem(&mname, 100);
                        p_member_declarator = find_member_declarator_name_by_index(&p_complete->member_declaration_list, member_index, mname, p_count);
                        if (p_member_declarator)
                        {
                            snprintf(name, 100, "__m%d.%s", no_name_index++, mname);
                            return p_member_declarator;
                        }
                    }
                }
            }
        }
        p_member_declaration = p_member_declaration->next;
    }
    return 0;
}

static int find_member_name(struct type * p_type, int index, char name[100])
{
    struct struct_or_union_specifier * p_complete;

    if (!type_is_struct_or_union(p_type))
    {
        return 1;
    }
    ;
    p_complete = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
    if (p_complete)
    {
        int count;
        struct member_declarator * p;

        count = 0;
        p = find_member_declarator_name_by_index(&p_complete->member_declaration_list, index, name, &count);
        if (p)
        {
            return 0;
        }
    }
    return 1;
}

static void object_print_non_constant_initialization(struct d_visit_ctx * ctx, struct osstream * ss, struct object * object, char * declarator_name, unsigned char  all, unsigned char  initialize_objects_that_does_not_have_initializer)
{
    if (object_is_reference(object))
    {
        object = object_get_referenced(object);
    }
    if (object->members.head != 0)
    {
        if (type_is_union(&object->type))
        {
            struct object * member;

            member = object->members.head;
            if (member->p_init_expression && object_has_constant_value(&member->p_init_expression->object) && !all)
            {
            }
            else
            {
                while (member)
                {
                    if (member->p_init_expression)
                    {
                        struct osstream  local;

                        print_identation_core(ss, ctx->indentation);
                        ss_fprintf(ss, "%s%s = ", declarator_name, member->member_designator);
                        _cake_zmem(&local, 12);
                        d_visit_expression(ctx, &local, member->p_init_expression);
                        ss_fprintf(ss, "%s", local.c_str);
                        ss_fprintf(ss, ";\n");
                        ss_close(&local);
                        break;
                    }
                    else
                    {
                        if (all)
                        {
                            if (initialize_objects_that_does_not_have_initializer)
                            {
                                print_identation_core(ss, ctx->indentation);
                                ss_fprintf(ss, "%s%s = 0;\n", declarator_name, member->member_designator);
                            }
                        }
                    }
                    member = member->next;
                }
            }
        }
        else
        {
            if (object->p_init_expression && object->p_init_expression->expression_type == 3 && type_is_array(&object->type))
            {
                struct osstream  local;
                unsigned int string_size;

                print_identation_core(ss, ctx->indentation);
                ss_fprintf(ss, "_cake_memcpy(%s%s, ", declarator_name, object->member_designator);
                _cake_zmem(&local, 12);
                d_visit_expression(ctx, &local, object->p_init_expression);
                string_size = object->p_init_expression->type.num_of_elements;
                ss_fprintf(ss, "%s, %zu", local.c_str, string_size);
                ss_fprintf(ss, ");\n");
                ss_close(&local);
                ctx->memcpy_used = 1;
            }
            else
            {
                if (object->p_init_expression)
                {
                    struct osstream  local;
                    unsigned int sz;
                    struct object * member;

                    print_identation_core(ss, ctx->indentation);
                    ss_fprintf(ss, "_cake_memcpy(&%s%s, ", declarator_name, object->member_designator);
                    _cake_zmem(&local, 12);
                    d_visit_expression(ctx, &local, object->p_init_expression);
                    sz = 0;
                    type_get_sizeof(&object->type, &sz, ctx->options.target);
                    ss_fprintf(ss, "&%s, %d", local.c_str, sz);
                    ss_fprintf(ss, ");\n");
                    ss_close(&local);
                    ctx->memcpy_used = 1;
                    member = object->members.head;
                    while (member)
                    {
                        object_print_non_constant_initialization(ctx, ss, member, declarator_name, all, 0);
                        member = member->next;
                    }
                }
                else
                {
                    struct object * member;

                    member = object->members.head;
                    while (member)
                    {
                        object_print_non_constant_initialization(ctx, ss, member, declarator_name, all, 1);
                        member = member->next;
                    }
                }
            }
        }
    }
    else
    {
        if (object->p_init_expression)
        {
            if (!all)
            {
                if (object->p_init_expression->expression_type == 3)
                {
                }
                else
                {
                    if (!object_has_constant_value(&object->p_init_expression->object))
                    {
                        struct osstream  local;

                        print_identation_core(ss, ctx->indentation);
                        ss_fprintf(ss, "%s%s = ", declarator_name, object->member_designator);
                        _cake_zmem(&local, 12);
                        d_visit_expression(ctx, &local, object->p_init_expression);
                        ss_fprintf(ss, "%s", local.c_str);
                        ss_fprintf(ss, ";\n");
                        ss_close(&local);
                    }
                }
            }
            else
            {
                struct osstream  local;

                print_identation_core(ss, ctx->indentation);
                ss_fprintf(ss, "%s%s = ", declarator_name, object->member_designator);
                _cake_zmem(&local, 12);
                d_visit_expression(ctx, &local, object->p_init_expression);
                ss_fprintf(ss, "%s", local.c_str);
                ss_fprintf(ss, ";\n");
                ss_close(&local);
            }
        }
        else
        {
            if (all)
            {
                if (initialize_objects_that_does_not_have_initializer)
                {
                    print_identation_core(ss, ctx->indentation);
                    ss_fprintf(ss, "%s%s = 0;\n", declarator_name, object->member_designator);
                }
            }
        }
    }
}

static void object_print_constant_initialization(struct d_visit_ctx * ctx, struct osstream * ss, struct object * object, unsigned char * first)
{
    if (object_is_reference(object))
    {
        object = object_get_referenced(object);
    }
    if (object->p_init_expression && object->p_init_expression->expression_type == 3)
    {
        if (!(*first))
        {
            ss_fprintf(ss, ",");
        }
        *first = 0;
        il_print_string(object->p_init_expression->first_token, object->p_init_expression->last_token, ss);
        return;
    }
    if (object->members.head != 0)
    {
        if (type_is_union(&object->type))
        {
            struct object * member;

            member = object->members.head;
            object_print_constant_initialization(ctx, ss, member, first);
        }
        else
        {
            struct object * member;

            member = object->members.head;
            while (member)
            {
                object_print_constant_initialization(ctx, ss, member, first);
                member = member->next;
            }
        }
    }
    else
    {
        if (!(*first))
        {
            ss_fprintf(ss, ",");
        }
        *first = 0;
        if (object->p_init_expression)
        {
            if (object_has_constant_value(&object->p_init_expression->object))
            {
                object_print_value(ss, &object->p_init_expression->object, ctx->options.target);
            }
            else
            {
                if (object->p_init_expression->expression_type == 3)
                {
                    il_print_string(object->p_init_expression->first_token, object->p_init_expression->last_token, ss);
                }
                else
                {
                    if (object->p_init_expression->expression_type == 2 && type_is_function(&object->p_init_expression->type))
                    {
                        d_visit_expression(ctx, ss, object->p_init_expression);
                    }
                    else
                    {
                        ss_fprintf(ss, "0 /*?*/");
                    }
                }
            }
        }
        else
        {
            ss_fprintf(ss, "0");
        }
    }
}

static char *get_op_by_expression_type(int type)
{
    /*switch*/
    {
        int __v0 = type;
        if (__v0 == 60) goto __L1; /*case 60*/
        if (__v0 == 61) goto __L2; /*case 61*/
        if (__v0 == 62) goto __L3; /*case 62*/
        if (__v0 == 63) goto __L4; /*case 63*/
        if (__v0 == 64) goto __L5; /*case 64*/
        if (__v0 == 65) goto __L6; /*case 65*/
        if (__v0 == 66) goto __L7; /*case 66*/
        if (__v0 == 67) goto __L8; /*case 67*/
        if (__v0 == 68) goto __L9; /*case 68*/
        if (__v0 == 69) goto __L10; /*case 69*/
        if (__v0 == 70) goto __L11; /*case 70*/
        goto __L12; /* default */

        {
            __L1: /*case 60*/ 
            return "=";
            __L2: /*case 61*/ 
            return "+=";
            __L3: /*case 62*/ 
            return "-=";
            __L4: /*case 63*/ 
            return "*=";
            __L5: /*case 64*/ 
            return "/=";
            __L6: /*case 65*/ 
            return "%=";
            __L7: /*case 66*/ 
            return "<<=";
            __L8: /*case 67*/ 
            return ">>=";
            __L9: /*case 68*/ 
            return "&=";
            __L10: /*case 69*/ 
            return "|=";
            __L11: /*case 70*/ 
            return "^=";
            __L12: /* default */ 
            goto __L0; /* break */

        }
        __L0:;
    }
    ;
    return "";
}

static void d_visit_expression(struct d_visit_ctx * ctx, struct osstream * oss, struct expression * p_expression)
{
    if (!ctx->address_of_argument && object_has_constant_value(&p_expression->object))
    {
        if (type_is_void_ptr(&p_expression->type) || type_is_nullptr_t(&p_expression->type))
        {
            if (object_is_zero(&p_expression->object))
            {
                if (ctx->define_nullptr)
                {
                    ss_fprintf(oss, "NULL");
                    ctx->null_pointer_constant_used = 1;
                }
                else
                {
                    ss_fprintf(oss, "0");
                }
                return;
            }
        }
        else
        {
            if (type_is_arithmetic(&p_expression->type))
            {
                object_print_value(oss, &p_expression->object, ctx->options.target);
                return;
            }
        }
    }
    ctx->address_of_argument = 0;
    /*switch*/
    {
        int __v0 = p_expression->expression_type;
        if (__v0 == 0) goto __L6; /*case 0*/
        if (__v0 == 4) goto __L7; /*case 4*/
        if (__v0 == 2) goto __L10; /*case 2*/
        if (__v0 == 3) goto __L20; /*case 3*/
        if (__v0 == 1) goto __L21; /*case 1*/
        if (__v0 == 5) goto __L22; /*case 5*/
        if (__v0 == 8) goto __L23; /*case 8*/
        if (__v0 == 6) goto __L24; /*case 6*/
        if (__v0 == 9) goto __L25; /*case 9*/
        if (__v0 == 7) goto __L27; /*case 7*/
        if (__v0 == 25) goto __L29; /*case 25*/
        if (__v0 == 21) goto __L31; /*case 21*/
        if (__v0 == 22) goto __L33; /*case 22*/
        if (__v0 == 23) goto __L34; /*case 23*/
        if (__v0 == 24) goto __L35; /*case 24*/
        if (__v0 == 14) goto __L37; /*case 14*/
        if (__v0 == 15) goto __L39; /*case 15*/
        if (__v0 == 16) goto __L41; /*case 16*/
        if (__v0 == 17) goto __L42; /*case 17*/
        if (__v0 == 13) goto __L43; /*case 13*/
        if (__v0 == 12) goto __L44; /*case 12*/
        if (__v0 == 40) goto __L48; /*case 40*/
        if (__v0 == 10) goto __L49; /*case 10*/
        if (__v0 == 11) goto __L51; /*case 11*/
        if (__v0 == 18) goto __L53; /*case 18*/
        if (__v0 == 19) goto __L54; /*case 19*/
        if (__v0 == 30) goto __L55; /*case 30*/
        if (__v0 == 29) goto __L56; /*case 29*/
        if (__v0 == 20) goto __L57; /*case 20*/
        if (__v0 == 32) goto __L58; /*case 32*/
        if (__v0 == 33) goto __L59; /*case 33*/
        if (__v0 == 34) goto __L60; /*case 34*/
        if (__v0 == 35) goto __L61; /*case 35*/
        if (__v0 == 36) goto __L62; /*case 36*/
        if (__v0 == 37) goto __L63; /*case 37*/
        if (__v0 == 38) goto __L64; /*case 38*/
        if (__v0 == 39) goto __L65; /*case 39*/
        if (__v0 == 31) goto __L66; /*case 31*/
        if (__v0 == 46) goto __L67; /*case 46*/
        if (__v0 == 45) goto __L68; /*case 45*/
        if (__v0 == 42) goto __L69; /*case 42*/
        if (__v0 == 43) goto __L70; /*case 43*/
        if (__v0 == 44) goto __L71; /*case 44*/
        if (__v0 == 71) goto __L72; /*case 71*/
        if (__v0 == 60) goto __L73; /*case 60*/
        if (__v0 == 61) goto __L75; /*case 61*/
        if (__v0 == 62) goto __L76; /*case 62*/
        if (__v0 == 63) goto __L77; /*case 63*/
        if (__v0 == 64) goto __L78; /*case 64*/
        if (__v0 == 65) goto __L79; /*case 65*/
        if (__v0 == 66) goto __L80; /*case 66*/
        if (__v0 == 67) goto __L81; /*case 67*/
        if (__v0 == 68) goto __L82; /*case 68*/
        if (__v0 == 69) goto __L83; /*case 69*/
        if (__v0 == 70) goto __L84; /*case 70*/
        if (__v0 == 41) goto __L85; /*case 41*/
        if (__v0 == 47) goto __L86; /*case 47*/
        if (__v0 == 48) goto __L87; /*case 48*/
        if (__v0 == 49) goto __L88; /*case 49*/
        if (__v0 == 50) goto __L89; /*case 50*/
        if (__v0 == 53) goto __L90; /*case 53*/
        if (__v0 == 54) goto __L91; /*case 54*/
        if (__v0 == 55) goto __L92; /*case 55*/
        if (__v0 == 56) goto __L93; /*case 56*/
        if (__v0 == 57) goto __L94; /*case 57*/
        if (__v0 == 52) goto __L95; /*case 52*/
        if (__v0 == 51) goto __L96; /*case 51*/
        if (__v0 == 59) goto __L97; /*case 59*/
        if (__v0 == 58) goto __L98; /*case 58*/
        if (__v0 == 26) goto __L99; /*case 26*/
        if (__v0 == 27) goto __L100; /*case 27*/
        if (__v0 == 28) goto __L101; /*case 28*/
        if (__v0 == 72) goto __L102; /*case 72*/
        goto __L5;

        {
            __L6: /*case 0*/ 
            ;
            goto __L5; /* break */

            __L7: /*case 4*/ 
            {
                char func_name[200];
                char name[100];

                ;
                _cake_zmem(&func_name, 200);
                _cake_zmem(&name, 100);
                if (ctx->p_current_function_opt->name_opt)
                {
                    snprintf(func_name, 200, "%s", ctx->p_current_function_opt->name_opt->lexeme);
                    snprintf(name, 100, "__cake_func_%s", func_name);
                }
                else
                {
                    snprintf(func_name, 200, "unnamed");
                    snprintf(name, 100, "__cake_func_%p", ctx->p_current_function_opt);
                }
                if (!ctx->is__func__predefined_identifier_added)
                {
                    ;
                    ctx->is__func__predefined_identifier_added = 1;
                    ss_fprintf(&ctx->add_this_before_external_decl, "static const char %s[] = \"%s\";\n", name, func_name);
                }
                ss_fprintf(oss, "%s", name);
            }
            goto __L5; /* break */

            __L10: /*case 2*/ 
            {
                char * declarator_name;
                unsigned char  is_function;
                unsigned char  is_local_function_definition;
                unsigned char  is_static;
                unsigned char  is_auto;
                unsigned char  is_inline;
                unsigned char  is_extern;
                unsigned char  is_local;

                ;
                ;
                declarator_name = "";
                if (p_expression->declarator->name_opt)
                {
                    declarator_name = p_expression->declarator->name_opt->lexeme;
                }
                is_function = type_is_function(&p_expression->declarator->type);
                is_local_function_definition = 0;
                if (is_function)
                {
                    is_local_function_definition = p_expression->type.storage_class_specifier_flags & 4096 && p_expression->declarator->function_body != 0;
                }
                is_static = ((p_expression->declarator->declaration_specifiers->storage_class_specifier_flags & 4) != 0);
                is_auto = ((p_expression->declarator->declaration_specifiers->storage_class_specifier_flags & 16) != 0);
                is_inline = ((p_expression->declarator->declaration_specifiers->function_specifier_flags & 1) != 0);
                is_extern = ((p_expression->type.storage_class_specifier_flags & 2) != 0);
                is_local = (!is_static && !is_extern) && p_expression->type.storage_class_specifier_flags & 4096;
                if (is_function)
                {
                    struct map_entry * p;

                    ss_fprintf(oss, "%s", p_expression->declarator->name_opt->lexeme);
                    p = hashmap_find(&ctx->file_scope_declarator_map, declarator_name);
                    if (p == 0)
                    {
                        struct hash_item_set  i;
                        struct osstream  ss;
                        struct declarator * p_function_defined;

                        _cake_zmem(&i, 32);
                        i.number = 1;
                        hashmap_set(&ctx->file_scope_declarator_map, declarator_name, &i);
                        hash_item_set_destroy(&i);
                        _cake_zmem(&ss, 12);
                        if ((is_inline || is_local_function_definition || is_auto) && !is_static)
                        {
                            ss_fprintf(&ss, "static ");
                        }
                        d_print_type(ctx, &ss, &p_expression->type, declarator_name, 1);
                        ss_fprintf(&ctx->add_this_before_external_decl, "%s", ss.c_str);
                        ss_fprintf(&ctx->add_this_before_external_decl, ";\n");
                        ;
                        p_function_defined = declarator_get_function_definition(p_expression->declarator);
                        if (p_function_defined && (is_static || is_inline || is_auto || is_local_function_definition))
                        {
                            struct osstream  local3;
                            struct osstream  local4;

                            _cake_zmem(&local3, 12);
                            _cake_zmem(&local4, 12);
                            d_print_type(ctx, &local4, &p_function_defined->type, declarator_name, 0);
                            ss_fprintf(&local3, "static %s\n", local4.c_str);
                            d_visit_function_body(ctx, &local3, p_function_defined);
                            ;
                            ;
                            ss_fprintf(&ctx->add_this_after_external_decl, "\n");
                            ss_fprintf(&ctx->add_this_after_external_decl, "%s", local3.c_str);
                            ss_close(&local3);
                            ss_close(&local4);
                        }
                        ss_close(&ss);
                    }
                }
                else
                {
                    if (is_local)
                    {
                        ss_fprintf(oss, "%s", p_expression->declarator->name_opt->lexeme);
                    }
                    else
                    {
                        void * p;

                        p = hashmap_find(&ctx->file_scope_declarator_map, declarator_name);
                        if (p == 0)
                        {
                            struct hash_item_set  i;
                            struct osstream  ss;

                            _cake_zmem(&i, 32);
                            i.number = 1;
                            hashmap_set(&ctx->file_scope_declarator_map, declarator_name, &i);
                            ;
                            ;
                            _cake_zmem(&ss, 12);
                            d_print_type(ctx, &ss, &p_expression->type, declarator_name, 1);
                            if (p_expression->p_init_declarator && p_expression->p_init_declarator->initializer)
                            {
                                print_initializer(ctx, &ss, p_expression->p_init_declarator, 1);
                                ;
                                ss_fprintf(&ctx->add_this_before_external_decl, "%s\n", ss.c_str);
                            }
                            else
                            {
                                ;
                                ss_fprintf(&ctx->add_this_before_external_decl, "%s;\n", ss.c_str);
                            }
                            ss_close(&ss);
                            hash_item_set_destroy(&i);
                        }
                        ss_fprintf(oss, "%s", p_expression->declarator->name_opt->lexeme);
                    }
                }
            }
            goto __L5; /* break */

            __L20: /*case 3*/ 
            il_print_string(p_expression->first_token, p_expression->last_token, oss);
            goto __L5; /* break */

            __L21: /*case 1*/ 
            __L22: /*case 5*/ 
            __L23: /*case 8*/ 
            __L24: /*case 6*/ 
            object_print_value(oss, &p_expression->object, ctx->options.target);
            goto __L5; /* break */

            __L25: /*case 9*/ 
            ;
            if (p_expression->right->expression_type == 9)
            {
                d_visit_expression(ctx, oss, p_expression->right);
            }
            else
            {
                ss_fprintf(oss, "(");
                d_visit_expression(ctx, oss, p_expression->right);
                ss_fprintf(oss, ")");
            }
            goto __L5; /* break */

            __L27: /*case 7*/ 
            ;
            if (p_expression->generic_selection->p_view_selected_expression)
            {
                d_visit_expression(ctx, oss, p_expression->generic_selection->p_view_selected_expression);
            }
            goto __L5; /* break */

            __L29: /*case 25*/ 
            ss_fprintf(oss, "__builtin_offsetof(");
            if (p_expression->type_name)
            {
                d_print_type(ctx, oss, &p_expression->type_name->type, 0, 0);
            }
            ss_fprintf(oss, ", ");
            ss_fprintf(oss, "%s", p_expression->offsetof_member_designator->lexeme);
            ss_fprintf(oss, ")");
            goto __L5; /* break */

            __L31: /*case 21*/ 
            ;
            if (p_expression->right != 0)
            {
                ss_fprintf(oss, "__builtin_va_start(");
                d_visit_expression(ctx, oss, p_expression->left);
                ss_fprintf(oss, ", ");
                d_visit_expression(ctx, oss, p_expression->right);
                ss_fprintf(oss, ")");
            }
            else
            {
                ss_fprintf(oss, "__builtin_c23_va_start(");
                d_visit_expression(ctx, oss, p_expression->left);
                ss_fprintf(oss, ")");
            }
            goto __L5; /* break */

            __L33: /*case 22*/ 
            ;
            ss_fprintf(oss, "__builtin_va_end(");
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, ")");
            goto __L5; /* break */

            __L34: /*case 23*/ 
            ;
            ;
            ss_fprintf(oss, "__builtin_va_copy(");
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, ", ");
            d_visit_expression(ctx, oss, p_expression->right);
            ss_fprintf(oss, ")");
            goto __L5; /* break */

            __L35: /*case 24*/ 
            ;
            ss_fprintf(oss, "__builtin_va_arg(");
            d_visit_expression(ctx, oss, p_expression->left);
            if (p_expression->type_name)
            {
                ss_fprintf(oss, ", ");
                d_print_type(ctx, oss, &p_expression->type_name->type, 0, 0);
            }
            ss_fprintf(oss, ")");
            goto __L5; /* break */

            __L37: /*case 14*/ 
            {
                char name[100];
                int r;

                ;
                d_visit_expression(ctx, oss, p_expression->left);
                _cake_zmem(&name, 100);
                r = find_member_name(&p_expression->left->type, p_expression->member_index, name);
                if (r == 0)
                {
                    ss_fprintf(oss, ".%s", name);
                }
            }
            goto __L5; /* break */

            __L39: /*case 15*/ 
            {
                ;
                d_visit_expression(ctx, oss, p_expression->left);
                {
                    struct type  t;
                    char name[100];
                    int r;

                    t = type_remove_pointer(&p_expression->left->type);
                    _cake_zmem(&name, 100);
                    r = find_member_name(&t, p_expression->member_index, name);
                    if (r == 0)
                    {
                        ss_fprintf(oss, "->%s", name);
                    }
                    type_destroy(&t);
                }
            }
            goto __L5; /* break */

            __L41: /*case 16*/ 
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, "++");
            goto __L5; /* break */

            __L42: /*case 17*/ 
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, "--");
            goto __L5; /* break */

            __L43: /*case 13*/ 
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, "[");
            d_visit_expression(ctx, oss, p_expression->right);
            ss_fprintf(oss, "]");
            goto __L5; /* break */

            __L44: /*case 12*/ 
            {
                struct param * param;
                struct argument_expression * arg;

                ;
                d_visit_expression(ctx, oss, p_expression->left);
                param = p_expression->left->type.params.head;
                ss_fprintf(oss, "(");
                arg = p_expression->argument_expression_list.head;
                while (arg)
                {
                    unsigned char  to_bool;

                    to_bool = param && type_is_bool(&param->type) && !(type_is_bool(&arg->expression->type) || type_is_essential_bool(&arg->expression->type));
                    if (to_bool)
                    {
                        expression_to_bool_value(ctx, oss, arg->expression);
                    }
                    else
                    {
                        d_visit_expression(ctx, oss, arg->expression);
                    }
                    if (param)
                    {
                        param = param->next;
                    }
                    if (arg->next)
                    {
                        ss_fprintf(oss, ", ");
                    }
                    arg = arg->next;
                }
                ss_fprintf(oss, ")");
            }
            goto __L5; /* break */

            __L48: /*case 40*/ 
            {
                ;
                ss_fprintf(oss, "&");
                ctx->address_of_argument = 1;
                d_visit_expression(ctx, oss, p_expression->right);
                ctx->address_of_argument = 0;
            }
            goto __L5; /* break */

            __L49: /*case 10*/ 
            {
                char generated_function_literal_name[100];
                struct osstream  function_literal_nameless;
                struct declarator * p_current_function_opt;
                int current_indentation;
                struct osstream  function_literal_body;
                struct osstream  function_literal;
                struct map_entry * l;

                ;
                _cake_zmem(&generated_function_literal_name, 100);
                print_identation_core(&ctx->add_this_before, ctx->indentation);
                _cake_zmem(&function_literal_nameless, 12);
                d_print_type(ctx, &function_literal_nameless, &p_expression->type, 0, 0);
                ;
                p_current_function_opt = ctx->p_current_function_opt;
                ctx->p_current_function_opt = p_expression->type_name->abstract_declarator;
                current_indentation = ctx->indentation;
                ctx->indentation = 0;
                _cake_zmem(&function_literal_body, 12);
                d_visit_compound_statement(ctx, &function_literal_body, p_expression->compound_statement);
                ctx->p_current_function_opt = p_current_function_opt;
                ctx->indentation = current_indentation;
                _cake_zmem(&function_literal, 12);
                ss_fprintf(&function_literal, "%s%s", function_literal_nameless.c_str, function_literal_body.c_str);
                ;
                l = hashmap_find(&ctx->instantiated_function_literals, function_literal.c_str);
                if (l != 0)
                {
                    snprintf(generated_function_literal_name, 100, "__c%zu_f", l->data.number);
                }
                else
                {
                    unsigned int current_cake_declarator_number;
                    struct hash_item_set  i;
                    struct osstream  lambda_sig;

                    current_cake_declarator_number = ctx->cake_file_scope_declarator_number++;
                    _cake_zmem(&i, 32);
                    i.number = current_cake_declarator_number;
                    hashmap_set(&ctx->instantiated_function_literals, function_literal.c_str, &i);
                    hash_item_set_destroy(&i);
                    snprintf(generated_function_literal_name, 100, "__c%d_f", current_cake_declarator_number);
                    _cake_zmem(&lambda_sig, 12);
                    d_print_type(ctx, &lambda_sig, &p_expression->type, generated_function_literal_name, 0);
                    ss_fprintf(&ctx->add_this_before_external_decl, "static %s\n%s", lambda_sig.c_str, function_literal_body.c_str);
                    ss_close(&lambda_sig);
                }
                ss_fprintf(oss, "%s", generated_function_literal_name);
                ss_close(&function_literal_nameless);
                ss_close(&function_literal);
            }
            goto __L5; /* break */

            __L51: /*case 11*/ 
            {
                char name[100];

                _cake_zmem(&name, 100);
                snprintf(name, 100, "__v%d", ctx->cake_local_declarator_number++);
                if (ctx->is_local)
                {
                    struct osstream  local;

                    _cake_zmem(&local, 12);
                    ss_swap(&ctx->block_scope_declarators, &local);
                    print_identation_core(&local, ctx->indentation);
                    d_print_type(ctx, &local, &p_expression->type, name, 0);
                    ss_fprintf(&local, ";\n", name);
                    ss_fprintf(&ctx->block_scope_declarators, "%s", local.c_str);
                    ss_clear(&local);
                    object_print_non_constant_initialization(ctx, &local, &p_expression->object, name, 1, 1);
                    ;
                    ss_fprintf(&ctx->add_this_before, "%s", local.c_str);
                    ss_close(&local);
                    ss_fprintf(oss, "%s", name);
                }
                else
                {
                    struct osstream  local;
                    unsigned char  first;

                    _cake_zmem(&local, 12);
                    print_identation_core(&local, ctx->indentation);
                    d_print_type(ctx, &local, &p_expression->type, name, 0);
                    first = 1;
                    ss_fprintf(&local, " = {");
                    object_print_constant_initialization(ctx, &local, &p_expression->object, &first);
                    ss_fprintf(&local, "};\n");
                    ss_fprintf(&ctx->add_this_before, "%s", local.c_str);
                    ss_close(&local);
                    ss_fprintf(oss, "%s", name);
                }
            }
            goto __L5; /* break */

            __L53: /*case 18*/ 
            object_print_value(oss, &p_expression->object, ctx->options.target);
            goto __L5; /* break */

            __L54: /*case 19*/ 
            object_print_value(oss, &p_expression->object, ctx->options.target);
            goto __L5; /* break */

            __L55: /*case 30*/ 
            __L56: /*case 29*/ 
            __L57: /*case 20*/ 
            object_print_value(oss, &p_expression->object, ctx->options.target);
            goto __L5; /* break */

            __L58: /*case 32*/ 
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L59: /*case 33*/ 
            ;
            ss_fprintf(oss, "++");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L60: /*case 34*/ 
            ;
            ss_fprintf(oss, "--");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L61: /*case 35*/ 
            ;
            ss_fprintf(oss, "!");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L62: /*case 36*/ 
            ;
            ss_fprintf(oss, "~");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L63: /*case 37*/ 
            ;
            ss_fprintf(oss, "-");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L64: /*case 38*/ 
            ;
            ss_fprintf(oss, "+");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L65: /*case 39*/ 
            ;
            ss_fprintf(oss, "*");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L66: /*case 31*/ 
            goto __L5; /* break */

            __L67: /*case 46*/ 
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " - ");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L68: /*case 45*/ 
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " + ");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L69: /*case 42*/ 
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " * ");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L70: /*case 43*/ 
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " / ");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L71: /*case 44*/ 
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, "%s", " % ");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L72: /*case 71*/ 
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, ", ");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L73: /*case 60*/ 
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " %s ", get_op_by_expression_type(p_expression->expression_type));
            if (type_is_bool(&p_expression->left->type))
            {
                expression_to_bool_value(ctx, oss, p_expression->right);
            }
            else
            {
                d_visit_expression(ctx, oss, p_expression->right);
            }
            goto __L5; /* break */

            __L75: /*case 61*/ 
            __L76: /*case 62*/ 
            __L77: /*case 63*/ 
            __L78: /*case 64*/ 
            __L79: /*case 65*/ 
            __L80: /*case 66*/ 
            __L81: /*case 67*/ 
            __L82: /*case 68*/ 
            __L83: /*case 69*/ 
            __L84: /*case 70*/ 
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " %s ", get_op_by_expression_type(p_expression->expression_type));
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L85: /*case 41*/ 
            {
                struct osstream  local2;

                ;
                _cake_zmem(&local2, 12);
                d_print_type(ctx, &local2, &p_expression->type, 0, 0);
                ss_fprintf(oss, "(%s)", local2.c_str);
                ss_close(&local2);
                d_visit_expression(ctx, oss, p_expression->left);
            }
            goto __L5; /* break */

            __L86: /*case 47*/ 
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " >> ");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L87: /*case 48*/ 
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " << ");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L88: /*case 49*/ 
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " > ");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L89: /*case 50*/ 
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " < ");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L90: /*case 53*/ 
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " == ");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L91: /*case 54*/ 
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " != ");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L92: /*case 55*/ 
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " & ");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L93: /*case 56*/ 
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " ^ ");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L94: /*case 57*/ 
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " | ");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L95: /*case 52*/ 
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " <= ");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L96: /*case 51*/ 
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " >= ");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L97: /*case 59*/ 
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " && ");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L98: /*case 58*/ 
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " || ");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

            __L99: /*case 26*/ 
            __L100: /*case 27*/ 
            __L101: /*case 28*/ 
            goto __L5; /* break */

            __L102: /*case 72*/ 
            ;
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->condition_expr);
            ss_fprintf(oss, " ? ");
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " : ");
            d_visit_expression(ctx, oss, p_expression->right);
            goto __L5; /* break */

        }
        __L5:;
    }
}

static unsigned char is_all_zero(struct object * object)
{
    if (object_is_reference(object))
    {
        object = object_get_referenced(object);
    }
    if (object->members.head != 0)
    {
        struct object * member;

        member = object->members.head;
        while (member)
        {
            if (!is_all_zero(member))
            {
                return 0;
            }
            member = member->next;
        }
    }
    if (object->p_init_expression)
    {
        if (object_has_constant_value(&object->p_init_expression->object))
        {
            if (object_is_true(&object->p_init_expression->object) != 0)
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    return 1;
}

static void print_initializer(struct d_visit_ctx * ctx, struct osstream * oss, struct init_declarator * p_init_declarator, unsigned char  bstatic)
{
    ;
    if (1) /*try*/
    {
        unsigned char  is_local;

        is_local = ctx->is_local;
        if (p_init_declarator->initializer->assignment_expression)
        {
            if (is_local && !bstatic)
            {
                if (p_init_declarator->initializer->assignment_expression->expression_type == 3 && type_is_array(&p_init_declarator->p_declarator->type))
                {
                    struct osstream  local;

                    print_identation_core(oss, ctx->indentation);
                    ss_fprintf(oss, "_cake_memcpy(%s, ", p_init_declarator->p_declarator->name_opt->lexeme);
                    _cake_zmem(&local, 12);
                    d_visit_expression(ctx, &local, p_init_declarator->initializer->assignment_expression);
                    ss_fprintf(oss, "%s, %d", local.c_str, p_init_declarator->p_declarator->type.num_of_elements);
                    ss_fprintf(oss, ");\n");
                    ss_close(&local);
                    ctx->memcpy_used = 1;
                }
                else
                {
                    print_identation_core(oss, ctx->indentation);
                    ss_fprintf(oss, "%s%s = ", p_init_declarator->p_declarator->name_opt->lexeme, "");
                    if (type_is_bool(&p_init_declarator->p_declarator->type))
                    {
                        expression_to_bool_value(ctx, oss, p_init_declarator->initializer->assignment_expression);
                    }
                    else
                    {
                        d_visit_expression(ctx, oss, p_init_declarator->initializer->assignment_expression);
                    }
                    ss_fprintf(oss, ";\n");
                }
            }
            else
            {
                ss_fprintf(oss, " = ");
                d_visit_expression(ctx, oss, p_init_declarator->initializer->assignment_expression);
                ss_fprintf(oss, ";\n");
            }
        }
        else
        {
            if (p_init_declarator->initializer->braced_initializer)
            {
                if (p_init_declarator->initializer->braced_initializer->initializer_list && p_init_declarator->initializer->braced_initializer->initializer_list->head)
                {
                    if (is_all_zero(&p_init_declarator->p_declarator->object))
                    {
                        if (is_local && !bstatic)
                        {
                            unsigned int sz;

                            sz = 0;
                            if (type_get_sizeof(&p_init_declarator->p_declarator->type, &sz, ctx->options.target) != 0)
                            {
                                goto __L0; /* throw */
                            }
                            print_identation_core(oss, ctx->indentation);
                            ss_fprintf(oss, "_cake_zmem(&%s, %zu);\n", p_init_declarator->p_declarator->name_opt->lexeme, sz);
                            ctx->zero_mem_used = 1;
                        }
                        else
                        {
                            ss_fprintf(oss, " = ");
                            ss_fprintf(oss, "{0};\n");
                        }
                    }
                    else
                    {
                        unsigned char  first;

                        first = 1;
                        if (!is_local || bstatic)
                        {
                            ss_fprintf(oss, " = ");
                            ss_fprintf(oss, "{");
                            object_print_constant_initialization(ctx, oss, &p_init_declarator->p_declarator->object, &first);
                            ss_fprintf(oss, "}");
                            ss_fprintf(oss, ";\n");
                        }
                        else
                        {
                            object_print_non_constant_initialization(ctx, oss, &p_init_declarator->p_declarator->object, p_init_declarator->p_declarator->name_opt->lexeme, 1, 1);
                        }
                    }
                }
                else
                {
                    if (is_local && !bstatic)
                    {
                        unsigned int sz;

                        sz = 0;
                        if (type_get_sizeof(&p_init_declarator->p_declarator->type, &sz, ctx->options.target) != 0)
                        {
                            goto __L0; /* throw */
                        }
                        print_identation_core(oss, ctx->indentation);
                        ss_fprintf(oss, "_cake_zmem(&%s, %zu);\n", p_init_declarator->p_declarator->name_opt->lexeme, sz);
                        ctx->zero_mem_used = 1;
                    }
                    else
                    {
                        ss_fprintf(oss, " = ");
                        ss_fprintf(oss, "{0};\n");
                    }
                }
            }
        }
    }
    else __L0: /*catch*/ 
    {
    }
}

static void d_visit_init_declarator(struct d_visit_ctx * ctx, struct osstream * oss0, struct init_declarator * p_init_declarator, int function_specifier_flags, int storage_class_specifier_flags)
{
    unsigned char  is_function;
    unsigned char  is_inline;
    unsigned char  is_block_scope;
    unsigned char  is_typedef;
    unsigned char  is_extern;
    unsigned char  is_function_body;
    unsigned char  is_static;
    unsigned char  is_auto;

    is_function = type_is_function(&p_init_declarator->p_declarator->type);
    is_inline = ((function_specifier_flags & 1) != 0);
    is_block_scope = ((storage_class_specifier_flags & 4096) != 0);
    is_typedef = ((storage_class_specifier_flags & 1) != 0);
    is_extern = ((storage_class_specifier_flags & 2) != 0);
    is_function_body = p_init_declarator->p_declarator->function_body != 0;
    if (is_typedef)
    {
        return;
    }
    is_static = ((storage_class_specifier_flags & 4) != 0);
    is_auto = ((storage_class_specifier_flags & 16) != 0);
    if (!is_extern && !is_block_scope && !is_inline && !is_static && !is_function && !is_function_body)
    {
        struct hash_item_set  i;
        struct osstream  ss;

        _cake_zmem(&i, 32);
        i.number = 1;
        hashmap_set(&ctx->file_scope_declarator_map, p_init_declarator->p_declarator->name_opt->lexeme, &i);
        _cake_zmem(&ss, 12);
        d_print_type(ctx, &ss, &p_init_declarator->p_declarator->type, p_init_declarator->p_declarator->name_opt->lexeme, 1);
        ss_fprintf(oss0, "%s", ss.c_str);
        if (p_init_declarator->initializer)
        {
            print_initializer(ctx, oss0, p_init_declarator, 1);
        }
        else
        {
            ss_fprintf(oss0, ";\n");
        }
        ss_close(&ss);
        hash_item_set_destroy(&i);
        return;
    }
    else
    {
        if (!is_extern && is_block_scope && !is_inline && !is_static && !is_function && !is_function_body)
        {
            struct osstream  ss;

            _cake_zmem(&ss, 12);
            d_print_type(ctx, &ss, &p_init_declarator->p_declarator->type, p_init_declarator->p_declarator->name_opt->lexeme, 0);
            print_identation(ctx, &ctx->block_scope_declarators);
            ss_fprintf(&ctx->block_scope_declarators, "%s;\n", ss.c_str);
            ss_close(&ss);
            if (p_init_declarator->initializer)
            {
                print_initializer(ctx, oss0, p_init_declarator, 0);
            }
            return;
        }
        else
        {
            if (!is_block_scope && !is_inline && !is_static && is_function && is_function_body)
            {
                struct osstream  ss;
                struct hash_item_set  i;

                _cake_zmem(&ss, 12);
                d_print_type(ctx, &ss, &p_init_declarator->p_declarator->type, p_init_declarator->p_declarator->name_opt->lexeme, 1);
                _cake_zmem(&i, 32);
                i.number = 1;
                hashmap_set(&ctx->file_scope_declarator_map, p_init_declarator->p_declarator->name_opt->lexeme, &i);
                ss_fprintf(&ss, "\n");
                d_visit_function_body(ctx, &ss, p_init_declarator->p_declarator);
                ss_fprintf(&ss, "\n");
                ss_fprintf(oss0, "%s\n", ss.c_str);
                ss_close(&ss);
                hash_item_set_destroy(&i);
                return;
            }
            else
            {
                unsigned char  rename_declarator;

                rename_declarator = 0;
                if (!is_extern && is_block_scope && !is_inline && is_static && !is_function)
                {
                    rename_declarator = 1;
                }
                else
                {
                    if (!is_inline && !is_static && !is_auto && is_function && !is_function_body)
                    {
                    }
                    else
                    {
                        if (!is_extern && is_block_scope && is_function)
                        {
                            rename_declarator = 1;
                        }
                    }
                }
                if (rename_declarator)
                {
                    if (!p_init_declarator->p_declarator->declarator_renamed)
                    {
                        char name[100];

                        p_init_declarator->p_declarator->declarator_renamed = 1;
                        _cake_zmem(&name, 100);
                        snprintf(name, 100, "__c%d_%s", ctx->cake_file_scope_declarator_number++, p_init_declarator->p_declarator->name_opt->lexeme);
                        free(p_init_declarator->p_declarator->name_opt->lexeme);
                        p_init_declarator->p_declarator->name_opt->lexeme = strdup(name);
                    }
                }
                return;
            }
        }
    }
}

static void d_visit_init_declarator_list(struct d_visit_ctx * ctx, struct osstream * oss, struct init_declarator_list * p_init_declarator_list, int function_specifier_flags, int storage_class_specifier_flags)
{
    struct init_declarator * p_init_declarator;

    p_init_declarator = p_init_declarator_list->head;
    while (p_init_declarator)
    {
        d_visit_init_declarator(ctx, oss, p_init_declarator, function_specifier_flags, storage_class_specifier_flags);
        p_init_declarator = p_init_declarator->next;
    }
}

static void d_visit_declaration(struct d_visit_ctx * ctx, struct osstream * oss, struct declaration * p_declaration)
{
    if (p_declaration->pragma_declaration)
    {
    }
    if (p_declaration->init_declarator_list.head)
    {
        int storage_class_specifier_flags;
        int function_specifier_flags;

        ;
        storage_class_specifier_flags = p_declaration->declaration_specifiers->storage_class_specifier_flags;
        function_specifier_flags = p_declaration->declaration_specifiers->function_specifier_flags;
        d_visit_init_declarator_list(ctx, oss, &p_declaration->init_declarator_list, function_specifier_flags, storage_class_specifier_flags);
    }
}
