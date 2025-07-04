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

struct type_name {
    struct token * first_token;
    struct token * last_token;
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct declarator * abstract_declarator;
    struct type  type;
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

struct condition {
    struct expression * expression;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator * p_init_declarator;
    struct token * first_token;
    struct token * last_token;
};

struct attribute_token {
    int   attributes_flags;
    struct token * token;
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

struct parameter_declaration {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct declaration_specifiers * declaration_specifiers;
    struct declarator * declarator;
    struct parameter_declaration * next;
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

struct attribute_list {
    int   attributes_flags;
    struct attribute * head;
    struct attribute * tail;
};

struct generic_assoc_list {
    struct generic_association * head;
    struct generic_association * tail;
};

struct token_list {
    struct token * head;
    struct token * tail;
};

struct member_declaration_list {
    struct token * first_token;
    struct token * last_token;
    struct member_declaration * head;
    struct member_declaration * tail;
};

struct map_entry {
    struct map_entry * next;
    unsigned int hash;
    char * key;
    int   type;
    union _struct_tag_5  data;
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
    struct type  type;
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

struct typeof_specifier {
    struct token * first_token;
    struct token * last_token;
    struct typeof_specifier_argument * typeof_specifier_argument;
    struct type  type;
};

struct generic_selection {
    struct expression * expression;
    struct type_name * type_name;
    struct expression * p_view_selected_expression;
    struct generic_assoc_list  generic_assoc_list;
    struct token * first_token;
    struct token * last_token;
};

struct unlabeled_statement {
    struct expression_statement * expression_statement;
    struct primary_block * primary_block;
    struct jump_statement * jump_statement;
};

struct initializer_list_item {
    struct designation * designation;
    struct initializer * initializer;
    struct initializer_list_item * next;
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
    int label_id;
    struct defer_list  defer_list;
};

struct member_declarator {
    struct declarator * declarator;
    struct expression * constant_expression;
    struct member_declarator * next;
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

struct object {
    int   state;
    int   value_type;
    struct type  type;
    char * debug_name;
    union _struct_tag_16  value;
    struct object * parent;
    struct expression * p_init_expression;
    struct object * members;
    struct object * next;
};

struct member_declarator_list {
    struct member_declarator * head;
    struct member_declarator * tail;
};

struct pragma_declaration {
    struct token * first_token;
    struct token * last_token;
};

struct designator {
    struct expression * constant_expression_opt;
    struct token * token;
    struct designator * next;
};

struct parameter_type_list {
    unsigned char   is_var_args;
    unsigned char   is_void;
    struct parameter_list * parameter_list;
};

struct initializer {
    struct token * first_token;
    struct braced_initializer * braced_initializer;
    struct expression * assignment_expression;
};

struct atomic_type_specifier {
    struct token * token;
    struct type_name * type_name;
};

struct balanced_token {
    struct token * token;
    struct balanced_token * next;
};

struct argument_expression_list {
    struct argument_expression * head;
    struct argument_expression * tail;
};

struct break_reference {
    struct selection_statement * p_selection_statement;
    struct iteration_statement * p_iteration_statement;
};

struct statement {
    struct labeled_statement * labeled_statement;
    struct unlabeled_statement * unlabeled_statement;
};

struct parameter_list {
    struct parameter_declaration * head;
    struct parameter_declaration * tail;
};

struct labeled_statement {
    struct label * label;
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

struct generic_association {
    struct type  type;
    struct type_name * p_type_name;
    struct expression * expression;
    struct token * first_token;
    struct token * last_token;
    struct generic_association * next;
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

struct designation {
    struct designator_list * designator_list;
    struct token * token;
};

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

struct attribute_specifier {
    struct token * first_token;
    struct token * last_token;
    struct attribute_list * attribute_list;
    struct attribute_specifier * next;
};

struct pointer {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * calling_convention;
    struct pointer * pointer;
};

struct alignment_specifier {
    struct type_name * type_name;
    struct expression * constant_expression;
    struct token * token;
};

struct expression_statement {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct expression * expression_opt;
};

struct declaration_list {
    struct declaration * head;
    struct declaration * tail;
};

struct osstream {
    char * c_str;
    int size;
    int capacity;
};

struct label {
    struct expression * constant_expression;
    struct expression * constant_expression_end;
    struct token * p_identifier_opt;
    struct token * p_first_token;
    struct label * next;
    int label_id;
};

struct block_item {
    struct token * first_token;
    struct declaration * declaration;
    struct unlabeled_statement * unlabeled_statement;
    struct label * label;
    struct block_item * next;
};

struct diagnostic {
    unsigned long long errors;
    unsigned long long warnings;
    unsigned long long notes;
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

struct defer_statement {
    struct token * first_token;
    struct token * last_token;
    struct secondary_block * secondary_block;
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

struct attribute_specifier_sequence {
    struct token * first_token;
    struct token * last_token;
    int   attributes_flags;
    struct attribute_specifier * head;
    struct attribute_specifier * tail;
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

struct function_specifier {
    int   flags;
    struct token * token;
};

struct type_qualifier_list {
    int   flags;
    struct type_qualifier * head;
    struct type_qualifier * tail;
};

struct primary_block {
    struct compound_statement * compound_statement;
    struct selection_statement * selection_statement;
    struct iteration_statement * iteration_statement;
    struct defer_statement * defer_statement;
    struct try_statement * try_statement;
};

struct member_declaration {
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct member_declarator_list * member_declarator_list_opt;
    struct static_assert_declaration * static_assert_declaration;
    struct pragma_declaration * pragma_declaration;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct member_declaration * next;
};

struct type_specifier_qualifier {
    struct type_specifier * type_specifier;
    struct type_qualifier * type_qualifier;
    struct alignment_specifier * alignment_specifier;
    struct type_specifier_qualifier * next;
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

struct defer_list_item {
    struct declarator * declarator;
    struct defer_statement * defer_statement;
    struct defer_list_item * next;
};

struct attribute_argument_clause {
    struct balanced_token_sequence * p_balanced_token_sequence;
    struct token * token;
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

struct storage_class_specifier {
    int   flags;
    struct token * token;
};

struct declaration_specifier {
    struct storage_class_specifier * storage_class_specifier;
    struct type_specifier_qualifier * type_specifier_qualifier;
    struct function_specifier * function_specifier;
    struct declaration_specifier * next;
};

struct balanced_token_sequence {
    struct balanced_token * head;
    struct balanced_token * tail;
};

struct init_declarator {
    unsigned char   has_shared_ownership;
    struct declarator * p_declarator;
    struct initializer * initializer;
    struct init_declarator * next;
};

struct try_statement {
    struct secondary_block * secondary_block;
    struct secondary_block * catch_secondary_block_opt;
    struct token * first_token;
    struct token * last_token;
    struct token * catch_token_opt;
    int catch_label_id;
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

struct array_declarator {
    struct direct_declarator * direct_declarator;
    struct expression * assignment_expression;
    struct expression * expression;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * token;
    struct token * static_token_opt;
};

struct braced_initializer {
    struct token * first_token;
    struct token * last_token;
    struct initializer_list * initializer_list;
};

struct direct_declarator {
    struct token * name_opt;
    struct token * p_calling_convention;
    struct declarator * declarator;
    struct array_declarator * array_declarator;
    struct function_declarator * function_declarator;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
};

struct secondary_block {
    struct token * first_token;
    struct token * last_token;
    struct statement * statement;
};

struct ast {
    struct token_list  token_list;
    struct declaration_list  declaration_list;
};

struct attribute {
    int   attributes_flags;
    struct attribute_token * attribute_token;
    struct attribute_argument_clause * attribute_argument_clause;
    struct attribute * next;
};

struct typeof_specifier_argument {
    struct expression * expression;
    struct type_name * type_name;
};

struct struct_entry_list {
    struct struct_entry ** data;
    int size;
    int capacity;
};

struct initializer_list {
    struct token * first_token;
    struct initializer_list_item * head;
    struct initializer_list_item * tail;
    int size;
};

struct init_statement {
    struct expression_statement * p_expression_statement;
    struct simple_declaration * p_simple_declaration;
};

struct function_declarator {
    struct direct_declarator * direct_declarator;
    struct scope  parameters_scope;
    struct parameter_type_list * parameter_type_list_opt;
};

struct struct_entry {
    unsigned char   done;
    struct struct_or_union_specifier * p_struct_or_union_specifier;
    struct struct_entry_list  dependencies;
    struct struct_entry * next;
};

struct d_visit_ctx {
    int indentation;
    int locals_count;
    int extern_count;
    int tag_name_count;
    struct hash_map  tag_names;
    struct hash_map  structs_map;
    struct hash_map  function_map;
    struct hash_map  static_declarators;
    struct osstream  local_declarators;
    struct osstream  add_this_before;
    struct osstream  add_this_before_external_decl;
    unsigned char   is_local;
    struct osstream  data_types;
    struct osstream  function_types;
    unsigned char   zero_mem_used;
    unsigned char   memcpy_used;
    struct declarator * p_current_function_opt;
    struct break_reference  break_reference;
    unsigned char   is__func__predefined_identifier_added;
    struct ast  ast;
};

struct param {
    struct type  type;
    struct param * next;
};

struct designator_list {
    struct designator * head;
    struct designator * tail;
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


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

int _Avx2WmemEnabledWeakValue = 0;
void hashmap_destroy(struct hash_map * map);
void ss_close(struct osstream * stream);

void d_visit_ctx_destroy(struct d_visit_ctx * ctx)
{
    hashmap_destroy(&ctx->tag_names);
    hashmap_destroy(&ctx->structs_map);
    hashmap_destroy(&ctx->function_map);
    hashmap_destroy(&ctx->static_declarators);
    ss_close(&ctx->local_declarators);
    ss_close(&ctx->add_this_before);
    ss_close(&ctx->add_this_before_external_decl);
    ss_close(&ctx->data_types);
    ss_close(&ctx->function_types);
}

struct struct_or_union_specifier *get_complete_struct_or_union_specifier(struct struct_or_union_specifier * p_struct_or_union_specifier);

static struct struct_or_union_specifier *get_complete_struct_or_union_specifier2(struct struct_or_union_specifier * p_struct_or_union_specifier)
{
    struct struct_or_union_specifier * p_complete;

