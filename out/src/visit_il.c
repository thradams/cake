struct member_declaration_list {
    struct token * first_token;
    struct token * last_token;
    struct member_declaration * head;
    struct member_declaration * tail;
};

struct declaration_list {
    struct declaration * head;
    struct declaration * tail;
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

struct type_name {
    struct token * first_token;
    struct token * last_token;
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct declarator * abstract_declarator;
    struct type  type;
};

struct pragma_declaration {
    struct token * first_token;
    struct token * last_token;
};

struct pointer {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct pointer * pointer;
};

struct attribute_list {
    int   attributes_flags;
    struct attribute * head;
    struct attribute * tail;
};

struct alignment_specifier {
    struct type_name * type_name;
    struct expression * constant_expression;
    struct token * token;
};

struct designator {
    struct expression * constant_expression_opt;
    struct token * token;
    struct designator * next;
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

struct typeof_specifier {
    struct token * first_token;
    struct token * last_token;
    struct typeof_specifier_argument * typeof_specifier_argument;
    struct type  type;
};

struct unlabeled_statement {
    struct expression_statement * expression_statement;
    struct primary_block * primary_block;
    struct jump_statement * jump_statement;
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

struct ast {
    struct token_list  token_list;
    struct declaration_list  declaration_list;
};

struct attribute_token {
    int   attributes_flags;
    struct token * token;
};

struct balanced_token_sequence {
    struct balanced_token * head;
    struct balanced_token * tail;
};

struct initializer {
    struct token * first_token;
    struct braced_initializer * braced_initializer;
    struct expression * assignment_expression;
};

struct hash_map {
    struct map_entry ** table;
    int capacity;
    int size;
};

struct osstream {
    char * c_str;
    int size;
    int capacity;
};

struct d_visit_ctx {
    int indentation;
    int locals_count;
    int extern_count;
    int tag_name_count;
    struct hash_map  tag_names;
    struct hash_map  structs_map;
    struct hash_map  function_map;
    struct osstream  add_this_before;
    struct osstream  add_this_before_external_decl;
    struct osstream  data_types;
    struct osstream  function_types;
    struct ast  ast;
};

struct attribute_specifier_sequence {
    struct token * first_token;
    struct token * last_token;
    int   attributes_flags;
    struct attribute_specifier * head;
    struct attribute_specifier * tail;
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

struct try_statement {
    struct secondary_block * secondary_block;
    struct secondary_block * catch_secondary_block_opt;
    struct token * first_token;
    struct token * last_token;
    struct token * catch_token_opt;
    int try_catch_block_index;
};

struct argument_expression_list {
    struct argument_expression * head;
    struct argument_expression * tail;
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

struct secondary_block {
    struct token * first_token;
    struct token * last_token;
    struct statement * statement;
};

struct defer_statement {
    struct token * first_token;
    struct token * last_token;
    struct secondary_block * secondary_block;
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

struct hash_item_set {
    unsigned int number;
    struct enum_specifier * p_enum_specifier;
    struct enumerator * p_enumerator;
    struct struct_or_union_specifier * p_struct_or_union_specifier;
    struct declarator * p_declarator;
    struct init_declarator * p_init_declarator;
    struct macro0 * p_macro;
    struct struct_entry * p_struct_entry;
};

struct attribute_argument_clause {
    struct balanced_token_sequence * p_balanced_token_sequence;
    struct token * token;
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
    int   state;
    struct flow_objects_view  alternatives;
    struct flow_object_state * next;
};

struct block_item_list {
    struct block_item * head;
    struct block_item * tail;
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

struct typeof_specifier_argument {
    struct expression * expression;
    struct type_name * type_name;
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

struct simple_declaration {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator_list  init_declarator_list;
    struct token * first_token;
    struct token * last_token;
};

struct attribute {
    int   attributes_flags;
    struct attribute_token * attribute_token;
    struct attribute_argument_clause * attribute_argument_clause;
    struct attribute * next;
};

struct macro;
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

struct struct_entry_list {
    struct struct_entry ** data;
    int size;
    int capacity;
};

struct struct_entry {
    unsigned char   done;
    struct struct_or_union_specifier * p_struct_or_union_specifier;
    struct struct_entry_list  dependencies;
    struct struct_entry * next;
};

struct type_qualifier {
    int   flags;
    struct token * token;
    struct type_qualifier * next;
};

struct initializer_list_item {
    struct designation * designation;
    struct initializer * initializer;
    struct initializer_list_item * next;
};

struct type_specifier_qualifier {
    struct type_specifier * type_specifier;
    struct type_qualifier * type_qualifier;
    struct alignment_specifier * alignment_specifier;
    struct type_specifier_qualifier * next;
};

struct labeled_statement {
    struct label * label;
    struct statement * statement;
};

struct braced_initializer {
    struct token * first_token;
    struct token * last_token;
    struct initializer_list * initializer_list;
};

struct direct_declarator {
    struct token * name_opt;
    struct declarator * declarator;
    struct array_declarator * array_declarator;
    struct function_declarator * function_declarator;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
};

struct init_declarator {
    unsigned char   has_shared_ownership;
    struct declarator * p_declarator;
    struct initializer * initializer;
    struct init_declarator * next;
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

struct type_specifier {
    int   flags;
    struct token * token;
    struct struct_or_union_specifier * struct_or_union_specifier;
    struct typeof_specifier * typeof_specifier;
    struct enum_specifier * enum_specifier;
    struct declarator * typedef_declarator;
    struct atomic_type_specifier * atomic_type_specifier;
};

struct member_declarator {
    struct declarator * declarator;
    struct expression * constant_expression;
    struct member_declarator * next;
};

struct condition {
    struct expression * expression;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator * p_init_declarator;
    struct token * first_token;
    struct token * last_token;
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

struct label {
    struct expression * constant_expression;
    struct token * p_identifier_opt;
    struct token * p_first_token;
};

struct atomic_type_specifier {
    struct token * token;
    struct type_name * type_name;
};

struct jump_statement {
    struct token * label;
    struct token * first_token;
    struct token * last_token;
    struct expression * expression_opt;
    int try_catch_block_index;
    struct defer_list  defer_list;
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

struct parameter_type_list {
    unsigned char   is_var_args;
    unsigned char   is_void;
    struct parameter_list * parameter_list;
};

struct primary_block {
    struct compound_statement * compound_statement;
    struct selection_statement * selection_statement;
    struct iteration_statement * iteration_statement;
    struct defer_statement * defer_statement;
    struct try_statement * try_statement;
};

struct defer_list_item {
    struct declarator * declarator;
    struct defer_statement * defer_statement;
    struct defer_list_item * next;
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

struct macro0;
struct function_specifier {
    int   flags;
    struct token * token;
};

struct statement {
    struct labeled_statement * labeled_statement;
    struct unlabeled_statement * unlabeled_statement;
};

struct init_statement {
    struct expression_statement * p_expression_statement;
    struct simple_declaration * p_simple_declaration;
};

struct argument_expression {
    struct expression * expression;
    struct argument_expression * next;
    unsigned char   set_unkown;
};

struct scope {
    int scope_level;
    struct hash_map  tags;
    struct hash_map  variables;
    struct scope * next;
    struct scope * previous;
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

struct expression_statement {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct expression * expression_opt;
};

struct balanced_token {
    struct token * token;
    struct balanced_token * next;
};

struct static_assert_declaration {
    struct token * first_token;
    struct token * last_token;
    struct expression * constant_expression;
    struct token * string_literal_opt;
};

struct param {
    struct type  type;
    struct param * next;
};

struct parameter_declaration {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct declaration_specifiers * declaration_specifiers;
    struct declarator * declarator;
    struct parameter_declaration * next;
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

struct initializer_list {
    struct token * first_token;
    struct initializer_list_item * head;
    struct initializer_list_item * tail;
    int size;
};

struct designation {
    struct designator_list * designator_list;
    struct token * token;
};

struct designator_list {
    struct designator * head;
    struct designator * tail;
};

struct block_item {
    struct token * first_token;
    struct declaration * declaration;
    struct unlabeled_statement * unlabeled_statement;
    struct label * label;
    struct block_item * next;
};

struct generic_association {
    struct type  type;
    struct type_name * p_type_name;
    struct expression * expression;
    struct token * first_token;
    struct token * last_token;
    struct generic_association * next;
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

struct declaration_specifier {
    struct storage_class_specifier * storage_class_specifier;
    struct type_specifier_qualifier * type_specifier_qualifier;
    struct function_specifier * function_specifier;
    struct declaration_specifier * next;
};

struct member_declarator_list {
    struct member_declarator * head;
    struct member_declarator * tail;
};

struct storage_class_specifier {
    int   flags;
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

struct array_declarator {
    struct direct_declarator * direct_declarator;
    struct expression * assignment_expression;
    struct expression * expression;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * token;
    struct token * static_token_opt;
};

struct type_qualifier_list {
    int   flags;
    struct type_qualifier * head;
    struct type_qualifier * tail;
};

struct parameter_list {
    struct parameter_declaration * head;
    struct parameter_declaration * tail;
};


struct struct_or_union_specifier *get_complete_struct_or_union_specifier(struct struct_or_union_specifier * p_struct_or_union_specifier);

struct struct_or_union_specifier *get_complete_struct_or_union_specifier2(struct struct_or_union_specifier * p_struct_or_union_specifier)
{
    struct struct_or_union_specifier * p_complete = get_complete_struct_or_union_specifier(p_struct_or_union_specifier);
    if (p_complete == ((void *)0))
    {
        p_complete = p_struct_or_union_specifier;
    }
    return p_complete;
}

void *realloc(void * ptr, unsigned int size);

int struct_entry_list_reserve(struct struct_entry_list * p, int n)
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

int struct_entry_list_push_back(struct struct_entry_list * p, struct struct_entry * book)
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
        int error = struct_entry_list_reserve(p, new_capacity);
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
        int i = 0;
        for (; i < indentation; i++)
        {
            ss_fprintf(ss, "    ");
        }
    }
}

static void print_identation(struct d_visit_ctx * ctx, struct osstream * oss)
{
    {
        int i = 0;
        for (; i < ctx->indentation; i++)
        {
            ss_fprintf(oss, "    ");
        }
    }
}

static void d_visit_secondary_block(struct d_visit_ctx * ctx, struct osstream * oss, struct secondary_block * p_secondary_block);

static void il_print_defer_new(struct d_visit_ctx * ctx, struct osstream * oss, struct defer_list_item * p_item)
{
    if (p_item->defer_statement == ((void *)0))
    {
        return;
    }
    d_visit_secondary_block(ctx, oss, p_item->defer_statement->secondary_block);
}

static int il_defer_count(struct defer_list * p_defer_list)
{
    int count = 0;
    struct defer_list_item * p_item = p_defer_list->head;
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
    struct defer_list_item * p_item = p_defer_list->head;
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
    switch (a->value_type)
    {
        case 2 :
        ss_fprintf(ss, "%d", a->value.bool_value ? 1 : 0);
        break;
        case 3 :
        ss_fprintf(ss, "%d", (int)a->value.signed_char_value);
        break;
        case 4 :
        ss_fprintf(ss, "%d", (int)a->value.unsigned_char_value);
        break;
        case 5 :
        ss_fprintf(ss, "%d", a->value.signed_short_value);
        break;
        case 6 :
        ss_fprintf(ss, "%d", a->value.unsigned_short_value);
        break;
        case 0 :
        ss_fprintf(ss, "%d", a->value.signed_int_value);
        break;
        case 1 :
        ss_fprintf(ss, "%u", a->value.unsigned_int_value);
        ss_fprintf(ss, "U");
        break;
        case 7 :
        ss_fprintf(ss, "%ld", a->value.signed_long_value);
        ss_fprintf(ss, "L");
        break;
        case 8 :
        ss_fprintf(ss, "%lu", a->value.unsigned_long_value);
        ss_fprintf(ss, "UL");
        break;
        case 9 :
        ss_fprintf(ss, "%lld", a->value.signed_long_long_value);
        ss_fprintf(ss, "LL");
        break;
        case 10 :
        ss_fprintf(ss, "%llu", a->value.unsigned_long_long_value);
        ss_fprintf(ss, "ULL");
        break;
        case 11 :
        ss_fprintf(ss, "%f", a->value.float_value);
        ss_fprintf(ss, "f");
        break;
        case 12 :
        ss_fprintf(ss, "%lf", a->value.double_value);
        break;
        case 13 :
        ss_fprintf(ss, "%Lf", a->value.long_double_value);
        ss_fprintf(ss, "L");
        break;
    }
}

int snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...);

static struct member_declarator *find_member_declarator_by_index2(struct member_declaration_list * list, int member_index, char name[100], int * p_count)
{
    if (list->head == ((void *)0))
    {
        return ((void *)0);
    }
    int no_name_index = 0;
    struct member_declaration * p_member_declaration = list->head;
    while (p_member_declaration)
    {
        struct member_declarator * p_member_declarator = ((void *)0);
        if (p_member_declaration->member_declarator_list_opt)
        {
            p_member_declarator = p_member_declaration->member_declarator_list_opt->head;
            while (p_member_declarator)
            {
                if (p_member_declarator->declarator)
                {
                    if (member_index ==  *p_count)
                    {
                        snprintf(name, 100, "%s", p_member_declarator->declarator->name_opt->lexeme);
                        return p_member_declarator;
                    }
                }
                ( *p_count)++;
                p_member_declarator = p_member_declarator->next;
            }
        }
        else
        {
            if (p_member_declaration->specifier_qualifier_list)
            {
                if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                {
                    struct struct_or_union_specifier * p_complete = get_complete_struct_or_union_specifier(p_member_declaration->specifier_qualifier_list->struct_or_union_specifier);
                    if (p_complete)
                    {
                        char mname[100];
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
    return ((void *)0);
}

unsigned char  type_is_struct_or_union(struct type * p_type);

int find_member_name(struct type * p_type, int index, char name[100])
{
    if ( !type_is_struct_or_union(p_type))
    {
        return 1;
    }
    struct struct_or_union_specifier * p_complete = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
    if (p_complete)
    {
        int count = 0;
        struct member_declarator * p = find_member_declarator_by_index2(&p_complete->member_declaration_list, index, name, &count);
        if (p)
        {
            return 0;
        }
    }
    return 1;
}

static int il_visit_literal_string(struct token * current, struct osstream * oss)
{
    unsigned char   has_u8_prefix = current->lexeme[0] == 117 && current->lexeme[1] == 56;
    unsigned char * psz = (unsigned char *)(current->lexeme + 0);
    if (has_u8_prefix)
    {
        psz = (unsigned char *)(current->lexeme + 2);
    }
    while ( *psz)
    {
        if ( *psz >= 128)
        {
            ss_fprintf(oss, "\\x%x",  *psz);
        }
        else
        {
            ss_fprintf(oss, "%c",  *psz);
        }
        psz++;
    }
    return 0;
}

unsigned char  type_is_function(struct type * p_type);
struct map_entry *hashmap_find(struct hash_map * map, char * key);
int hashmap_set(struct hash_map * map, char * key, struct hash_item_set * item);
static void d_print_type(struct d_visit_ctx * ctx, struct osstream * ss, struct type * p_type, char * name_opt);
static void d_visit_compound_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct compound_statement * p_compound_statement);
void ss_close(struct osstream * stream);
struct type type_remove_pointer(struct type * p_type);
void type_destroy(struct type * p_type);
static void object_print_constant_initialization(struct d_visit_ctx * ctx, struct osstream * ss, struct object * object, unsigned char  * first);
static void object_print_non_constant_initialization(struct d_visit_ctx * ctx, struct osstream * ss, struct object * object, char * declarator_name);

static void d_visit_expression(struct d_visit_ctx * ctx, struct osstream * oss, struct expression * p_expression)
{
    switch (p_expression->expression_type)
    {
        case 0 :
        ;
        break;
        case 4 :
        break;
        case 2 :
        {
            ss_fprintf(oss, "%s", p_expression->declarator->name_opt->lexeme);
            if (type_is_function(&p_expression->declarator->type) || (p_expression->type.storage_class_specifier_flags & 2))
            {
                char * func_name = ((void *)0);
                func_name = p_expression->first_token->lexeme;
                struct map_entry * p = hashmap_find(&ctx->function_map, func_name);
                if (p == ((void *)0))
                {
                    struct hash_item_set  i = {0};
                    i.number = 1;
                    hashmap_set(&ctx->function_map, func_name, &i);
                    struct osstream  ss = {0};
                    d_print_type(ctx, &ss, &p_expression->type, func_name);
                    if (type_is_function(&p_expression->declarator->type))
                    {
                        if (p_expression->declarator->function_body && p_expression->declarator->declaration_specifiers && p_expression->declarator->declaration_specifiers->function_specifier_flags & 1)
                        {
                            ss_fprintf(&ctx->function_types, "\n");
                            struct osstream  copy =  *oss;
                            oss->c_str = 0;
                            oss->capacity = 0;
                            oss->size = 0;
                            int i2 = ctx->indentation;
                            ctx->indentation = 0;
                            d_visit_compound_statement(ctx, oss, p_expression->declarator->function_body);
                            ctx->indentation = i2;
                            ss_fprintf(&ctx->function_types, "inline %s\n", ss.c_str);
                            ss_fprintf(&ctx->function_types, "%s", oss->c_str);
                            ss_close(oss);
                             *oss = copy;
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
        }
        break;
        case 3 :
        il_visit_literal_string(p_expression->first_token, oss);
        break;
        case 1 :
        case 5 :
        case 8 :
        case 6 :
        object_print_value(oss, &p_expression->object);
        break;
        case 9 :
        ;
        ss_fprintf(oss, "(");
        d_visit_expression(ctx, oss, p_expression->right);
        ss_fprintf(oss, ")");
        break;
        case 7 :
        ;
        if (p_expression->generic_selection->p_view_selected_expression)
        {
            d_visit_expression(ctx, oss, p_expression->generic_selection->p_view_selected_expression);
        }
        break;
        case 14 :
        {
            d_visit_expression(ctx, oss, p_expression->left);
            char name[100];
            int r = find_member_name(&p_expression->left->type, p_expression->member_index, name);
            if (r == 0)
            {
                ss_fprintf(oss, ".%s", name);
            }
        }
        break;
        case 15 :
        {
            d_visit_expression(ctx, oss, p_expression->left);
            {
                struct type  t = type_remove_pointer(&p_expression->left->type);
                char name[100];
                int r = find_member_name(&t, p_expression->member_index, name);
                if (r == 0)
                {
                    ss_fprintf(oss, "->%s", name);
                }
                type_destroy(&t);
            }
        }
        break;
        case 16 :
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, "++");
        break;
        case 17 :
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, "--");
        break;
        case 13 :
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, "[");
        d_visit_expression(ctx, oss, p_expression->right);
        ss_fprintf(oss, "]");
        break;
        case 12 :
        {
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, "(");
            struct argument_expression * arg = p_expression->argument_expression_list.head;
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
        break;
        case 33 :
        ss_fprintf(oss, "&");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 10 :
        {
            char name[100];
            snprintf(name, sizeof (name), "_lambda_%d", ctx->extern_count++);
            print_identation_core(&ctx->add_this_before, ctx->indentation);
            struct osstream  lambda = {0};
            ss_fprintf(&lambda, "static ");
            d_print_type(ctx, &lambda, &p_expression->type, name);
            ss_fprintf(&lambda, "\n");
            int current_indentation = ctx->indentation;
            ctx->indentation = 0;
            d_visit_compound_statement(ctx, &lambda, p_expression->compound_statement);
            ctx->indentation = current_indentation;
            ss_fprintf(&ctx->add_this_before_external_decl, "%s\n", lambda.c_str);
            ss_fprintf(oss, "%s", name);
        }
        break;
        case 11 :
        {
            char name[100];
            snprintf(name, sizeof (name), "__cmp_lt_%d", ctx->locals_count++);
            struct osstream  local = {0};
            print_identation_core(&local, ctx->indentation);
            d_print_type(ctx, &local, &p_expression->type, name);
            unsigned char   first = 1;
            ss_fprintf(&local, " = {");
            object_print_constant_initialization(ctx, &local, &p_expression->object, &first);
            ss_fprintf(&local, "};\n");
            object_print_non_constant_initialization(ctx, &local, &p_expression->object, name);
            ss_fprintf(&ctx->add_this_before, "%s", local.c_str);
            ss_close(&local);
            ss_fprintf(oss, "%s", name);
        }
        break;
        case 18 :
        ss_fprintf(oss, "sizeof ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 19 :
        {
            struct osstream  local0 = {0};
            d_print_type(ctx, &local0, &p_expression->type_name->type, 0);
            ss_fprintf(oss, "sizeof (%s)", local0.c_str);
            ss_close(&local0);
        }
        break;
        case 24 :
        case 20 :
        object_print_value(oss, &p_expression->object);
        break;
        case 26 :
        d_visit_expression(ctx, oss, p_expression->right);
        ss_fprintf(oss, " ++");
        break;
        case 27 :
        d_visit_expression(ctx, oss, p_expression->right);
        ss_fprintf(oss, " --");
        break;
        case 28 :
        ss_fprintf(oss, " !");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 29 :
        ss_fprintf(oss, " ~");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 30 :
        ss_fprintf(oss, " -");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 31 :
        ss_fprintf(oss, " +");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 32 :
        ss_fprintf(oss, " *");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 25 :
        break;
        case 39 :
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " - ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 38 :
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " + ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 35 :
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " * ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 36 :
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " / ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 37 :
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, "%s", " % ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 54 :
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, ", ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 53 :
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " = ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 34 :
        {
            struct osstream  local2 = {0};
            d_print_type(ctx, &local2, &p_expression->type, ((void *)0));
            ss_fprintf(oss, "(%s)", local2.c_str);
            ss_close(&local2);
            d_visit_expression(ctx, oss, p_expression->left);
        }
        break;
        case 40 :
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " >> ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 41 :
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " << ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 42 :
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " > ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 43 :
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " < ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 46 :
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " == ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 47 :
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " != ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 48 :
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " & ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 49 :
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " ^ ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 50 :
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " | ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 45 :
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " <= ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 44 :
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " >= ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 52 :
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " && ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 51 :
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " || ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
        case 21 :
        case 22 :
        case 23 :
        break;
        case 55 :
        d_visit_expression(ctx, oss, p_expression->condition_expr);
        ss_fprintf(oss, " ? ");
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " : ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
    }
}

static void d_visit_declarator(struct d_visit_ctx * ctx, struct osstream * oss, struct declarator * p_declarator)
{
}

void ss_clear(struct osstream * stream);

static void d_visit_expression_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct expression_statement * p_expression_statement)
{
    ss_clear(&ctx->add_this_before);
    struct osstream  local = {0};
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

unsigned char  object_has_constant_value(struct object * a);

static void d_visit_jump_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct jump_statement * p_jump_statement)
{
    if (p_jump_statement->first_token->type == 9037)
    {
        il_print_defer_list(ctx, oss, &p_jump_statement->defer_list);
        print_identation(ctx, oss);
        ss_fprintf(oss, "goto _catch_label_%d;\n", p_jump_statement->try_catch_block_index);
    }
    else
    {
        if (p_jump_statement->first_token->type == 9027)
        {
            unsigned char   constant_expression_or_void = p_jump_statement->expression_opt == ((void *)0) || object_has_constant_value(&p_jump_statement->expression_opt->object) || il_defer_count(&p_jump_statement->defer_list) == 0;
            if (constant_expression_or_void)
            {
                il_print_defer_list(ctx, oss, &p_jump_statement->defer_list);
                print_identation(ctx, oss);
                ss_fprintf(oss, "return");
                if (p_jump_statement->expression_opt)
                {
                    ss_fprintf(oss, " ");
                    d_visit_expression(ctx, oss, p_jump_statement->expression_opt);
                }
                ss_fprintf(oss, ";\n");
            }
            else
            {
                char name[100] = {0};
                snprintf(name, sizeof name, "__ck%d", ctx->locals_count++);
                print_identation(ctx, oss);
                d_print_type(ctx, oss, &p_jump_statement->expression_opt->type, name);
                ss_fprintf(oss, " = ");
                if (p_jump_statement->expression_opt)
                {
                    d_visit_expression(ctx, oss, p_jump_statement->expression_opt);
                }
                ss_fprintf(oss, ";\n");
                il_print_defer_list(ctx, oss, &p_jump_statement->defer_list);
                print_identation(ctx, oss);
                ss_fprintf(oss, "return %s;\n", name);
            }
        }
        else
        {
            if (p_jump_statement->first_token->type == 9000 || p_jump_statement->first_token->type == 9005)
            {
                il_print_defer_list(ctx, oss, &p_jump_statement->defer_list);
                print_identation(ctx, oss);
                if (p_jump_statement->first_token->type == 9000)
                {
                    ss_fprintf(oss, "break;\n");
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
    if (p_labeled_statement->label)
    {
    }
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
            d_visit_expression(ctx, oss, p_iteration_statement->expression1);
            ss_fprintf(oss, ");\n");
        }
        else
        {
            if (p_iteration_statement->first_token->type == 9015)
            {
                if (p_iteration_statement->declaration)
                {
                    ss_fprintf(oss, "{\n");
                    ctx->indentation++;
                    d_visit_declaration(ctx, oss, p_iteration_statement->declaration);
                    print_identation(ctx, oss);
                    ss_fprintf(oss, "for (");
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
}

static void d_visit_init_declarator_list(struct d_visit_ctx * ctx, struct osstream * oss, struct init_declarator_list * p_init_declarator_list, unsigned char   binline);

static void d_visit_simple_declaration(struct d_visit_ctx * ctx, struct osstream * oss, struct simple_declaration * p_simple_declaration)
{
    d_visit_init_declarator_list(ctx, oss, &p_simple_declaration->init_declarator_list, 0);
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

static void d_visit_init_declarator(struct d_visit_ctx * ctx, struct osstream * oss, struct init_declarator * p_init_declarator, unsigned char   binline);

static void d_visit_condition(struct d_visit_ctx * ctx, struct osstream * oss, struct condition * p_condition)
{
    if (p_condition->p_init_declarator)
    {
        d_visit_init_declarator(ctx, oss, p_condition->p_init_declarator, 0);
    }
    if (p_condition->expression)
    {
        d_visit_expression(ctx, oss, p_condition->expression);
    }
}

static unsigned char  is_compound_statement(struct secondary_block * p_secondary_block)
{
    if (p_secondary_block->statement && p_secondary_block->statement->unlabeled_statement && p_secondary_block->statement->unlabeled_statement->primary_block && p_secondary_block->statement->unlabeled_statement->primary_block->compound_statement)
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
        ss_fprintf(oss, "switch (");
        d_visit_condition(ctx, oss, p_selection_statement->condition);
        ss_fprintf(oss, ")\n");
        d_visit_secondary_block(ctx, oss, p_selection_statement->secondary_block);
    }
    else
    {
        if (p_selection_statement->first_token->type == 9017)
        {
            unsigned char   addclose = 0;
            if (p_selection_statement->p_init_statement)
            {
                ss_fprintf(oss, "{\n");
                addclose = 1;
                ctx->indentation++;
                d_visit_init_statement(ctx, oss, p_selection_statement->p_init_statement);
                print_identation(ctx, oss);
            }
            if (p_selection_statement->condition)
            {
                if (p_selection_statement->condition->p_init_declarator)
                {
                    ss_fprintf(oss, "{\n");
                    ctx->indentation++;
                    addclose = 1;
                    d_visit_init_declarator(ctx, oss, p_selection_statement->condition->p_init_declarator, 0);
                    print_identation(ctx, oss);
                    ss_fprintf(oss, "if (");
                    ss_fprintf(oss, "%s", p_selection_statement->condition->p_init_declarator->p_declarator->name_opt->lexeme);
                    ss_fprintf(oss, ")\n");
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
            if (p_selection_statement->secondary_block)
            {
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
    ss_fprintf(oss, "if (1)\n");
    d_visit_secondary_block(ctx, oss, p_try_statement->secondary_block);
    print_identation(ctx, oss);
    ss_fprintf(oss, "else _catch_label_%d:\n", p_try_statement->try_catch_block_index);
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
        print_identation(ctx, oss);
        ss_fprintf(oss, "case ");
        object_print_value(oss, &p_label->constant_expression->object);
        ss_fprintf(oss, " :\n");
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
                ss_fprintf(oss, "default:\n");
            }
        }
    }
}

void ss_swap(struct osstream * a, struct osstream * b);

static void d_visit_block_item(struct d_visit_ctx * ctx, struct osstream * oss, struct block_item * p_block_item)
{
    struct osstream  ss0 = {0};
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
    struct block_item * p_block_item = p_block_item_list->head;
    while (p_block_item)
    {
        d_visit_block_item(ctx, oss, p_block_item);
        p_block_item = p_block_item->next;
    }
}

static void d_visit_compound_statement(struct d_visit_ctx * ctx, struct osstream * oss, struct compound_statement * p_compound_statement)
{
    print_identation(ctx, oss);
    ss_fprintf(oss, "{\n");
    ctx->indentation++;
    d_visit_block_item_list(ctx, oss, &p_compound_statement->block_item_list);
    unsigned char   ends_with_jump = 0;
    if (p_compound_statement->block_item_list.tail && p_compound_statement->block_item_list.tail->unlabeled_statement && p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement != ((void *)0))
    {
        ends_with_jump = 1;
    }
    if ( !ends_with_jump)
    {
        il_print_defer_list(ctx, oss, &p_compound_statement->defer_list);
    }
    ctx->indentation--;
    print_identation(ctx, oss);
    ss_fprintf(oss, "}\n");
}

void *calloc(int nmemb, unsigned int size);
unsigned char  type_is_array(struct type * p_type);
struct type get_array_item_type(struct type * p_type);

static void register_struct_types_and_functions(struct d_visit_ctx * ctx, struct type * p_type0)
{
    if (1)
    {
        struct type * p_type = p_type0;
        while (p_type)
        {
            switch (p_type->category)
            {
                case 0 :
                {
                    if (p_type->struct_or_union_specifier)
                    {
                        struct struct_or_union_specifier * p_complete = get_complete_struct_or_union_specifier2(p_type->struct_or_union_specifier);
                        if (p_complete)
                        {
                            char name[100];
                            snprintf(name, sizeof name, "%p", (void *)p_complete);
                            struct map_entry * p = hashmap_find(&ctx->structs_map, name);
                            if (p == ((void *)0))
                            {
                                struct map_entry * p_name = hashmap_find(&ctx->tag_names, p_complete->tag_name);
                                if (p_name != ((void *)0))
                                {
                                    char new_name[100];
                                    snprintf(new_name, sizeof name, "%s%d", p_complete->tag_name, ctx->tag_name_count++);
                                    snprintf(p_complete->tag_name, sizeof p_complete->tag_name, "%s", new_name);
                                    struct hash_item_set  i = {0};
                                    i.number = 1;
                                    hashmap_set(&ctx->tag_names, new_name, &i);
                                }
                                else
                                {
                                    struct hash_item_set  i = {0};
                                    i.number = 1;
                                    hashmap_set(&ctx->tag_names, p_complete->tag_name, &i);
                                }
                                struct struct_entry * p_struct_entry = calloc(1, sizeof  *p_struct_entry);
                                if (p_struct_entry == ((void *)0))
                                {
                                    goto _catch_label_1;
                                }
                                p_struct_entry->p_struct_or_union_specifier = p_complete;
                                struct hash_item_set  i = {0};
                                i.p_struct_entry = p_struct_entry;
                                hashmap_set(&ctx->structs_map, name, &i);
                                struct member_declaration * member_declaration = p_complete->member_declaration_list.head;
                                while (member_declaration)
                                {
                                    struct member_declarator * member_declarator = ((void *)0);
                                    if (member_declaration->member_declarator_list_opt)
                                    {
                                        member_declarator = member_declaration->member_declarator_list_opt->head;
                                        while (member_declarator)
                                        {
                                            if (type_is_struct_or_union(&member_declarator->declarator->type))
                                            {
                                                struct struct_or_union_specifier * p_complete_member = p_complete_member = get_complete_struct_or_union_specifier(member_declarator->declarator->type.struct_or_union_specifier);
                                                char name2[100];
                                                snprintf(name2, sizeof name2, "%p", (void *)p_complete_member);
                                                register_struct_types_and_functions(ctx, &member_declarator->declarator->type);
                                                struct map_entry * p2 = hashmap_find(&ctx->structs_map, name2);
                                                if (p2 != ((void *)0))
                                                {
                                                    struct_entry_list_push_back(&p_struct_entry->dependencies, p2->data.p_struct_entry);
                                                }
                                            }
                                            if (type_is_array(&member_declarator->declarator->type))
                                            {
                                                struct type  t = get_array_item_type(&member_declarator->declarator->type);
                                                if (type_is_struct_or_union(&t))
                                                {
                                                    struct struct_or_union_specifier * p_complete_member = p_complete_member = get_complete_struct_or_union_specifier(t.struct_or_union_specifier);
                                                    char name2[100];
                                                    snprintf(name2, sizeof name2, "%p", (void *)p_complete_member);
                                                    register_struct_types_and_functions(ctx, &t);
                                                    struct map_entry * p2 = hashmap_find(&ctx->structs_map, name2);
                                                    if (p2 != ((void *)0))
                                                    {
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
                                            member_declarator = member_declarator->next;
                                        }
                                    }
                                    else
                                    {
                                        if (member_declaration->specifier_qualifier_list != ((void *)0))
                                        {
                                            if (member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                                            {
                                                struct type  t = {0};
                                                t.category = 0;
                                                t.struct_or_union_specifier = member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                                                t.type_specifier_flags = 32768;
                                                if (type_is_struct_or_union(&t))
                                                {
                                                    struct struct_or_union_specifier * p_complete_member = p_complete_member = get_complete_struct_or_union_specifier(t.struct_or_union_specifier);
                                                    char name2[100];
                                                    snprintf(name2, sizeof name2, "%p", (void *)p_complete_member);
                                                    register_struct_types_and_functions(ctx, &t);
                                                    struct map_entry * p2 = hashmap_find(&ctx->structs_map, name2);
                                                    if (p2 != ((void *)0))
                                                    {
                                                        struct_entry_list_push_back(&p_struct_entry->dependencies, p2->data.p_struct_entry);
                                                    }
                                                }
                                                if (type_is_array(&t))
                                                {
                                                    struct type  t = get_array_item_type(&t);
                                                    if (type_is_struct_or_union(&t))
                                                    {
                                                        struct struct_or_union_specifier * p_complete_member = p_complete_member = get_complete_struct_or_union_specifier(t.struct_or_union_specifier);
                                                        char name2[100];
                                                        snprintf(name2, sizeof name2, "%p", (void *)p_complete_member);
                                                        register_struct_types_and_functions(ctx, &t);
                                                        struct map_entry * p2 = hashmap_find(&ctx->structs_map, name2);
                                                        if (p2 != ((void *)0))
                                                        {
                                                            struct_entry_list_push_back(&p_struct_entry->dependencies, p2->data.p_struct_entry);
                                                        }
                                                    }
                                                    else
                                                    {
                                                        register_struct_types_and_functions(ctx, &t);
                                                    }
                                                    type_destroy(&t);
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
                break;
                case 1 :
                {
                    struct param * pa = p_type->params.head;
                    while (pa)
                    {
                        register_struct_types_and_functions(ctx, &pa->type);
                        pa = pa->next;
                    }
                }
                break;
                case 2 :
                case 3 :
                break;
            }
            p_type = p_type->next;
        }
    }
    else _catch_label_1:
    {
    }
}

unsigned char  struct_or_union_specifier_is_union(struct struct_or_union_specifier * p);
struct enum_specifier *get_complete_enum_specifier(struct enum_specifier * p_enum_specifier);
unsigned char  print_type_specifier_flags(struct osstream * ss, unsigned char  * first, int   e_type_specifier_flags);

static void d_print_type_core(struct d_visit_ctx * ctx, struct osstream * ss, struct type * p_type0, char * name_opt)
{
    struct type * p_type = p_type0;
    unsigned char   previous_was_pointer_to = 0;
    while (p_type)
    {
        switch (p_type->category)
        {
            case 0 :
            {
                struct osstream  local = {0};
                unsigned char   first = 1;
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
                        struct enum_specifier * p_enum_specifier = get_complete_enum_specifier(p_type->enum_specifier);
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
                    name_opt = ((void *)0);
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
                ss_close(&local2);
            }
            previous_was_pointer_to = 0;
            break;
            case 2 :
            if (name_opt)
            {
                ss_fprintf(ss, "%s", name_opt);
                name_opt = ((void *)0);
            }
            ss_fprintf(ss, "[");
            unsigned char   b = 1;
            if (p_type->num_of_elements > 0)
            {
                if ( !b)
                {
                    ss_fprintf(ss, " ");
                }
                ss_fprintf(ss, "%d", p_type->num_of_elements);
            }
            ss_fprintf(ss, "]");
            previous_was_pointer_to = 0;
            break;
            case 1 :
            if (name_opt)
            {
                ss_fprintf(ss, "%s", name_opt);
                name_opt = ((void *)0);
            }
            ss_fprintf(ss, "(");
            struct param * pa = p_type->params.head;
            while (pa)
            {
                struct osstream  sslocal = {0};
                struct osstream  local2 = {0};
                d_print_type_core(ctx, &local2, &pa->type, pa->type.name_opt);
                ss_fprintf(&sslocal, "%s", local2.c_str);
                ss_fprintf(ss, "%s", sslocal.c_str);
                if (pa->next)
                {
                    ss_fprintf(ss, ", ");
                }
                ss_close(&sslocal);
                pa = pa->next;
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
            previous_was_pointer_to = 0;
            break;
            case 3 :
            {
                struct osstream  local = {0};
                if (p_type->next && ((p_type->next->category == 1 || p_type->next->category == 2)))
                {
                    ss_fprintf(&local, "(");
                }
                ss_fprintf(&local, "*");
                unsigned char   first = 0;
                if (name_opt)
                {
                    if ( !first)
                    {
                        ss_fprintf(ss, " ");
                    }
                    ss_fprintf(ss, "%s", name_opt);
                    name_opt = ((void *)0);
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
                previous_was_pointer_to = 1;
            }
            break;
        }
        p_type = p_type->next;
    }
}

static void d_print_type(struct d_visit_ctx * ctx, struct osstream * ss, struct type * p_type, char * name_opt)
{
    register_struct_types_and_functions(ctx, p_type);
    struct osstream  local = {0};
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
    if (object->members != ((void *)0))
    {
        struct object * member = object->members;
        while (member)
        {
            if ( !is_all_zero(member))
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
            if (object->p_init_expression->object.value.bool_value != 0)
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
        if ( !( *first))
        {
            ss_fprintf(ss, ", ");
        }
         *first = 0;
        il_visit_literal_string(object->p_init_expression->first_token, ss);
        return;
    }
    if (object->members != ((void *)0))
    {
        if (type_is_union(&object->type2))
        {
            struct object * member = object->members;
            object_print_constant_initialization(ctx, ss, member, first);
        }
        else
        {
            struct object * member = object->members;
            while (member)
            {
                object_print_constant_initialization(ctx, ss, member, first);
                member = member->next;
            }
        }
    }
    else
    {
        if ( !( *first))
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

static void object_print_non_constant_initialization(struct d_visit_ctx * ctx, struct osstream * ss, struct object * object, char * declarator_name)
{
    if (object_is_reference(object))
    {
        object = object_get_referenced(object);
    }
    if (object->members != ((void *)0))
    {
        if (type_is_union(&object->type2))
        {
            struct object * member = object->members;
            if (member->p_init_expression && object_has_constant_value(&member->p_init_expression->object))
            {
            }
            else
            {
                while (member)
                {
                    if (member->p_init_expression)
                    {
                        print_identation_core(ss, ctx->indentation);
                        ss_fprintf(ss, "%s%s = ", declarator_name, member->debug_name);
                        struct osstream  local = {0};
                        d_visit_expression(ctx, &local, member->p_init_expression);
                        ss_fprintf(ss, "%s", local.c_str);
                        ss_fprintf(ss, ";\n");
                        ss_close(&local);
                        break;
                    }
                    member = member->next;
                }
            }
        }
        else
        {
            struct object * member = object->members;
            while (member)
            {
                object_print_non_constant_initialization(ctx, ss, member, declarator_name);
                member = member->next;
            }
        }
    }
    else
    {
        if (object->p_init_expression)
        {
            if (object->p_init_expression->expression_type == 3)
            {
            }
            else
            {
                if ( !object_has_constant_value(&object->p_init_expression->object))
                {
                    print_identation_core(ss, ctx->indentation);
                    ss_fprintf(ss, "%s%s = ", declarator_name, object->debug_name);
                    struct osstream  local = {0};
                    d_visit_expression(ctx, &local, object->p_init_expression);
                    ss_fprintf(ss, "%s", local.c_str);
                    ss_fprintf(ss, ";\n");
                    ss_close(&local);
                }
            }
        }
    }
}

static void d_visit_init_declarator(struct d_visit_ctx * ctx, struct osstream * oss, struct init_declarator * p_init_declarator, unsigned char   binline)
{
    if (type_is_function(&p_init_declarator->p_declarator->type) && p_init_declarator->p_declarator->function_body == ((void *)0))
    {
    }
    else
    {
        if ((p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & 2) || (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & 1))
        {
        }
        else
        {
            print_identation(ctx, oss);
            if (binline)
            {
                ss_fprintf(oss, "__inline ");
            }
            struct osstream  ss = {0};
            d_print_type(ctx, &ss, &p_init_declarator->p_declarator->type, p_init_declarator->p_declarator->name_opt->lexeme);
            ss_fprintf(oss, "%s", ss.c_str);
            if (p_init_declarator->initializer)
            {
                ss_fprintf(oss, " = ");
                if (p_init_declarator->initializer->assignment_expression)
                {
                    d_visit_expression(ctx, oss, p_init_declarator->initializer->assignment_expression);
                    ss_fprintf(oss, ";\n");
                }
                else
                {
                    if (p_init_declarator->initializer->braced_initializer)
                    {
                        if (p_init_declarator->initializer->braced_initializer->initializer_list && p_init_declarator->initializer->braced_initializer->initializer_list->head)
                        {
                            if (is_all_zero(&p_init_declarator->p_declarator->object))
                            {
                                ss_fprintf(oss, "{0};\n");
                            }
                            else
                            {
                                unsigned char   first = 1;
                                ss_fprintf(oss, "{");
                                object_print_constant_initialization(ctx, oss, &p_init_declarator->p_declarator->object, &first);
                                ss_fprintf(oss, "}");
                                ss_fprintf(oss, ";\n");
                                object_print_non_constant_initialization(ctx, oss, &p_init_declarator->p_declarator->object, p_init_declarator->p_declarator->name_opt->lexeme);
                            }
                        }
                        else
                        {
                            ss_fprintf(oss, "{0};\n");
                        }
                    }
                }
            }
            else
            {
                if (p_init_declarator->p_declarator->function_body == ((void *)0))
                {
                    ss_fprintf(oss, ";\n");
                }
            }
            if (p_init_declarator->p_declarator->function_body)
            {
                struct hash_item_set  i = {0};
                i.number = 1;
                hashmap_set(&ctx->function_map, p_init_declarator->p_declarator->name_opt->lexeme, &i);
                ss_fprintf(oss, "\n");
                d_visit_compound_statement(ctx, oss, p_init_declarator->p_declarator->function_body);
                ss_fprintf(oss, "\n");
            }
        }
    }
}

static void d_visit_init_declarator_list(struct d_visit_ctx * ctx, struct osstream * oss, struct init_declarator_list * p_init_declarator_list, unsigned char   binline)
{
    struct init_declarator * p_init_declarator = p_init_declarator_list->head;
    while (p_init_declarator)
    {
        d_visit_init_declarator(ctx, oss, p_init_declarator, binline);
        p_init_declarator = p_init_declarator->next;
    }
}

static void d_visit_declaration(struct d_visit_ctx * ctx, struct osstream * oss, struct declaration * p_declaration)
{
    unsigned char   binline = 0;
    if (p_declaration->declaration_specifiers && p_declaration->declaration_specifiers->head)
    {
        struct declaration_specifier * p = p_declaration->declaration_specifiers->head;
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
        if ( !binline)
        {
            d_visit_init_declarator_list(ctx, oss, &p_declaration->init_declarator_list, binline);
        }
    }
}

void print_complete_struct(struct d_visit_ctx * ctx, struct osstream * ss, struct struct_or_union_specifier * p_struct_or_union_specifier)
{
    struct struct_or_union_specifier * p_complete = get_complete_struct_or_union_specifier2(p_struct_or_union_specifier);
    struct member_declaration * member_declaration = p_complete->member_declaration_list.head;
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
    int no_name_index = 0;
    while (member_declaration)
    {
        struct member_declarator * member_declarator = ((void *)0);
        if (member_declaration->member_declarator_list_opt)
        {
            member_declarator = member_declaration->member_declarator_list_opt->head;
            while (member_declarator)
            {
                if (member_declarator->declarator->name_opt)
                {
                    ss_fprintf(ss, "    ");
                    d_print_type(ctx, ss, &member_declarator->declarator->type, member_declarator->declarator->name_opt->lexeme);
                    ss_fprintf(ss, ";\n");
                }
                member_declarator = member_declarator->next;
            }
        }
        else
        {
            if (member_declaration->specifier_qualifier_list != ((void *)0))
            {
                if (member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                {
                    struct type  t = {0};
                    t.category = 0;
                    t.struct_or_union_specifier = member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                    t.type_specifier_flags = 32768;
                    char name[100];
                    snprintf(name, sizeof name, "__m%d", no_name_index++);
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

void print_complete_structs(struct d_visit_ctx * ctx, struct osstream * ss, struct struct_entry * p_struct_entry)
{
    if (p_struct_entry->done)
    {
        return;
    }
    {
        int i = 0;
        for (; i < p_struct_entry->dependencies.size; i++)
        {
            struct struct_entry * p_struct_entry_item = p_struct_entry->dependencies.data[i];
            print_complete_structs(ctx, ss, p_struct_entry_item);
            p_struct_entry_item = p_struct_entry_item->next;
        }
    }
    if ( !p_struct_entry->done)
    {
        p_struct_entry->done = 1;
        struct osstream  local = {0};
        print_complete_struct(ctx, &local, p_struct_entry->p_struct_or_union_specifier);
        ss_fprintf(ss, local.c_str);
        ss_close(&local);
    }
}

void d_visit(struct d_visit_ctx * ctx, struct osstream * oss)
{
    struct osstream  declarations = {0};
    ctx->indentation = 0;
    struct declaration * p_declaration = ctx->ast.declaration_list.head;
    while (p_declaration)
    {
        ss_clear(&ctx->add_this_before_external_decl);
        ss_clear(&ctx->function_types);
        struct osstream  declaration = {0};
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
        int i = 0;
        for (; i < ctx->structs_map.capacity; i++)
        {
            struct map_entry * entry = ctx->structs_map.table[i];
            while (entry)
            {
                print_complete_structs(ctx, oss, entry->data.p_struct_entry);
                entry = entry->next;
            }
        }
    }
    ss_fprintf(oss, "\n");
    if (declarations.c_str)
    {
        ss_fprintf(oss, "%s", declarations.c_str);
    }
    ss_close(&declarations);
}