    p_complete = get_complete_struct_or_union_specifier(p_struct_or_union_specifier);
    if (p_complete == 0U)
    {
        p_complete = p_struct_or_union_specifier->complete_struct_or_union_specifier_indirection;
    }
    return p_complete;
}

void free(void * ptr);

void struct_entry_delete(struct struct_entry * p)
{
    if (p)
    {
        free(p->dependencies.data);
        free(p);
    }
}

void *realloc(void * ptr, unsigned int size);

int struct_entry_list_reserve(struct struct_entry_list * p, int n)
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

int ss_fprintf(struct osstream * stream, char * fmt, ...);

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

static void d_visit_secondary_block(struct d_visit_ctx * ctx, struct osstream * oss, struct secondary_block * p_secondary_block);

static void il_print_defer_new(struct d_visit_ctx * ctx, struct osstream * oss, struct defer_list_item * p_item)
{
    if (p_item->defer_statement == 0U)
    {
        return;
    }
    d_visit_secondary_block(ctx, oss, p_item->defer_statement->secondary_block);
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

struct object *object_get_referenced(struct object * p_object);

static void object_print_value(struct osstream * ss, struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        register int   _R0 = a->value_type;
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
        goto _CKL0;

        {
            /*case 2*/ _CKL1:
            ss_fprintf(ss, "%d", a->value.bool_value ? 1 : 0);
            /*break*/ goto _CKL0;

            /*case 3*/ _CKL2:
            ss_fprintf(ss, "%d", (int)a->value.signed_char_value);
            /*break*/ goto _CKL0;

            /*case 4*/ _CKL3:
            ss_fprintf(ss, "%d", (int)a->value.unsigned_char_value);
            /*break*/ goto _CKL0;

            /*case 5*/ _CKL4:
            ss_fprintf(ss, "%d", a->value.signed_short_value);
            /*break*/ goto _CKL0;

            /*case 6*/ _CKL5:
            ss_fprintf(ss, "%d", a->value.unsigned_short_value);
            /*break*/ goto _CKL0;

            /*case 0*/ _CKL6:
            ss_fprintf(ss, "%d", a->value.signed_int_value);
            /*break*/ goto _CKL0;

            /*case 1*/ _CKL7:
            ss_fprintf(ss, "%u", a->value.unsigned_int_value);
            ss_fprintf(ss, "U");
            /*break*/ goto _CKL0;

            /*case 7*/ _CKL8:
            ss_fprintf(ss, "%ld", a->value.signed_long_value);
            ss_fprintf(ss, "L");
            /*break*/ goto _CKL0;

            /*case 8*/ _CKL9:
            ss_fprintf(ss, "%lu", a->value.unsigned_long_value);
            ss_fprintf(ss, "UL");
            /*break*/ goto _CKL0;

            /*case 9*/ _CKL10:
            ss_fprintf(ss, "%lld", a->value.signed_long_long_value);
            ss_fprintf(ss, "LL");
            /*break*/ goto _CKL0;

            /*case 10*/ _CKL11:
            ss_fprintf(ss, "%llu", a->value.unsigned_long_long_value);
            ss_fprintf(ss, "ULL");
            /*break*/ goto _CKL0;

            /*case 11*/ _CKL12:
            ss_fprintf(ss, "%f", a->value.float_value);
            ss_fprintf(ss, "f");
            /*break*/ goto _CKL0;

            /*case 12*/ _CKL13:
            ss_fprintf(ss, "%lf", a->value.double_value);
            /*break*/ goto _CKL0;

            /*case 13*/ _CKL14:
            ss_fprintf(ss, "%Lf", a->value.long_double_value);
            ss_fprintf(ss, "L");
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
}

unsigned char  object_has_constant_value(struct object * a);
unsigned char  object_to_bool(struct object * a);
unsigned char  type_is_bool(struct type * p_type);
static void d_visit_expression(struct d_visit_ctx * ctx, struct osstream * oss, struct expression * p_expression);

static void expression_to_bool_value(struct d_visit_ctx * ctx, struct osstream * oss, struct expression * p_expression)
{
    if (object_has_constant_value(&p_expression->object))
    {
        if (object_to_bool(&p_expression->object))
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
        if (type_is_bool(&p_expression->type))
        {
            d_visit_expression(ctx, oss, p_expression);
        }
        else
        {
            ss_fprintf(oss, "!!(");
            d_visit_expression(ctx, oss, p_expression);
            ss_fprintf(oss, ")");
        }
    }
}

int snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...);

static struct member_declarator *find_member_declarator_by_index2(struct member_declaration_list * list, int member_index, char name[100], int * p_count)
{
    int no_name_index;
    struct member_declaration * p_member_declaration;

    if (list->head == 0U)
    {
        return 0U;
    }
    no_name_index = 0;
    p_member_declaration = list->head;
    while (p_member_declaration)
    {
        struct member_declarator * p_member_declarator;

        p_member_declarator = 0U;
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
                        p_member_declarator = find_member_declarator_by_index2(&p_complete->member_declaration_list, member_index, mname, p_count);
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
    return 0U;
}

unsigned char  type_is_struct_or_union(struct type * p_type);

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
        p = find_member_declarator_by_index2(&p_complete->member_declaration_list, index, name, &count);
        if (p)
        {
            return 0;
        }
    }
    return 1;
}

static int il_visit_literal_string(struct token * current, struct osstream * oss)
{
    unsigned char   has_u8_prefix;
    unsigned char * psz;

    has_u8_prefix = !!(current->lexeme[0] == 117 && current->lexeme[1] == 56);
    psz = (unsigned char *)(current->lexeme + 0);
    if (has_u8_prefix)
    {
        psz = (unsigned char *)(current->lexeme + 2);
    }
    while (*psz)
    {
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

static char *get_op_by_expression_type(int   type)
{
    /*switch*/
    {
        register int   _R1 = type;
        if (_R1 == 53) goto _CKL1; /*case 53*/
        if (_R1 == 54) goto _CKL2; /*case 54*/
        if (_R1 == 55) goto _CKL3; /*case 55*/
        if (_R1 == 56) goto _CKL4; /*case 56*/
        if (_R1 == 57) goto _CKL5; /*case 57*/
        if (_R1 == 58) goto _CKL6; /*case 58*/
        if (_R1 == 59) goto _CKL7; /*case 59*/
        if (_R1 == 60) goto _CKL8; /*case 60*/
        if (_R1 == 61) goto _CKL9; /*case 61*/
        if (_R1 == 62) goto _CKL10; /*case 62*/
        if (_R1 == 63) goto _CKL11; /*case 63*/
        goto /*default*/ _CKL12;

        {
            /*case 53*/ _CKL1:
            return "=";
            /*case 54*/ _CKL2:
            return "+=";
            /*case 55*/ _CKL3:
            return "-=";
            /*case 56*/ _CKL4:
            return "*=";
            /*case 57*/ _CKL5:
            return "/=";
            /*case 58*/ _CKL6:
            return "%=";
            /*case 59*/ _CKL7:
            return "<<=";
            /*case 60*/ _CKL8:
            return ">>=";
            /*case 61*/ _CKL9:
            return "&=";
            /*case 62*/ _CKL10:
            return "|=";
            /*case 63*/ _CKL11:
            return "^=";
            /*default*/ _CKL12:
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    ;
    return "";
}

unsigned char  type_is_function(struct type * p_type);
struct map_entry *hashmap_find(struct hash_map * map, char * key);
int hashmap_set(struct hash_map * map, char * key, struct hash_item_set * item);
void hash_item_set_destroy(struct hash_item_set * p);
static void d_print_type(struct d_visit_ctx * ctx, struct osstream * ss, struct type * p_type, char * name_opt);
static void print_initializer(struct d_visit_ctx * ctx, struct osstream * oss, struct init_declarator * p_init_declarator, unsigned char   bstatic);
static void d_visit_compound_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct compound_statement * p_compound_statement);
void ss_swap(struct osstream * a, struct osstream * b);
char *strdup(char * src);
struct type type_remove_pointer(struct type * p_type);
void type_destroy(struct type * p_type);
void ss_clear(struct osstream * stream);
static void object_print_non_constant_initialization(struct d_visit_ctx * ctx, struct osstream * ss, struct object * object, char * declarator_name, unsigned char   all);
static void object_print_constant_initialization(struct d_visit_ctx * ctx, struct osstream * ss, struct object * object, unsigned char  * first);

static void d_visit_expression(struct d_visit_ctx * ctx, struct osstream * oss, struct expression * p_expression)
{
    if (object_has_constant_value(&p_expression->object))
    {
        object_print_value(oss, &p_expression->object);
        return;
    }
    /*switch*/
    {
        register int   _R2 = p_expression->expression_type;
        if (_R2 == 0) goto _CKL2; /*case 0*/
        if (_R2 == 4) goto _CKL3; /*case 4*/
        if (_R2 == 2) goto _CKL6; /*case 2*/
        if (_R2 == 3) goto _CKL15; /*case 3*/
        if (_R2 == 1) goto _CKL18; /*case 1*/
        if (_R2 == 5) goto _CKL19; /*case 5*/
        if (_R2 == 8) goto _CKL20; /*case 8*/
        if (_R2 == 6) goto _CKL21; /*case 6*/
        if (_R2 == 9) goto _CKL22; /*case 9*/
        if (_R2 == 7) goto _CKL23; /*case 7*/
        if (_R2 == 14) goto _CKL25; /*case 14*/
        if (_R2 == 15) goto _CKL27; /*case 15*/
        if (_R2 == 16) goto _CKL29; /*case 16*/
        if (_R2 == 17) goto _CKL30; /*case 17*/
        if (_R2 == 13) goto _CKL31; /*case 13*/
        if (_R2 == 12) goto _CKL32; /*case 12*/
        if (_R2 == 33) goto _CKL34; /*case 33*/
        if (_R2 == 10) goto _CKL35; /*case 10*/
        if (_R2 == 11) goto _CKL36; /*case 11*/
        if (_R2 == 18) goto _CKL38; /*case 18*/
        if (_R2 == 19) goto _CKL39; /*case 19*/
        if (_R2 == 24) goto _CKL40; /*case 24*/
        if (_R2 == 20) goto _CKL41; /*case 20*/
        if (_R2 == 26) goto _CKL42; /*case 26*/
        if (_R2 == 27) goto _CKL43; /*case 27*/
        if (_R2 == 28) goto _CKL44; /*case 28*/
        if (_R2 == 29) goto _CKL45; /*case 29*/
        if (_R2 == 30) goto _CKL46; /*case 30*/
        if (_R2 == 31) goto _CKL47; /*case 31*/
        if (_R2 == 32) goto _CKL48; /*case 32*/
        if (_R2 == 25) goto _CKL49; /*case 25*/
        if (_R2 == 39) goto _CKL50; /*case 39*/
        if (_R2 == 38) goto _CKL51; /*case 38*/
        if (_R2 == 35) goto _CKL52; /*case 35*/
        if (_R2 == 36) goto _CKL53; /*case 36*/
        if (_R2 == 37) goto _CKL54; /*case 37*/
        if (_R2 == 64) goto _CKL55; /*case 64*/
        if (_R2 == 53) goto _CKL56; /*case 53*/
        if (_R2 == 54) goto _CKL58; /*case 54*/
        if (_R2 == 55) goto _CKL59; /*case 55*/
        if (_R2 == 56) goto _CKL60; /*case 56*/
        if (_R2 == 57) goto _CKL61; /*case 57*/
        if (_R2 == 58) goto _CKL62; /*case 58*/
        if (_R2 == 59) goto _CKL63; /*case 59*/
        if (_R2 == 60) goto _CKL64; /*case 60*/
        if (_R2 == 61) goto _CKL65; /*case 61*/
        if (_R2 == 62) goto _CKL66; /*case 62*/
        if (_R2 == 63) goto _CKL67; /*case 63*/
        if (_R2 == 34) goto _CKL68; /*case 34*/
        if (_R2 == 40) goto _CKL69; /*case 40*/
        if (_R2 == 41) goto _CKL70; /*case 41*/
        if (_R2 == 42) goto _CKL71; /*case 42*/
        if (_R2 == 43) goto _CKL72; /*case 43*/
        if (_R2 == 46) goto _CKL73; /*case 46*/
        if (_R2 == 47) goto _CKL74; /*case 47*/
        if (_R2 == 48) goto _CKL75; /*case 48*/
        if (_R2 == 49) goto _CKL76; /*case 49*/
        if (_R2 == 50) goto _CKL77; /*case 50*/
        if (_R2 == 45) goto _CKL78; /*case 45*/
        if (_R2 == 44) goto _CKL79; /*case 44*/
        if (_R2 == 52) goto _CKL80; /*case 52*/
        if (_R2 == 51) goto _CKL81; /*case 51*/
        if (_R2 == 21) goto _CKL82; /*case 21*/
        if (_R2 == 22) goto _CKL83; /*case 22*/
        if (_R2 == 23) goto _CKL84; /*case 23*/
        if (_R2 == 65) goto _CKL85; /*case 65*/
        goto _CKL1;

        {
            /*case 0*/ _CKL2:
            ;
            /*break*/ goto _CKL1;

            /*case 4*/ _CKL3:
            {
                char func_name[200];
                char name[100];

                ;
                _cake_zmem(&func_name, 200);
                _cake_zmem(&name, 100);
                if (ctx->p_current_function_opt->name_opt)
                {
                    snprintf(func_name, 200U, "%s", ctx->p_current_function_opt->name_opt->lexeme);
                    snprintf(name, 100U, "__cake_func_%s", func_name);
                }
                else
                {
                    snprintf(func_name, 200U, "unnamed");
                    snprintf(name, 100U, "__cake_func_%p", ctx->p_current_function_opt);
                }
                if (!ctx->is__func__predefined_identifier_added)
                {
                    ;
                    ctx->is__func__predefined_identifier_added = 1;
                    ss_fprintf(&ctx->add_this_before_external_decl, "static const char %s[] = \"%s\";\n", name, func_name);
                }
                ss_fprintf(oss, "%s", name);
            }
            /*break*/ goto _CKL1;

            /*case 2*/ _CKL6:
            {
                if (type_is_function(&p_expression->declarator->type) || (p_expression->type.storage_class_specifier_flags & 2))
                {
                    char * func_name;
                    struct map_entry * p;

                    ss_fprintf(oss, "%s", p_expression->declarator->name_opt->lexeme);
                    func_name = 0U;
                    func_name = p_expression->first_token->lexeme;
                    p = hashmap_find(&ctx->function_map, func_name);
                    if (p == 0U)
                    {
                        struct hash_item_set  i;
                        struct osstream  ss;

                        _cake_zmem(&i, 32);
                        i.number = 1;
                        hashmap_set(&ctx->function_map, func_name, &i);
                        hash_item_set_destroy(&i);
                        _cake_zmem(&ss, 12);
                        d_print_type(ctx, &ss, &p_expression->type, func_name);
                        if (p_expression->p_init_declarator && p_expression->p_init_declarator->initializer)
                        {
                            print_initializer(ctx, oss, p_expression->p_init_declarator, 1);
                        }
                        if (type_is_function(&p_expression->declarator->type))
                        {
                            ;
                            if (p_expression->declarator->function_body && p_expression->declarator->declaration_specifiers && p_expression->declarator->declaration_specifiers->function_specifier_flags & 1)
                            {
                                struct osstream  copy;
                                int i2;

                                ss_fprintf(&ctx->function_types, "\n");
                                copy = *oss;
                                oss->c_str = 0;
                                oss->capacity = 0;
                                oss->size = 0;
                                i2 = ctx->indentation;
                                ctx->indentation = 0;
                                d_visit_compound_statement(ctx, oss, p_expression->declarator->function_body);
                                ctx->indentation = i2;
                                ss_fprintf(&ctx->function_types, "inline %s\n", ss.c_str);
                                ss_fprintf(&ctx->function_types, "%s", oss->c_str);
                                ss_swap(oss, &copy);
                                ss_close(&copy);
                            }
                            else
                            {
                                ss_fprintf(&ctx->function_types, "%s;\n", ss.c_str);
                            }
                        }
                        else
                        {
                            ss_fprintf(&ctx->function_types, "%s;\n", ss.c_str);
                        }
                        ss_close(&ss);
                    }
                }
                else
                {
                    if (!type_is_function(&p_expression->declarator->type) && p_expression->type.storage_class_specifier_flags & 4)
                    {
                        char newname[200];
                        void * p;

                        _cake_zmem(&newname, 200);
                        snprintf(newname, 200U, "%p", p_expression->declarator);
                        p = hashmap_find(&ctx->static_declarators, newname);
                        if (p == 0U)
                        {
                            struct hash_item_set  i;
                            struct osstream  ss;

                            _cake_zmem(&i, 32);
                            i.number = 1;
                            hashmap_set(&ctx->static_declarators, newname, &i);
                            ;
                            ;
                            ctx->extern_count++;
                            snprintf(newname, 200U, "__ck_%s%d", p_expression->declarator->name_opt->lexeme, ctx->extern_count);
                            free(p_expression->declarator->name_opt->lexeme);
                            p_expression->declarator->name_opt->lexeme = strdup(newname);
                            _cake_zmem(&ss, 12);
                            d_print_type(ctx, &ss, &p_expression->type, newname);
                            if (p_expression->p_init_declarator && p_expression->p_init_declarator->initializer)
                            {
                                print_initializer(ctx, &ss, p_expression->p_init_declarator, 1);
                                ss_fprintf(&ctx->function_types, "%s\n", ss.c_str);
                            }
                            else
                            {
                                ss_fprintf(&ctx->function_types, "%s;\n", ss.c_str);
                            }
                            ss_close(&ss);
                            hash_item_set_destroy(&i);
                        }
                        ss_fprintf(oss, "%s", p_expression->declarator->name_opt->lexeme);
                    }
                    else
                    {
                        ss_fprintf(oss, "%s", p_expression->declarator->name_opt->lexeme);
                    }
                }
            }
            /*break*/ goto _CKL1;

            /*case 3*/ _CKL15:
            {
                struct token * ptk;

                ptk = p_expression->first_token;
                do
                {
                    if (ptk->type == 130)
                    {
                        il_visit_literal_string(ptk, oss);
                    }
                    if (ptk == p_expression->last_token)
                    {
                        break;
                    }
                    ptk = ptk->next;
                }
                while (ptk);
            }
            /*break*/ goto _CKL1;

            /*case 1*/ _CKL18:
            /*case 5*/ _CKL19:
            /*case 8*/ _CKL20:
            /*case 6*/ _CKL21:
            object_print_value(oss, &p_expression->object);
            /*break*/ goto _CKL1;

            /*case 9*/ _CKL22:
            ;
            ss_fprintf(oss, "(");
            d_visit_expression(ctx, oss, p_expression->right);
            ss_fprintf(oss, ")");
            /*break*/ goto _CKL1;

            /*case 7*/ _CKL23:
            ;
            if (p_expression->generic_selection->p_view_selected_expression)
            {
                d_visit_expression(ctx, oss, p_expression->generic_selection->p_view_selected_expression);
            }
            /*break*/ goto _CKL1;

            /*case 14*/ _CKL25:
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
            /*break*/ goto _CKL1;

            /*case 15*/ _CKL27:
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
            /*break*/ goto _CKL1;

            /*case 16*/ _CKL29:
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, "++");
            /*break*/ goto _CKL1;

            /*case 17*/ _CKL30:
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, "--");
            /*break*/ goto _CKL1;

            /*case 13*/ _CKL31:
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, "[");
            d_visit_expression(ctx, oss, p_expression->right);
            ss_fprintf(oss, "]");
            /*break*/ goto _CKL1;

            /*case 12*/ _CKL32:
            {
                struct argument_expression * arg;

                ;
                d_visit_expression(ctx, oss, p_expression->left);
                ss_fprintf(oss, "(");
                arg = p_expression->argument_expression_list.head;
                while (arg)
                {
                    d_visit_expression(ctx, oss, arg->expression);
                    if (arg->next)
                    {
                        ss_fprintf(oss, ", ");
                    }
                    arg = arg->next;
                }
                ss_fprintf(oss, ")");
            }
            /*break*/ goto _CKL1;

            /*case 33*/ _CKL34:
            ;
            ss_fprintf(oss, "&");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 10*/ _CKL35:
            {
                char name[100];
                struct osstream  lambda;
                int current_indentation;
                struct declarator * p_current_function_opt;

                _cake_zmem(&name, 100);
                snprintf(name, 100U, "_lambda_%d", ctx->extern_count++);
                print_identation_core(&ctx->add_this_before, ctx->indentation);
                _cake_zmem(&lambda, 12);
                ss_fprintf(&lambda, "static ");
                d_print_type(ctx, &lambda, &p_expression->type, name);
                ss_fprintf(&lambda, "\n");
                current_indentation = ctx->indentation;
                ctx->indentation = 0;
                ;
                p_current_function_opt = ctx->p_current_function_opt;
                ctx->p_current_function_opt = p_expression->type_name->abstract_declarator;
                d_visit_compound_statement(ctx, &lambda, p_expression->compound_statement);
                ctx->indentation = current_indentation;
                ctx->p_current_function_opt = p_current_function_opt;
                ss_fprintf(&ctx->add_this_before_external_decl, "%s\n", lambda.c_str);
                ss_fprintf(oss, "%s", name);
                ss_close(&lambda);
            }
            /*break*/ goto _CKL1;

            /*case 11*/ _CKL36:
            {
                char name[100];

                _cake_zmem(&name, 100);
                snprintf(name, 100U, "__cmp_lt_%d", ctx->locals_count++);
                if (ctx->is_local)
                {
                    struct osstream  local;

                    _cake_zmem(&local, 12);
                    ss_swap(&ctx->local_declarators, &local);
                    print_identation_core(&local, ctx->indentation);
                    d_print_type(ctx, &local, &p_expression->type, name);
                    ss_fprintf(&local, ";\n", name);
                    ss_fprintf(&ctx->local_declarators, "%s", local.c_str);
                    ss_clear(&local);
                    object_print_non_constant_initialization(ctx, &local, &p_expression->object, name, 1);
                    ss_fprintf(&ctx->add_this_before, "%s", local.c_str);
                    ss_close(&local);
                    ss_fprintf(oss, "%s", name);
                }
                else
                {
                    struct osstream  local;
                    unsigned char   first;

                    _cake_zmem(&local, 12);
                    print_identation_core(&local, ctx->indentation);
                    d_print_type(ctx, &local, &p_expression->type, name);
                    first = 1;
                    ss_fprintf(&local, " = {");
                    object_print_constant_initialization(ctx, &local, &p_expression->object, &first);
                    ss_fprintf(&local, "};\n");
                    ss_fprintf(&ctx->add_this_before, "%s", local.c_str);
                    ss_close(&local);
                    ss_fprintf(oss, "%s", name);
                }
            }
            /*break*/ goto _CKL1;

            /*case 18*/ _CKL38:
            object_print_value(oss, &p_expression->object);
            /*break*/ goto _CKL1;

            /*case 19*/ _CKL39:
            object_print_value(oss, &p_expression->object);
            /*break*/ goto _CKL1;

            /*case 24*/ _CKL40:
            /*case 20*/ _CKL41:
            object_print_value(oss, &p_expression->object);
            /*break*/ goto _CKL1;

            /*case 26*/ _CKL42:
            ;
            ss_fprintf(oss, "++");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 27*/ _CKL43:
            ;
            ss_fprintf(oss, "--");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 28*/ _CKL44:
            ;
            ss_fprintf(oss, "!");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 29*/ _CKL45:
            ;
            ss_fprintf(oss, "~");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 30*/ _CKL46:
            ;
            ss_fprintf(oss, "-");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 31*/ _CKL47:
            ;
            ss_fprintf(oss, "+");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 32*/ _CKL48:
            ;
            ss_fprintf(oss, "*");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 25*/ _CKL49:
            /*break*/ goto _CKL1;

            /*case 39*/ _CKL50:
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " - ");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 38*/ _CKL51:
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " + ");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 35*/ _CKL52:
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " * ");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 36*/ _CKL53:
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " / ");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 37*/ _CKL54:
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, "%s", " % ");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 64*/ _CKL55:
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, ", ");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 53*/ _CKL56:
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
            /*break*/ goto _CKL1;

            /*case 54*/ _CKL58:
            /*case 55*/ _CKL59:
            /*case 56*/ _CKL60:
            /*case 57*/ _CKL61:
            /*case 58*/ _CKL62:
            /*case 59*/ _CKL63:
            /*case 60*/ _CKL64:
            /*case 61*/ _CKL65:
            /*case 62*/ _CKL66:
            /*case 63*/ _CKL67:
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " %s ", get_op_by_expression_type(p_expression->expression_type));
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 34*/ _CKL68:
            {
                struct osstream  local2;

                ;
                _cake_zmem(&local2, 12);
                d_print_type(ctx, &local2, &p_expression->type, 0U);
                ss_fprintf(oss, "(%s)", local2.c_str);
                ss_close(&local2);
                d_visit_expression(ctx, oss, p_expression->left);
            }
            /*break*/ goto _CKL1;

            /*case 40*/ _CKL69:
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " >> ");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 41*/ _CKL70:
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " << ");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 42*/ _CKL71:
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " > ");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 43*/ _CKL72:
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " < ");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 46*/ _CKL73:
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " == ");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 47*/ _CKL74:
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " != ");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 48*/ _CKL75:
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " & ");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 49*/ _CKL76:
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " ^ ");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 50*/ _CKL77:
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " | ");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 45*/ _CKL78:
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " <= ");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 44*/ _CKL79:
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " >= ");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 52*/ _CKL80:
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " && ");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 51*/ _CKL81:
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " || ");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

            /*case 21*/ _CKL82:
            /*case 22*/ _CKL83:
            /*case 23*/ _CKL84:
            /*break*/ goto _CKL1;

            /*case 65*/ _CKL85:
            ;
            ;
            ;
            d_visit_expression(ctx, oss, p_expression->condition_expr);
            ss_fprintf(oss, " ? ");
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, " : ");
            d_visit_expression(ctx, oss, p_expression->right);
            /*break*/ goto _CKL1;

        }
        _CKL1:;
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

struct type get_function_return_type(struct type * p_type);

static void d_visit_jump_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct jump_statement * p_jump_statement)
{
    if (p_jump_statement->first_token->type == 9037)
    {
        il_print_defer_list(ctx, oss, &p_jump_statement->defer_list);
        print_identation(ctx, oss);
        ss_fprintf(oss, "/*throw*/ goto _CKL%d;\n", p_jump_statement->label_id);
    }
    else
    {
        if (p_jump_statement->first_token->type == 9027)
        {
            struct type  return_type;
            unsigned char   constant_expression_or_void;

            return_type = get_function_return_type(&ctx->p_current_function_opt->type);
            constant_expression_or_void = !!(p_jump_statement->expression_opt == 0U || object_has_constant_value(&p_jump_statement->expression_opt->object) || il_defer_count(&p_jump_statement->defer_list) == 0);
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
                snprintf(name, 100U, "__ck%d", ctx->locals_count++);
                print_identation(ctx, oss);
                d_print_type(ctx, oss, &return_type, name);
                ss_fprintf(oss, " = ");
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
                        ss_fprintf(oss, "/*break*/ goto _CKL%d;\n\n", ctx->break_reference.p_selection_statement->label_id);
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

static void d_visit_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct statement * p_statement);

static void d_visit_labeled_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct labeled_statement * p_labeled_statement)
{
    ;
    d_visit_statement(ctx, oss, p_labeled_statement->statement);
}

static void d_visit_unlabeled_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct unlabeled_statement * p_unlabeled_statement);

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

static void d_visit_declaration(struct d_visit_ctx * ctx, struct osstream * oss, struct declaration * p_declaration);

static void d_visit_iteration_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct iteration_statement * p_iteration_statement)
{
    struct break_reference  old;

    old = ctx->break_reference;
    ctx->break_reference.p_iteration_statement = p_iteration_statement;
    ctx->break_reference.p_selection_statement = 0U;
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
                    struct osstream  local_declarators;
                    struct osstream  local;

                    ss_fprintf(oss, "{\n");
                    ctx->indentation++;
                    _cake_zmem(&local_declarators, 12);
                    ss_swap(&local_declarators, &ctx->local_declarators);
                    _cake_zmem(&local, 12);
                    d_visit_declaration(ctx, &local, p_iteration_statement->declaration);
                    ss_fprintf(oss, "%s", ctx->local_declarators.c_str);
                    if (local.c_str)
                    {
                        ss_fprintf(oss, "%s", local.c_str);
                    }
                    ss_swap(&local_declarators, &ctx->local_declarators);
                    print_identation(ctx, oss);
                    ss_fprintf(oss, "for (");
                    ss_close(&local_declarators);
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

static void d_visit_init_declarator_list(struct d_visit_ctx * ctx, struct osstream * oss, struct init_declarator_list * p_init_declarator_list, unsigned char   binline, unsigned char   bstatic);

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

static void d_visit_init_declarator(struct d_visit_ctx * ctx, struct osstream * oss, struct init_declarator * p_init_declarator, unsigned char   binline, unsigned char   bstatic);

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

static unsigned char  is_compound_statement(struct secondary_block * p_secondary_block)
{
    ;
    if (p_secondary_block->statement->unlabeled_statement && p_secondary_block->statement->unlabeled_statement->primary_block && p_secondary_block->statement->unlabeled_statement->primary_block->compound_statement)
    {
        return 1;
    }
    return 0;
}

int object_to_str(struct object * a, int n, char str[]);

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
        ctx->break_reference.p_iteration_statement = 0U;
        ctx->break_reference.p_selection_statement = p_selection_statement;
        ;
        _cake_zmem(&ss, 12);
        ss_fprintf(&ss, "/*switch*/\n");
        print_identation(ctx, &ss);
        ss_fprintf(&ss, "{\n");
        ctx->indentation++;
        _cake_zmem(&name, 100);
        snprintf(name, 100U, "_R%d", ctx->locals_count++);
        print_identation(ctx, &ss);
        ss_fprintf(&ss, "register ");
        d_print_type(ctx, &ss, &p_selection_statement->condition->expression->type, name);
        ss_fprintf(&ss, " = ");
        ;
        d_visit_condition(ctx, &ss, p_selection_statement->condition);
        ss_fprintf(&ss, ";\n");
        p_label = p_selection_statement->label_list.head;
        p_label_default = 0U;
        while (p_label)
        {
            if (p_label->p_first_token->type == 9007)
            {
                p_label_default = p_label;
            }
            else
            {
                print_identation(ctx, &ss);
                if (p_label->constant_expression_end == 0U)
                {
                    char str[50];

                    _cake_zmem(&str, 50);
                    object_to_str(&p_label->constant_expression->object, 50, str);
                    ss_fprintf(&ss, "if (%s == %s) goto _CKL%d; /*case %s*/\n", name, str, p_label->label_id, str);
                }
                else
                {
                    char str_begin[50];
                    char str_end[50];

                    _cake_zmem(&str_begin, 50);
                    object_to_str(&p_label->constant_expression->object, 50, str_begin);
                    _cake_zmem(&str_end, 50);
                    object_to_str(&p_label->constant_expression_end->object, 50, str_end);
                    ss_fprintf(&ss, "if (%s >= %s && %s <= %s) goto _CKL%d; /*case %s ... %s*/\n", name, str_begin, name, str_end, p_label->label_id, str_begin, str_end);
                }
            }
            p_label = p_label->next;
        }
        print_identation(ctx, &ss);
        if (p_label_default)
        {
            ss_fprintf(&ss, "goto /*default*/ _CKL%d;\n", p_label_default->label_id);
        }
        else
        {
            ss_fprintf(&ss, "goto _CKL%d;\n", p_selection_statement->label_id);
        }
        ss_fprintf(&ss, "\n");
        d_visit_secondary_block(ctx, &ss, p_selection_statement->secondary_block);
        print_identation(ctx, &ss);
        ss_fprintf(&ss, "_CKL%d:;\n", ctx->break_reference.p_selection_statement->label_id);
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
            unsigned char   addclose;

            addclose = 0;
            if (p_selection_statement->p_init_statement)
            {
                struct osstream  local_declarators;
                struct osstream  local2;

                ss_fprintf(oss, "{\n");
                addclose = 1;
                ctx->indentation++;
                _cake_zmem(&local_declarators, 12);
                ss_swap(&local_declarators, &ctx->local_declarators);
                _cake_zmem(&local2, 12);
                d_visit_init_statement(ctx, &local2, p_selection_statement->p_init_statement);
                ss_fprintf(oss, "%s", ctx->local_declarators.c_str);
                ss_fprintf(oss, "\n");
                ss_fprintf(oss, "%s", local2.c_str);
                ss_close(&local2);
                print_identation(ctx, oss);
                ss_swap(&local_declarators, &ctx->local_declarators);
                ss_close(&local_declarators);
            }
            if (p_selection_statement->condition)
            {
                if (p_selection_statement->condition->p_init_declarator)
                {
                    struct osstream  local_declarators;
                    struct osstream  local2;

                    ss_fprintf(oss, "{\n");
                    ctx->indentation++;
                    addclose = 1;
                    _cake_zmem(&local_declarators, 12);
                    ss_swap(&local_declarators, &ctx->local_declarators);
                    _cake_zmem(&local2, 12);
                    d_visit_init_declarator(ctx, &local2, p_selection_statement->condition->p_init_declarator, 0, 0);
                    ss_fprintf(oss, "%s", ctx->local_declarators.c_str);
                    ss_fprintf(oss, "\n");
                    ss_fprintf(oss, "%s", local2.c_str);
                    ss_close(&local2);
                    ss_swap(&local_declarators, &ctx->local_declarators);
                    print_identation(ctx, oss);
                    ss_fprintf(oss, "if (");
                    ss_fprintf(oss, "%s", p_selection_statement->condition->p_init_declarator->p_declarator->name_opt->lexeme);
                    ss_fprintf(oss, ")\n");
                    ss_close(&local_declarators);
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
    ss_fprintf(oss, "/*try*/ if (1)\n");
    d_visit_secondary_block(ctx, oss, p_try_statement->secondary_block);
    print_identation(ctx, oss);
    ss_fprintf(oss, "/*catch*/ else _CKL%d:\n", p_try_statement->catch_label_id);
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

static void d_visit_label(struct d_visit_ctx * ctx, struct osstream * oss, struct label * p_label)
{
    if (p_label->p_first_token->type == 9001)
    {
        char str[50];

        print_identation(ctx, oss);
        _cake_zmem(&str, 50);
        object_to_str(&p_label->constant_expression->object, 50, str);
        if (p_label->constant_expression_end == 0U)
        {
            ss_fprintf(oss, "/*case %s*/ ", str);
        }
        else
        {
            char str2[50];

            _cake_zmem(&str2, 50);
            object_to_str(&p_label->constant_expression_end->object, 50, str2);
            ss_fprintf(oss, "/*case %s ... %s*/ ", str, str2);
        }
        ss_fprintf(oss, "_CKL%d:\n", p_label->label_id);
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
                ss_fprintf(oss, "/*default*/ _CKL%d:\n", p_label->label_id);
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
    unsigned char   is_local;
    struct osstream  local_declarators;
    struct osstream  local;
    unsigned char   ends_with_jump;

    is_local = ctx->is_local;
    ctx->is_local = 1;
    _cake_zmem(&local_declarators, 12);
    ss_swap(&ctx->local_declarators, &local_declarators);
    _cake_zmem(&local, 12);
    ctx->indentation++;
    d_visit_block_item_list(ctx, &local, &p_compound_statement->block_item_list);
    ends_with_jump = 0;
    if (p_compound_statement->block_item_list.tail && p_compound_statement->block_item_list.tail->unlabeled_statement && p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement != 0U)
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
    if (ctx->local_declarators.c_str)
    {
        ss_fprintf(oss, "%s", ctx->local_declarators.c_str);
        ss_fprintf(oss, "\n");
    }
    if (local.c_str)
    {
        ss_fprintf(oss, "%s", local.c_str);
    }
    print_identation(ctx, oss);
    ss_fprintf(oss, "}\n");
    ctx->is_local = is_local;
    ss_swap(&ctx->local_declarators, &local_declarators);
    ss_close(&local_declarators);
    ss_close(&local);
}

void *calloc(unsigned int nmemb, unsigned int size);
unsigned char  type_is_array(struct type * p_type);
struct type get_array_item_type(struct type * p_type);

static void register_struct_types_and_functions(struct d_visit_ctx * ctx, struct type * p_type0)
{
    /*try*/ if (1)
    {
        struct type * p_type;

        p_type = p_type0;
        while (p_type)
        {
            /*switch*/
            {
                register int   _R3 = p_type->category;
                if (_R3 == 0) goto _CKL2; /*case 0*/
                if (_R3 == 1) goto _CKL24; /*case 1*/
                if (_R3 == 2) goto _CKL25; /*case 2*/
                if (_R3 == 3) goto _CKL26; /*case 3*/
                goto _CKL1;

                {
                    /*case 0*/ _CKL2:
                    {
                        if (p_type->struct_or_union_specifier)
                        {
                            struct struct_or_union_specifier * p_complete;

                            p_complete = get_complete_struct_or_union_specifier2(p_type->struct_or_union_specifier);
                            if (p_complete)
                            {
                                char name[100];
                                struct map_entry * p;

                                _cake_zmem(&name, 100);
                                snprintf(name, 100U, "%p", (void *)p_complete);
                                p = hashmap_find(&ctx->structs_map, name);
                                if (p == 0U)
                                {
                                    struct map_entry * p_name;
                                    struct struct_entry * p_struct_entry;
                                    struct member_declaration * member_declaration;

                                    p_name = hashmap_find(&ctx->tag_names, p_complete->tag_name);
                                    if (p_name != 0U)
                                    {
                                        char new_name[100];
                                        struct hash_item_set  i;

                                        _cake_zmem(&new_name, 100);
                                        snprintf(new_name, 100U, "%s%d", p_complete->tag_name, ctx->tag_name_count++);
                                        snprintf(p_complete->tag_name, 200U, "%s", new_name);
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
                                    p_struct_entry = calloc(1, 24U);
                                    if (p_struct_entry == 0U)
                                    {
                                        /*throw*/ goto _CKL0;
                                    }
                                    p_struct_entry->p_struct_or_union_specifier = p_complete;
                                    {
                                        struct hash_item_set  i;

                                        _cake_zmem(&i, 32);
                                        i.p_struct_entry = p_struct_entry;
                                        hashmap_set(&ctx->structs_map, name, &i);
                                        hash_item_set_destroy(&i);
                                    }
                                    member_declaration = p_complete->member_declaration_list.head;
                                    while (member_declaration)
                                    {
                                        struct member_declarator * member_declarator;

                                        member_declarator = 0U;
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
                                                        if (p_complete_member == 0U)
                                                        {
                                                            /*throw*/ goto _CKL0;
                                                        }
                                                        _cake_zmem(&name2, 100);
                                                        snprintf(name2, 100U, "%p", (void *)p_complete_member);
                                                        register_struct_types_and_functions(ctx, &member_declarator->declarator->type);
                                                        p2 = hashmap_find(&ctx->structs_map, name2);
                                                        if (p2 != 0U)
                                                        {
                                                            ;
                                                            struct_entry_list_push_back(&p_struct_entry->dependencies, p2->data.p_struct_entry);
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
                                                            snprintf(name2, 100U, "%p", (void *)p_complete_member);
                                                            register_struct_types_and_functions(ctx, &t);
                                                            p2 = hashmap_find(&ctx->structs_map, name2);
                                                            if (p2 != 0U)
                                                            {
                                                                ;
                                                                struct_entry_list_push_back(&p_struct_entry->dependencies, p2->data.p_struct_entry);
                                                            }
                                                        }
                                                        else
                                                        {
                                                            register_struct_types_and_functions(ctx, &member_declarator->declarator->type);
                                                        }
                                                        type_destroy(&t);
                                                    }
                                                    else
                                                    {
                                                        register_struct_types_and_functions(ctx, &member_declarator->declarator->type);
                                                    }
                                                }
                                                member_declarator = member_declarator->next;
                                            }
                                        }
                                        else
                                        {
                                            if (member_declaration->specifier_qualifier_list != 0U)
                                            {
                                                if (member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                                                {
                                                    struct type  t;

                                                    _cake_zmem(&t, 60);
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
                                                        snprintf(name2, 100U, "%p", (void *)p_complete_member);
                                                        register_struct_types_and_functions(ctx, &t);
                                                        p2 = hashmap_find(&ctx->structs_map, name2);
                                                        if (p2 != 0U)
                                                        {
                                                            ;
                                                            struct_entry_list_push_back(&p_struct_entry->dependencies, p2->data.p_struct_entry);
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
                                                            if (p_complete_member == 0U)
                                                            {
                                                                type_destroy(&t2);
                                                                /*throw*/ goto _CKL0;
                                                            }
                                                            _cake_zmem(&name2, 100);
                                                            snprintf(name2, 100U, "%p", (void *)p_complete_member);
                                                            register_struct_types_and_functions(ctx, &t);
                                                            p2 = hashmap_find(&ctx->structs_map, name2);
                                                            if (p2 != 0U)
                                                            {
                                                                ;
                                                                struct_entry_list_push_back(&p_struct_entry->dependencies, p2->data.p_struct_entry);
                                                            }
                                                        }
                                                        else
                                                        {
                                                            register_struct_types_and_functions(ctx, &t);
                                                        }
                                                        type_destroy(&t2);
                                                    }
                                                    else
                                                    {
                                                        register_struct_types_and_functions(ctx, &t);
                                                    }
                                                    type_destroy(&t);
                                                }
                                            }
                                        }
                                        member_declaration = member_declaration->next;
                                    }
                                }
                            }
                        }
                        else
                        {
                        }
                    }
                    /*break*/ goto _CKL1;

                    /*case 1*/ _CKL24:
                    {
                        struct param * pa;

                        pa = p_type->params.head;
                        while (pa)
                        {
                            register_struct_types_and_functions(ctx, &pa->type);
                            pa = pa->next;
                        }
                    }
                    /*break*/ goto _CKL1;

                    /*case 2*/ _CKL25:
                    /*case 3*/ _CKL26:
                    /*break*/ goto _CKL1;

                }
                _CKL1:;
            }
            p_type = p_type->next;
        }
    }
    /*catch*/ else _CKL0:
    {
    }
}

unsigned char  struct_or_union_specifier_is_union(struct struct_or_union_specifier * p);
struct enum_specifier *get_complete_enum_specifier(struct enum_specifier * p_enum_specifier);
unsigned char  print_type_specifier_flags(struct osstream * ss, unsigned char  * first, int   e_type_specifier_flags);

static void d_print_type_core(struct d_visit_ctx * ctx, struct osstream * ss, struct type * p_type0, char * name_opt)
{
    struct type * p_type;

    p_type = p_type0;
    while (p_type)
    {
        /*switch*/
        {
            register int   _R4 = p_type->category;
            if (_R4 == 0) goto _CKL1; /*case 0*/
            if (_R4 == 2) goto _CKL10; /*case 2*/
            if (_R4 == 1) goto _CKL14; /*case 1*/
            if (_R4 == 3) goto _CKL22; /*case 3*/
            goto _CKL0;

            {
                unsigned char   b;
                struct param * pa;

                /*case 0*/ _CKL1:
                {
                    struct osstream  local;
                    unsigned char   first;
                    struct osstream  local2;

                    _cake_zmem(&local, 12);
                    first = 1;
                    if (p_type->struct_or_union_specifier)
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
                    else
                    {
                        if (p_type->enum_specifier)
                        {
                            struct enum_specifier * p_enum_specifier;

                            p_enum_specifier = get_complete_enum_specifier(p_type->enum_specifier);
                            if (p_enum_specifier && p_enum_specifier->specifier_qualifier_list)
                            {
                                print_type_specifier_flags(&local, &first, p_enum_specifier->specifier_qualifier_list->type_specifier_flags);
                            }
                            else
                            {
                                ss_fprintf(&local, "int ");
                            }
                        }
                        else
                        {
                            if (p_type->type_specifier_flags & 512)
                            {
                                ss_fprintf(&local, "unsigned char ");
                            }
                            else
                            {
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
                        name_opt = 0U;
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
                /*break*/ goto _CKL0;

                /*case 2*/ _CKL10:
                if (name_opt)
                {
                    ss_fprintf(ss, "%s", name_opt);
                    name_opt = 0U;
                }
                ss_fprintf(ss, "[");
                b = 1;
                if (p_type->num_of_elements > 0)
                {
                    if (!b)
                    {
                        ss_fprintf(ss, " ");
                    }
                    ss_fprintf(ss, "%d", p_type->num_of_elements);
                }
                ss_fprintf(ss, "]");
                /*break*/ goto _CKL0;

                /*case 1*/ _CKL14:
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
                    name_opt = 0U;
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
                /*break*/ goto _CKL0;

                /*case 3*/ _CKL22:
                {
                    struct osstream  local;
                    unsigned char   first;

                    _cake_zmem(&local, 12);
                    if (p_type->next && ((p_type->next->category == 1 || p_type->next->category == 2)))
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
                    if (name_opt)
                    {
                        if (!first)
                        {
                            ss_fprintf(ss, " ");
                        }
                        ss_fprintf(ss, "%s", name_opt);
                        name_opt = 0U;
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
                /*break*/ goto _CKL0;

            }
            _CKL0:;
        }
        p_type = p_type->next;
    }
}

static void d_print_type(struct d_visit_ctx * ctx, struct osstream * ss, struct type * p_type, char * name_opt)
{
    struct osstream  local;

    register_struct_types_and_functions(ctx, p_type);
    _cake_zmem(&local, 12);
    d_print_type_core(ctx, &local, p_type, name_opt);
    if (p_type->storage_class_specifier_flags & 2)
    {
        ss_fprintf(ss, "extern ");
    }
    if (p_type->storage_class_specifier_flags & 4)
    {
        ss_fprintf(ss, "static ");
    }
    ss_fprintf(ss, "%s", local.c_str);
    ss_close(&local);
}

unsigned char  object_is_reference(struct object * p_object);

static unsigned char  is_all_zero(struct object * object)
{
    if (object_is_reference(object))
    {
        object = object_get_referenced(object);
    }
    if (object->members != 0U)
    {
        struct object * member;

        member = object->members;
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
            if (object_to_bool(&object->p_init_expression->object) != 0)
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

unsigned char  type_is_union(struct type * p_type);

static void object_print_constant_initialization(struct d_visit_ctx * ctx, struct osstream * ss, struct object * object, unsigned char  * first)
{
    if (object_is_reference(object))
    {
        object = object_get_referenced(object);
    }
    if (object->p_init_expression && object->p_init_expression->expression_type == 3)
    {
        if (!(*first))
        {
            ss_fprintf(ss, ", ");
        }
        *first = 0;
        il_visit_literal_string(object->p_init_expression->first_token, ss);
        return;
    }
    if (object->members != 0U)
    {
        if (type_is_union(&object->type))
        {
            struct object * member;

            member = object->members;
            object_print_constant_initialization(ctx, ss, member, first);
        }
        else
        {
            struct object * member;

            member = object->members;
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
            ss_fprintf(ss, ", ");
        }
        *first = 0;
        if (object->p_init_expression)
        {
            if (object_has_constant_value(&object->p_init_expression->object))
            {
                object_print_value(ss, &object->p_init_expression->object);
            }
            else
            {
                if (object->p_init_expression->expression_type == 3)
                {
                    il_visit_literal_string(object->p_init_expression->first_token, ss);
                }
                else
                {
                    ss_fprintf(ss, "0");
                }
            }
        }
        else
        {
            ss_fprintf(ss, "0");
        }
    }
}

static void object_print_non_constant_initialization(struct d_visit_ctx * ctx, struct osstream * ss, struct object * object, char * declarator_name, unsigned char   all)
{
    if (object_is_reference(object))
    {
        object = object_get_referenced(object);
    }
    if (object->members != 0U)
    {
        if (type_is_union(&object->type))
        {
            struct object * member;

            member = object->members;
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
                        ss_fprintf(ss, "%s%s = ", declarator_name, member->debug_name);
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
                            print_identation_core(ss, ctx->indentation);
                            ss_fprintf(ss, "%s%s = 0;\n", declarator_name, member->debug_name);
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

                print_identation_core(ss, ctx->indentation);
                ss_fprintf(ss, "_cake_memcpy(%s%s, ", declarator_name, object->debug_name);
                _cake_zmem(&local, 12);
                d_visit_expression(ctx, &local, object->p_init_expression);
                ss_fprintf(ss, "%s, %d", local.c_str, object->type.num_of_elements);
                ss_fprintf(ss, ");\n");
                ss_close(&local);
                ctx->memcpy_used = 1;
            }
            else
            {
                struct object * member;

                member = object->members;
                while (member)
                {
                    object_print_non_constant_initialization(ctx, ss, member, declarator_name, all);
                    member = member->next;
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
                        ss_fprintf(ss, "%s%s = ", declarator_name, object->debug_name);
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
                ss_fprintf(ss, "%s%s = ", declarator_name, object->debug_name);
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
                print_identation_core(ss, ctx->indentation);
                ss_fprintf(ss, "%s%s = 0;\n", declarator_name, object->debug_name);
            }
        }
    }
}

unsigned int type_get_sizeof(struct type * p_type);

static void print_initializer(struct d_visit_ctx * ctx, struct osstream * oss, struct init_declarator * p_init_declarator, unsigned char   bstatic)
{
    unsigned char   is_local;

    ;
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
            print_identation_core(oss, ctx->indentation);
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

                        sz = type_get_sizeof(&p_init_declarator->p_declarator->type);
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
                    unsigned char   first;

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
                        object_print_non_constant_initialization(ctx, oss, &p_init_declarator->p_declarator->object, p_init_declarator->p_declarator->name_opt->lexeme, 1);
                    }
                }
            }
            else
            {
                if (is_local && !bstatic)
                {
                    unsigned int sz;

                    sz = type_get_sizeof(&p_init_declarator->p_declarator->type);
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

static void d_visit_init_declarator(struct d_visit_ctx * ctx, struct osstream * oss0, struct init_declarator * p_init_declarator, unsigned char   binline, unsigned char   bstatic)
{
    struct osstream * oss;

    oss = 0U;
    /*try*/ if (1)
    {
        unsigned char   is_local;
        unsigned char   is_static_not_function;
        struct osstream  ss;

        is_local = ctx->is_local;
        is_static_not_function = !!(bstatic && !type_is_function(&p_init_declarator->p_declarator->type));
        oss = is_static_not_function ? &ctx->add_this_before_external_decl : oss0;
        if (type_is_function(&p_init_declarator->p_declarator->type) && p_init_declarator->p_declarator->function_body == 0U)
        {
            return;
        }
        if (p_init_declarator->p_declarator->declaration_specifiers && ((p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & 2) || (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & 1)))
        {
            return;
        }
        if (is_local && is_static_not_function)
        {
            return;
        }
        if (p_init_declarator->p_declarator->name_opt == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        if (binline)
        {
            ss_fprintf(oss, "__inline ");
        }
        _cake_zmem(&ss, 12);
        if (is_local && !bstatic)
        {
            d_print_type(ctx, &ss, &p_init_declarator->p_declarator->type, p_init_declarator->p_declarator->name_opt->lexeme);
            print_identation(ctx, &ctx->local_declarators);
            ss_fprintf(&ctx->local_declarators, "%s;\n", ss.c_str);
            ss_clear(&ss);
        }
        else
        {
            if (is_local)
            {
                print_identation(ctx, &ss);
            }
            d_print_type(ctx, &ss, &p_init_declarator->p_declarator->type, p_init_declarator->p_declarator->name_opt->lexeme);
            if (p_init_declarator->initializer == 0U && p_init_declarator->p_declarator->function_body == 0U)
            {
                ss_fprintf(oss, "%s;\n", ss.c_str);
            }
            else
            {
                ss_fprintf(oss, "%s", ss.c_str);
            }
        }
        if (p_init_declarator->initializer)
        {
            char newname[200];
            struct hash_item_set  i;

            _cake_zmem(&newname, 200);
            snprintf(newname, 200U, "%p", p_init_declarator->p_declarator);
            _cake_zmem(&i, 32);
            i.number = 1;
            hashmap_set(&ctx->static_declarators, newname, &i);
            print_initializer(ctx, oss, p_init_declarator, bstatic);
            hash_item_set_destroy(&i);
        }
        else
        {
        }
        if (p_init_declarator->p_declarator->function_body)
        {
            struct hash_item_set  i;

            ;
            ctx->is__func__predefined_identifier_added = 0;
            ctx->p_current_function_opt = p_init_declarator->p_declarator;
            _cake_zmem(&i, 32);
            i.number = 1;
            hashmap_set(&ctx->function_map, p_init_declarator->p_declarator->name_opt->lexeme, &i);
            ss_fprintf(oss, "\n");
            d_visit_compound_statement(ctx, oss, p_init_declarator->p_declarator->function_body);
            ss_fprintf(oss, "\n");
            hash_item_set_destroy(&i);
        }
        ss_close(&ss);
    }
    /*catch*/ else _CKL0:
    {
    }
}

static void d_visit_init_declarator_list(struct d_visit_ctx * ctx, struct osstream * oss, struct init_declarator_list * p_init_declarator_list, unsigned char   binline, unsigned char   bstatic)
{
    struct init_declarator * p_init_declarator;

    p_init_declarator = p_init_declarator_list->head;
    while (p_init_declarator)
    {
        d_visit_init_declarator(ctx, oss, p_init_declarator, binline, bstatic);
        p_init_declarator = p_init_declarator->next;
    }
}

static void d_visit_declaration(struct d_visit_ctx * ctx, struct osstream * oss, struct declaration * p_declaration)
{
    unsigned char   binline;

    binline = 0;
    if (p_declaration->declaration_specifiers && p_declaration->declaration_specifiers->head)
    {
        struct declaration_specifier * p;

        p = p_declaration->declaration_specifiers->head;
        while (p)
        {
            if (p->function_specifier && p->function_specifier->token->type == 9018)
            {
                binline = 1;
            }
            p = p->next;
        }
    }
    if (p_declaration->init_declarator_list.head)
    {
        unsigned char   is_static;

        ;
        is_static = !!(p_declaration->declaration_specifiers->storage_class_specifier_flags & 4);
        if (!binline)
        {
            d_visit_init_declarator_list(ctx, oss, &p_declaration->init_declarator_list, binline, is_static);
        }
    }
}

static void print_complete_struct(struct d_visit_ctx * ctx, struct osstream * ss, struct struct_or_union_specifier * p_struct_or_union_specifier)
{
    /*try*/ if (1)
    {
        struct struct_or_union_specifier * p_complete;
        struct member_declaration * member_declaration;
        int no_name_index;

        p_complete = get_complete_struct_or_union_specifier2(p_struct_or_union_specifier);
        if (p_complete == 0U)
        {
            /*throw*/ goto _CKL0;
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

            member_declarator = 0U;
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
                            d_print_type(ctx, ss, &member_declarator->declarator->type, member_declarator->declarator->name_opt->lexeme);
                            member_declarator->declarator->type.num_of_elements = 0;
                        }
                        else
                        {
                            d_print_type(ctx, ss, &member_declarator->declarator->type, member_declarator->declarator->name_opt->lexeme);
                        }
                        ss_fprintf(ss, ";\n");
                    }
                    member_declarator = member_declarator->next;
                }
            }
            else
            {
                if (member_declaration->specifier_qualifier_list != 0U)
                {
                    if (member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                    {
                        struct type  t;
                        char name[100];

                        _cake_zmem(&t, 60);
                        t.category = 0;
                        t.struct_or_union_specifier = member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                        t.type_specifier_flags = 32768;
                        _cake_zmem(&name, 100);
                        snprintf(name, 100U, "__m%d", no_name_index++);
                        ss_fprintf(ss, "    ");
                        d_print_type(ctx, ss, &t, name);
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
    /*catch*/ else _CKL0:
    {
    }
}

void print_complete_structs(struct d_visit_ctx * ctx, struct osstream * ss, struct struct_entry * p_struct_entry)
{
    if (p_struct_entry->done)
    {
        return;
    }
    {
        int i;
        i = 0;
        for (; i < p_struct_entry->dependencies.size; i++)
        {
            struct struct_entry * p_struct_entry_item;

            p_struct_entry_item = p_struct_entry->dependencies.data[i];
            print_complete_structs(ctx, ss, p_struct_entry_item);
        }
    }
    if (!p_struct_entry->done)
    {
        struct osstream  local;

        p_struct_entry->done = 1;
        _cake_zmem(&local, 12);
        print_complete_struct(ctx, &local, p_struct_entry->p_struct_or_union_specifier);
        if (local.c_str)
        {
            ss_fprintf(ss, local.c_str);
        }
        ss_close(&local);
    }
}

void d_visit(struct d_visit_ctx * ctx, struct osstream * oss)
{
    struct osstream  declarations;
    struct declaration * p_declaration;

    _cake_zmem(&declarations, 12);
    ctx->indentation = 0;
    p_declaration = ctx->ast.declaration_list.head;
    while (p_declaration)
    {
        struct osstream  declaration;

        ss_clear(&ctx->add_this_before_external_decl);
        ss_clear(&ctx->function_types);
        _cake_zmem(&declaration, 12);
        d_visit_declaration(ctx, &declaration, p_declaration);
        if (ctx->function_types.size > 0)
        {
            ss_fprintf(&declarations, "%s\n", ctx->function_types.c_str);
            ss_clear(&ctx->function_types);
        }
        if (ctx->add_this_before.size > 0)
        {
            ss_fprintf(oss, "%s", ctx->add_this_before.c_str);
            ss_clear(&ctx->add_this_before);
        }
        if (ctx->add_this_before_external_decl.size > 0)
        {
            ss_fprintf(&declarations, "%s", ctx->add_this_before_external_decl.c_str);
            ss_clear(&ctx->add_this_before_external_decl);
        }
        if (declaration.size > 0)
        {
            ss_fprintf(&declarations, "%s", declaration.c_str);
        }
        ss_close(&declaration);
        p_declaration = p_declaration->next;
    }
    if (ctx->data_types.c_str)
    {
        ss_fprintf(oss, "%s", ctx->data_types.c_str);
        ss_fprintf(oss, "\n");
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
                print_complete_structs(ctx, oss, entry->data.p_struct_entry);
                entry = entry->next;
            }
        }
    }
    ss_fprintf(oss, "\n");
    if (ctx->zero_mem_used)
    {
        char * str;

        str = "static void _cake_zmem(void *dest, register unsigned int len)\n""{\n""  register unsigned char *ptr = (unsigned char*)dest;\n""  while (len-- > 0) *ptr++ = 0;\n""}\n\n";
        ss_fprintf(oss, "%s", str);
    }
    if (ctx->memcpy_used)
    {
        char * str;

        str = "static void _cake_memcpy(void * dest, const void * src, unsigned long n)\n""{\n""  char *csrc = (char *)src;\n""  char *cdest = (char *)dest;\n""  for (int i=0; i<n; i++) cdest[i] = csrc[i]; \n""}\n\n";
        ss_fprintf(oss, "%s", str);
    }
    if (declarations.c_str)
    {
        ss_fprintf(oss, "%s", declarations.c_str);
    }
    ss_close(&declarations);
}

