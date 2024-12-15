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

struct defer_statement {
    struct token * first_token;
    struct token * last_token;
    struct secondary_block * secondary_block;
};

struct secondary_block {
    struct token * first_token;
    struct token * last_token;
    struct statement * statement;
};

struct attribute_argument_clause {
    struct balanced_token_sequence * p_balanced_token_sequence;
    struct token * token;
};

struct hash_map {
    struct map_entry ** table;
    int capacity;
    int size;
};

struct argument_expression_list {
    struct argument_expression * head;
    struct argument_expression * tail;
};

struct initializer_list {
    struct token * first_token;
    struct initializer_list_item * head;
    struct initializer_list_item * tail;
    int size;
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

struct generic_association {
    struct type  type;
    struct type_name * p_type_name;
    struct expression * expression;
    struct token * first_token;
    struct token * last_token;
    struct generic_association * next;
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

struct attribute {
    int   attributes_flags;
    struct attribute_token * attribute_token;
    struct attribute_argument_clause * attribute_argument_clause;
    struct attribute * next;
};

struct generic_assoc_list {
    struct generic_association * head;
    struct generic_association * tail;
};

struct defer_list {
    struct defer_list_item * head;
    struct defer_list_item * tail;
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

union _struct_tag_0 {
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
    union _struct_tag_0  value;
    struct object * parent;
    struct expression * p_init_expression;
    struct object * members;
    struct object * next;
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

struct init_statement {
    struct expression_statement * p_expression_statement;
    struct simple_declaration * p_simple_declaration;
};

struct label {
    struct expression * constant_expression;
    struct token * p_identifier_opt;
    struct token * p_first_token;
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

struct parameter_type_list {
    unsigned char   is_var_args;
    unsigned char   is_void;
    struct parameter_list * parameter_list;
};

union _struct_tag_1 {
    unsigned int number;
    struct enum_specifier * p_enum_specifier;
    struct enumerator * p_enumerator;
    struct struct_or_union_specifier * p_struct_or_union_specifier;
    struct declarator * p_declarator;
    struct init_declarator * p_init_declarator;
    struct macro * p_macro;
    struct struct_entry * p_struct_entry;
};

struct block_item {
    struct token * first_token;
    struct declaration * declaration;
    struct unlabeled_statement * unlabeled_statement;
    struct label * label;
    struct block_item * next;
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

struct defer_list_item {
    struct declarator * declarator;
    struct defer_statement * defer_statement;
    struct defer_list_item * next;
};

struct type_specifier_qualifier {
    struct type_specifier * type_specifier;
    struct type_qualifier * type_qualifier;
    struct alignment_specifier * alignment_specifier;
    struct type_specifier_qualifier * next;
};

struct expression_statement {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct expression * expression_opt;
};

struct try_statement {
    struct secondary_block * secondary_block;
    struct secondary_block * catch_secondary_block_opt;
    struct token * first_token;
    struct token * last_token;
    struct token * catch_token_opt;
    int try_catch_block_index;
};

struct __crt_multibyte_data;
struct pragma_declaration {
    struct token * first_token;
    struct token * last_token;
};

struct attribute_specifier_sequence {
    struct token * first_token;
    struct token * last_token;
    int   attributes_flags;
    struct attribute_specifier * head;
    struct attribute_specifier * tail;
};

struct parameter_declaration {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct declaration_specifiers * declaration_specifiers;
    struct declarator * declarator;
    struct parameter_declaration * next;
};

struct enumerator_list {
    struct enumerator * head;
    struct enumerator * tail;
};

struct attribute_token {
    int   attributes_flags;
    struct token * token;
};

struct static_assert_declaration {
    struct token * first_token;
    struct token * last_token;
    struct expression * constant_expression;
    struct token * string_literal_opt;
};

struct jump_statement {
    struct token * label;
    struct token * first_token;
    struct token * last_token;
    struct expression * expression_opt;
    int try_catch_block_index;
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

struct direct_declarator {
    struct token * name_opt;
    struct declarator * declarator;
    struct array_declarator * array_declarator;
    struct function_declarator * function_declarator;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
};

struct designation {
    struct designator_list * designator_list;
    struct token * token;
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

struct labeled_statement {
    struct label * label;
    struct statement * statement;
};

struct __crt_locale_data;
struct typeof_specifier {
    struct token * first_token;
    struct token * last_token;
    struct typeof_specifier_argument * typeof_specifier_argument;
    struct type  type;
};

struct condition {
    struct expression * expression;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator * p_init_declarator;
    struct token * first_token;
    struct token * last_token;
};

struct generic_selection {
    struct expression * expression;
    struct type_name * type_name;
    struct expression * p_view_selected_expression;
    struct generic_assoc_list  generic_assoc_list;
    struct token * first_token;
    struct token * last_token;
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

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
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

struct scope {
    int scope_level;
    struct hash_map  tags;
    struct hash_map  variables;
    struct scope * next;
    struct scope * previous;
};

struct braced_initializer {
    struct token * first_token;
    struct token * last_token;
    struct initializer_list * initializer_list;
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

struct _iobuf {
    void * _Placeholder;
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

struct argument_expression {
    struct expression * expression;
    struct argument_expression * next;
    unsigned char   set_unkown;
};

struct balanced_token {
    struct token * token;
    struct balanced_token * next;
};

struct macro;
struct array_declarator {
    struct direct_declarator * direct_declarator;
    struct expression * assignment_expression;
    struct expression * expression;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * token;
    struct token * static_token_opt;
};

struct atomic_type_specifier {
    struct token * token;
    struct type_name * type_name;
};

struct parameter_list {
    struct parameter_declaration * head;
    struct parameter_declaration * tail;
};

struct member_declaration {
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct member_declarator_list * member_declarator_list_opt;
    struct static_assert_declaration * static_assert_declaration;
    struct pragma_declaration * pragma_declaration;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct member_declaration * next;
};

struct initializer {
    struct token * first_token;
    struct braced_initializer * braced_initializer;
    struct expression * assignment_expression;
};

struct attribute_list {
    int   attributes_flags;
    struct attribute * head;
    struct attribute * tail;
};

struct initializer_list_item {
    struct designation * designation;
    struct initializer * initializer;
    struct initializer_list_item * next;
};

struct typeof_specifier_argument {
    struct expression * expression;
    struct type_name * type_name;
};

struct type_qualifier_list {
    int   flags;
    struct type_qualifier * head;
    struct type_qualifier * tail;
};

struct designator_list {
    struct designator * head;
    struct designator * tail;
};

struct member_declarator {
    struct declarator * declarator;
    struct expression * constant_expression;
    struct member_declarator * next;
};

struct simple_declaration {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator_list  init_declarator_list;
    struct token * first_token;
    struct token * last_token;
};

struct struct_entry;
struct function_specifier {
    int   flags;
    struct token * token;
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

struct init_declarator {
    unsigned char   has_shared_ownership;
    struct declarator * p_declarator;
    struct initializer * initializer;
    struct init_declarator * next;
};

struct type_qualifier {
    int   flags;
    struct token * token;
    struct type_qualifier * next;
};

struct alignment_specifier {
    struct type_name * type_name;
    struct expression * constant_expression;
    struct token * token;
};

struct balanced_token_sequence {
    struct balanced_token * head;
    struct balanced_token * tail;
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

struct function_declarator {
    struct direct_declarator * direct_declarator;
    struct scope  parameters_scope;
    struct parameter_type_list * parameter_type_list_opt;
};

struct param {
    struct type  type;
    struct param * next;
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

struct map_entry {
    struct map_entry * next;
    unsigned int hash;
    char * key;
    int   type;
    union _struct_tag_1  data;
};

struct declaration_specifier {
    struct storage_class_specifier * storage_class_specifier;
    struct type_specifier_qualifier * type_specifier_qualifier;
    struct function_specifier * function_specifier;
    struct declaration_specifier * next;
};


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
                if (a < ( -9223372036854775807LL - 1) + b)
                {
                    return 0;
                }
            }
            else
            {
                if (b == ( -9223372036854775807LL - 1))
                {
                    return 0;
                }
                if (a > 9223372036854775807LL - ( -b))
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
            if (a == ( -9223372036854775807LL - 1) || b == ( -9223372036854775807LL - 1))
            {
                return 0;
            }
            if (a < ( -9223372036854775807LL - 1) - b)
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
            if (a == ( -9223372036854775807LL - 1) || b == ( -9223372036854775807LL - 1))
            {
                return 0;
            }
            if ( -a > 9223372036854775807LL /  -b)
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
                if (a < ( -9223372036854775807LL - 1) / b)
                {
                    return 0;
                }
            }
            else
            {
                if (b < ( -9223372036854775807LL - 1) / a)
                {
                    return 0;
                }
            }
        }
    }
     *result = a * b;
    return 1;
}

void free(void * ptr);

void object_delete(struct object * p)
{
    if (p)
    {
        free(p->debug_name);
        free(p);
    }
}

struct object *object_get_referenced(struct object * p_object);

unsigned char  object_has_constant_value(struct object * a)
{
    a = object_get_referenced(a);
    return a->state == 3;
}

int snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...);

void object_to_string(struct object * a, char buffer[], int sz)
{
    a = object_get_referenced(a);
    buffer[0] = 0;
    switch (a->value_type)
    {
        case 2 :
        snprintf(buffer, sz, "%s", a->value.signed_char_value ? "true" : "false");
        break;
        case 3 :
        snprintf(buffer, sz, "%c", a->value.signed_char_value);
        break;
        case 4 :
        snprintf(buffer, sz, "%c", a->value.unsigned_char_value);
        break;
        case 5 :
        snprintf(buffer, sz, "%c", a->value.signed_short_value);
        break;
        case 6 :
        snprintf(buffer, sz, "%c", a->value.signed_short_value);
        break;
        case 0 :
        snprintf(buffer, sz, "%d", a->value.signed_int_value);
        break;
        case 1 :
        snprintf(buffer, sz, "%u", a->value.signed_int_value);
        break;
        case 7 :
        snprintf(buffer, sz, "%ld", a->value.signed_long_value);
        break;
        case 8 :
        break;
        case 9 :
        snprintf(buffer, sz, "%lud", a->value.signed_long_value);
        break;
        case 10 :
        snprintf(buffer, sz, "%llu", a->value.signed_long_long_value);
        break;
        case 11 :
        snprintf(buffer, sz, "%f", a->value.float_value);
        break;
        case 12 :
        snprintf(buffer, sz, "%f", a->value.double_value);
        break;
        case 13 :
        snprintf(buffer, sz, "%Lf", a->value.long_double_value);
        break;
    }
}

struct object object_make_size_t(unsigned int value)
{
    struct object  r = {0};
    r.state = 3;
    r.value_type = 1;
    r.value.unsigned_int_value = value;
    return r;
}

struct object object_make_nullptr()
{
    struct object  r = {0};
    r.state = 3;
    r.value_type = 0;
    r.value.signed_short_value = 0;
    return r;
}

struct object object_make_wchar_t(unsigned short value)
{
    struct object  r = {0};
    r.state = 3;
    r.value_type = 5;
    r.value.signed_short_value = value;
    return r;
}

struct object object_make_bool(unsigned char   value)
{
    struct object  r = {0};
    r.state = 3;
    r.value_type = 2;
    r.value.bool_value = value;
    return r;
}

unsigned char  object_to_bool(struct object * a)
{
    a = object_get_referenced(a);
    switch (a->value_type)
    {
        case 2 :
        return a->value.bool_value;
        case 3 :
        return a->value.signed_char_value;
        case 4 :
        return a->value.unsigned_char_value;
        case 5 :
        return a->value.signed_short_value;
        case 6 :
        return a->value.unsigned_short_value;
        case 0 :
        return a->value.signed_int_value;
        case 1 :
        return a->value.unsigned_int_value;
        case 7 :
        return a->value.signed_long_value;
        case 8 :
        return a->value.unsigned_long_value;
        case 9 :
        return a->value.signed_long_long_value;
        case 10 :
        return a->value.unsigned_long_long_value;
        case 11 :
        return a->value.float_value;
        case 12 :
        return a->value.double_value;
        case 13 :
        return a->value.long_double_value;
    }
    ;
    return 0;
}

struct object object_make_signed_char(signed char value)
{
    struct object  r = {0};
    r.state = 3;
    r.value_type = 3;
    r.value.signed_char_value = value;
    return r;
}

int object_increment_value(struct object * a)
{
    a = object_get_referenced(a);
    switch (a->value_type)
    {
        case 2 :
        return a->value.bool_value++;
        case 3 :
        return a->value.signed_char_value++;
        case 4 :
        return a->value.unsigned_char_value++;
        case 5 :
        return a->value.signed_short_value++;
        case 6 :
        return a->value.unsigned_short_value++;
        case 0 :
        return a->value.signed_int_value++;
        case 1 :
        return a->value.unsigned_int_value++;
        case 7 :
        return a->value.signed_long_value++;
        case 8 :
        return a->value.unsigned_long_value++;
        case 9 :
        return a->value.signed_long_long_value++;
        case 10 :
        return a->value.unsigned_long_long_value++;
        case 11 :
        return a->value.float_value++;
        case 12 :
        return a->value.double_value++;
        case 13 :
        return a->value.long_double_value++;
    }
    ;
    return 0;
}

signed char object_to_signed_char(struct object * a)
{
    a = object_get_referenced(a);
    switch (a->value_type)
    {
        case 2 :
        return a->value.bool_value;
        case 3 :
        return a->value.signed_char_value;
        case 4 :
        return a->value.unsigned_char_value;
        case 5 :
        return a->value.signed_short_value;
        case 6 :
        return a->value.unsigned_short_value;
        case 0 :
        return a->value.signed_int_value;
        case 1 :
        return a->value.unsigned_int_value;
        case 7 :
        return a->value.signed_long_value;
        case 8 :
        return a->value.unsigned_long_value;
        case 9 :
        return a->value.signed_long_long_value;
        case 10 :
        return a->value.unsigned_long_long_value;
        case 11 :
        return a->value.float_value;
        case 12 :
        return a->value.double_value;
        case 13 :
        return a->value.long_double_value;
    }
    ;
    return 0;
}

struct object object_make_unsigned_char(unsigned char value)
{
    struct object  r = {0};
    r.state = 3;
    r.value_type = 4;
    r.value.unsigned_char_value = value;
    return r;
}

unsigned char object_to_unsigned_char(struct object * a)
{
    a = object_get_referenced(a);
    switch (a->value_type)
    {
        case 2 :
        return a->value.bool_value;
        case 3 :
        return a->value.signed_char_value;
        case 4 :
        return a->value.unsigned_char_value;
        case 5 :
        return a->value.signed_short_value;
        case 6 :
        return a->value.unsigned_short_value;
        case 0 :
        return a->value.signed_int_value;
        case 1 :
        return a->value.unsigned_int_value;
        case 7 :
        return a->value.signed_long_value;
        case 8 :
        return a->value.unsigned_long_value;
        case 9 :
        return a->value.signed_long_long_value;
        case 10 :
        return a->value.unsigned_long_long_value;
        case 11 :
        return a->value.float_value;
        case 12 :
        return a->value.double_value;
        case 13 :
        return a->value.long_double_value;
    }
    ;
    return 0;
}

struct object object_make_signed_short(signed short value)
{
    struct object  r = {0};
    r.state = 3;
    r.value_type = 5;
    r.value.signed_short_value = value;
    return r;
}

signed short object_to_signed_short(struct object * a)
{
    a = object_get_referenced(a);
    switch (a->value_type)
    {
        case 2 :
        return a->value.bool_value;
        case 3 :
        return a->value.signed_char_value;
        case 4 :
        return a->value.unsigned_char_value;
        case 5 :
        return a->value.signed_short_value;
        case 6 :
        return a->value.unsigned_short_value;
        case 0 :
        return a->value.signed_int_value;
        case 1 :
        return a->value.unsigned_int_value;
        case 7 :
        return a->value.signed_long_value;
        case 8 :
        return a->value.unsigned_long_value;
        case 9 :
        return a->value.signed_long_long_value;
        case 10 :
        return a->value.unsigned_long_long_value;
        case 11 :
        return a->value.float_value;
        case 12 :
        return a->value.double_value;
        case 13 :
        return a->value.long_double_value;
    }
    ;
    return 0;
}

struct object object_make_unsigned_short(unsigned short value)
{
    struct object  r = {0};
    r.state = 3;
    r.value_type = 6;
    r.value.unsigned_short_value = value;
    return r;
}

unsigned short object_to_unsigned_short(struct object * a)
{
    a = object_get_referenced(a);
    switch (a->value_type)
    {
        case 2 :
        return a->value.bool_value;
        case 3 :
        return a->value.signed_char_value;
        case 4 :
        return a->value.unsigned_char_value;
        case 5 :
        return a->value.signed_short_value;
        case 6 :
        return a->value.unsigned_short_value;
        case 0 :
        return a->value.signed_int_value;
        case 1 :
        return a->value.unsigned_int_value;
        case 7 :
        return a->value.signed_long_value;
        case 8 :
        return a->value.unsigned_long_value;
        case 9 :
        return a->value.signed_long_long_value;
        case 10 :
        return a->value.unsigned_long_long_value;
        case 11 :
        return a->value.float_value;
        case 12 :
        return a->value.double_value;
        case 13 :
        return a->value.long_double_value;
    }
    ;
    return 0;
}

struct object object_make_signed_int(signed int value)
{
    struct object  r = {0};
    r.state = 3;
    r.value_type = 0;
    r.value.signed_int_value = value;
    return r;
}

signed int object_to_signed_int(struct object * a)
{
    a = object_get_referenced(a);
    switch (a->value_type)
    {
        case 2 :
        return a->value.bool_value;
        case 3 :
        return a->value.signed_char_value;
        case 4 :
        return a->value.unsigned_char_value;
        case 5 :
        return a->value.signed_short_value;
        case 6 :
        return a->value.unsigned_short_value;
        case 0 :
        return a->value.signed_int_value;
        case 1 :
        return a->value.unsigned_int_value;
        case 7 :
        return a->value.signed_long_value;
        case 8 :
        return a->value.unsigned_long_value;
        case 9 :
        return a->value.signed_long_long_value;
        case 10 :
        return a->value.unsigned_long_long_value;
        case 11 :
        return a->value.float_value;
        case 12 :
        return a->value.double_value;
        case 13 :
        return a->value.long_double_value;
    }
    ;
    return 0;
}

struct object object_make_unsigned_int(unsigned int value)
{
    struct object  r = {0};
    r.state = 3;
    r.value_type = 1;
    r.value.unsigned_int_value = value;
    return r;
}

unsigned int object_to_unsigned_int(struct object * a)
{
    a = object_get_referenced(a);
    switch (a->value_type)
    {
        case 2 :
        return a->value.bool_value;
        case 3 :
        return a->value.signed_char_value;
        case 4 :
        return a->value.unsigned_char_value;
        case 5 :
        return a->value.signed_short_value;
        case 6 :
        return a->value.unsigned_short_value;
        case 0 :
        return a->value.signed_int_value;
        case 1 :
        return a->value.unsigned_int_value;
        case 7 :
        return a->value.signed_long_value;
        case 8 :
        return a->value.unsigned_long_value;
        case 9 :
        return a->value.signed_long_long_value;
        case 10 :
        return a->value.unsigned_long_long_value;
        case 11 :
        return a->value.float_value;
        case 12 :
        return a->value.double_value;
        case 13 :
        return a->value.long_double_value;
    }
    ;
    return 0;
}

struct object object_make_signed_long(signed long value)
{
    struct object  r = {0};
    r.state = 3;
    r.value_type = 7;
    r.value.signed_long_value = value;
    return r;
}

signed long object_to_signed_long(struct object * a)
{
    a = object_get_referenced(a);
    switch (a->value_type)
    {
        case 2 :
        return a->value.bool_value;
        case 3 :
        return a->value.signed_char_value;
        case 4 :
        return a->value.unsigned_char_value;
        case 5 :
        return a->value.signed_short_value;
        case 6 :
        return a->value.unsigned_short_value;
        case 0 :
        return a->value.signed_int_value;
        case 1 :
        return a->value.unsigned_int_value;
        case 7 :
        return a->value.signed_long_value;
        case 8 :
        return a->value.unsigned_long_value;
        case 9 :
        return a->value.signed_long_long_value;
        case 10 :
        return a->value.unsigned_long_long_value;
        case 11 :
        return a->value.float_value;
        case 12 :
        return a->value.double_value;
        case 13 :
        return a->value.long_double_value;
    }
    ;
    return 0;
}

struct object object_make_unsigned_long(unsigned long value)
{
    struct object  r = {0};
    r.state = 3;
    r.value_type = 8;
    r.value.unsigned_long_value = value;
    return r;
}

unsigned long object_to_unsigned_long(struct object * a)
{
    a = object_get_referenced(a);
    switch (a->value_type)
    {
        case 2 :
        return a->value.bool_value;
        case 3 :
        return a->value.signed_char_value;
        case 4 :
        return a->value.unsigned_char_value;
        case 5 :
        return a->value.signed_short_value;
        case 6 :
        return a->value.unsigned_short_value;
        case 0 :
        return a->value.signed_int_value;
        case 1 :
        return a->value.unsigned_int_value;
        case 7 :
        return a->value.signed_long_value;
        case 8 :
        return a->value.unsigned_long_value;
        case 9 :
        return a->value.signed_long_long_value;
        case 10 :
        return a->value.unsigned_long_long_value;
        case 11 :
        return a->value.float_value;
        case 12 :
        return a->value.double_value;
        case 13 :
        return a->value.long_double_value;
    }
    ;
    return 0;
}

struct object object_make_signed_long_long(signed long long value)
{
    struct object  r = {0};
    r.state = 3;
    r.value_type = 9;
    r.value.signed_long_long_value = value;
    return r;
}

signed long long object_to_signed_long_long(struct object * a)
{
    a = object_get_referenced(a);
    switch (a->value_type)
    {
        case 2 :
        return a->value.bool_value;
        case 3 :
        return a->value.signed_char_value;
        case 4 :
        return a->value.unsigned_char_value;
        case 5 :
        return a->value.signed_short_value;
        case 6 :
        return a->value.unsigned_short_value;
        case 0 :
        return a->value.signed_int_value;
        case 1 :
        return a->value.unsigned_int_value;
        case 7 :
        return a->value.signed_long_value;
        case 8 :
        return a->value.unsigned_long_value;
        case 9 :
        return a->value.signed_long_long_value;
        case 10 :
        return a->value.unsigned_long_long_value;
        case 11 :
        return a->value.float_value;
        case 12 :
        return a->value.double_value;
        case 13 :
        return a->value.long_double_value;
    }
    ;
    return 0;
}

struct object object_make_unsigned_long_long(unsigned long long value)
{
    struct object  r = {0};
    r.state = 3;
    r.value_type = 10;
    r.value.unsigned_long_long_value = value;
    return r;
}

unsigned long long object_to_unsigned_long_long(struct object * a)
{
    a = object_get_referenced(a);
    switch (a->value_type)
    {
        case 2 :
        return a->value.bool_value;
        case 3 :
        return a->value.signed_char_value;
        case 4 :
        return a->value.unsigned_char_value;
        case 5 :
        return a->value.signed_short_value;
        case 6 :
        return a->value.unsigned_short_value;
        case 0 :
        return a->value.signed_int_value;
        case 1 :
        return a->value.unsigned_int_value;
        case 7 :
        return a->value.signed_long_value;
        case 8 :
        return a->value.unsigned_long_value;
        case 9 :
        return a->value.signed_long_long_value;
        case 10 :
        return a->value.unsigned_long_long_value;
        case 11 :
        return a->value.float_value;
        case 12 :
        return a->value.double_value;
        case 13 :
        return a->value.long_double_value;
    }
    ;
    return 0;
}

struct object object_make_float(float value)
{
    struct object  r = {0};
    r.state = 3;
    r.value_type = 11;
    r.value.float_value = value;
    return r;
}

float object_to_float(struct object * a)
{
    a = object_get_referenced(a);
    switch (a->value_type)
    {
        case 2 :
        return a->value.bool_value;
        case 3 :
        return a->value.signed_char_value;
        case 4 :
        return a->value.unsigned_char_value;
        case 5 :
        return a->value.signed_short_value;
        case 6 :
        return a->value.unsigned_short_value;
        case 0 :
        return a->value.signed_int_value;
        case 1 :
        return a->value.unsigned_int_value;
        case 7 :
        return a->value.signed_long_value;
        case 8 :
        return a->value.unsigned_long_value;
        case 9 :
        return a->value.signed_long_long_value;
        case 10 :
        return a->value.unsigned_long_long_value;
        case 11 :
        return a->value.float_value;
        case 12 :
        return a->value.double_value;
        case 13 :
        return a->value.long_double_value;
    }
    ;
    return 0;
}

struct object object_make_double(double value)
{
    struct object  r = {0};
    r.state = 3;
    r.value_type = 12;
    r.value.double_value = value;
    return r;
}

double object_to_double(struct object * a)
{
    a = object_get_referenced(a);
    switch (a->value_type)
    {
        case 2 :
        return a->value.bool_value;
        case 3 :
        return a->value.signed_char_value;
        case 4 :
        return a->value.unsigned_char_value;
        case 5 :
        return a->value.signed_short_value;
        case 6 :
        return a->value.unsigned_short_value;
        case 0 :
        return a->value.signed_int_value;
        case 1 :
        return a->value.unsigned_int_value;
        case 7 :
        return a->value.signed_long_value;
        case 8 :
        return a->value.unsigned_long_value;
        case 9 :
        return a->value.signed_long_long_value;
        case 10 :
        return a->value.unsigned_long_long_value;
        case 11 :
        return a->value.float_value;
        case 12 :
        return a->value.double_value;
        case 13 :
        return a->value.long_double_value;
    }
    ;
    return 0;
}

struct object object_make_reference(struct object * object)
{
    struct object  r = {0};
    r.state = 4;
    r.value_type = 0;
    r.value.signed_int_value = 0;
    r.members = object_get_referenced(object);
    return r;
}

struct object object_make_long_double(long double value)
{
    struct object  r = {0};
    r.state = 3;
    r.value_type = 13;
    r.value.long_double_value = value;
    return r;
}

long double object_to_long_double(struct object * a)
{
    a = object_get_referenced(a);
    switch (a->value_type)
    {
        case 2 :
        return a->value.bool_value;
        case 3 :
        return a->value.signed_char_value;
        case 4 :
        return a->value.unsigned_char_value;
        case 5 :
        return a->value.signed_short_value;
        case 6 :
        return a->value.unsigned_short_value;
        case 0 :
        return a->value.signed_int_value;
        case 1 :
        return a->value.unsigned_int_value;
        case 7 :
        return a->value.signed_long_value;
        case 8 :
        return a->value.unsigned_long_value;
        case 9 :
        return a->value.signed_long_long_value;
        case 10 :
        return a->value.unsigned_long_long_value;
        case 11 :
        return a->value.float_value;
        case 12 :
        return a->value.double_value;
        case 13 :
        return a->value.long_double_value;
    }
    ;
    return 0;
}

struct object object_cast(int   t, struct object * v)
{
    v = object_get_referenced(v);
    if (v->value_type == t)
    {
        return  *v;
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
    struct object  empty = {0};
    return empty;
}

unsigned char  type_is_union(struct type * p_type);

void object_default_initialization(struct object * p_object, unsigned char   is_constant)
{
    if (p_object->members == ((void *)0))
    {
        if (is_constant)
        {
            p_object->state = 3;
        }
        else
        {
            p_object->state = 2;
        }
        p_object->value.unsigned_long_long_value = 0;
    }
    if (type_is_union(&p_object->type2))
    {
        struct object * p = p_object->members;
        if (p)
        {
            object_default_initialization(p, is_constant);
        }
    }
    else
    {
        struct object * p = p_object->members;
        while (p)
        {
            object_default_initialization(p, is_constant);
            p = p->next;
        }
    }
}

struct object *object_get_referenced(struct object * p_object)
{
    if (p_object->state == 4)
    {
        ;
        p_object = p_object->members;
    }
    ;
    return p_object;
}

unsigned char  object_is_derived(struct object * p_object)
{
    if (p_object->state == 4)
    {
        return 0;
    }
    return p_object->members != ((void *)0);
}

unsigned char  object_is_reference(struct object * p_object)
{
    return p_object->state == 4;
}

static void object_fix_parent(struct object * p_object, struct object * parent)
{
    struct object * it = p_object->members;
    while (it)
    {
        it->parent = parent;
        it = it->next;
    }
}

struct object *object_get_member(struct object * p_object, int index)
{
    p_object = (struct object *)object_get_referenced(p_object);
    if (p_object->members == ((void *)0))
    {
        return ((void *)0);
    }
    struct object * it = p_object->members;
    int count = 0;
    while (it)
    {
        if (index == count)
        {
            return it;
        }
        count++;
        it = it->next;
    }
    return ((void *)0);
}

void object_set(struct object * to, struct expression * p_init_expression, struct object * from, unsigned char   is_constant)
{
    from = object_get_referenced(from);
    to->p_init_expression = p_init_expression;
    if (object_is_derived(to))
    {
        struct object * it_to = to->members;
        struct object * it_from = from->members;
        while (it_from && it_to)
        {
            object_set(it_to, ((void *)0), it_from, is_constant);
            it_to = it_to->next;
            it_from = it_from->next;
        }
        if (it_from != ((void *)0) || it_to != ((void *)0))
        {
        }
    }
    else
    {
        ;
        to->state = from->state;
        to->value = object_cast(to->value_type, from).value;
        if ( !is_constant && to->state == 3)
        {
            to->state = 2;
        }
    }
}

void *calloc(int nmemb, unsigned int size);
char *strdup(char * src);
struct type type_dup(struct type * p_type);
struct type get_array_item_type(struct type * p_type);
void type_destroy(struct type * p_type);
int  type_to_object_type(struct type * type);
struct struct_or_union_specifier *get_complete_struct_or_union_specifier(struct struct_or_union_specifier * p_struct_or_union_specifier);

struct object *make_object_ptr_core(struct type * p_type, char * name)
{
    struct object * p_object = ((void *)0);
    if (1)
    {
        if (p_type->category == 1)
        {
            p_object = calloc(1, sizeof  *p_object);
            if (p_object == ((void *)0))
            {
                goto _catch_label_1;
            }
            p_object->debug_name = strdup(name);
            p_object->type2 = type_dup(p_type);
            return p_object;
        }
        if (p_type->category == 3)
        {
            p_object = calloc(1, sizeof  *p_object);
            if (p_object == ((void *)0))
            {
                goto _catch_label_1;
            }
             *p_object = object_make_nullptr();
            p_object->state = 0;
            ;
            p_object->debug_name = strdup(name);
            p_object->type2 = type_dup(p_type);
            return p_object;
        }
        if (p_type->category == 2)
        {
            p_object = calloc(1, sizeof  *p_object);
            if (p_object == ((void *)0))
            {
                goto _catch_label_1;
            }
            p_object->type2 = type_dup(p_type);
            p_object->debug_name = strdup(name);
            if (p_type->num_of_elements > 0)
            {
                struct type  array_item_type = get_array_item_type(p_type);
                struct object * p_tail_object = ((void *)0);
                {
                    int i = 0;
                    for (; i < p_type->num_of_elements; i++)
                    {
                        char buffer[200] = {0};
                        snprintf(buffer, sizeof buffer, "%s[%d]", name, i);
                        struct object * p_member_obj = make_object_ptr_core(&array_item_type, buffer);
                        if (p_member_obj == ((void *)0))
                        {
                            type_destroy(&array_item_type);
                            goto _catch_label_1;
                        }
                        p_member_obj->parent = p_object;
                        p_member_obj->debug_name = strdup(buffer);
                        if (p_tail_object == ((void *)0))
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
        if (p_type->struct_or_union_specifier == ((void *)0))
        {
            p_object = calloc(1, sizeof  *p_object);
            if (p_object == ((void *)0))
            {
                goto _catch_label_1;
            }
            p_object->state = 0;
            p_object->value_type = type_to_object_type(p_type);
            p_object->value.signed_long_long_value =  -1;
            p_object->debug_name = strdup(name);
            p_object->type2 = type_dup(p_type);
            return p_object;
        }
        struct struct_or_union_specifier * p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
        if (p_struct_or_union_specifier == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_object = calloc(1, sizeof  *p_object);
        if (p_object == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_object->debug_name = strdup(name);
        p_object->type2 = type_dup(p_type);
        struct object * p_last_member_obj = ((void *)0);
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
                        char buffer[200] = {0};
                        snprintf(buffer, sizeof buffer, "%s.%s", name, p_member_declarator->declarator->name_opt->lexeme);
                        struct object * p_member_obj = make_object_ptr_core(&p_member_declarator->declarator->type, buffer);
                        if (p_member_obj == ((void *)0))
                        {
                            goto _catch_label_1;
                        }
                        p_member_obj->parent = p_object;
                        if (p_object->members == ((void *)0))
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
                if (p_member_declaration->specifier_qualifier_list != ((void *)0))
                {
                    if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                    {
                        struct type  t = {0};
                        t.category = 0;
                        t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                        t.type_specifier_flags = 32768;
                        char buffer[200] = {0};
                        snprintf(buffer, sizeof buffer, ".%s", name);
                        struct object * p_member_obj = make_object_ptr_core(&t, buffer);
                        if (p_member_obj == ((void *)0))
                        {
                            goto _catch_label_1;
                        }
                        p_member_obj->parent = p_object;
                        if (p_last_member_obj == ((void *)0))
                        {
                            ;
                            p_object->members = p_member_obj;
                            p_last_member_obj = p_member_obj;
                        }
                        else
                        {
                            p_last_member_obj->next = p_member_obj;
                        }
                        type_destroy(&t);
                    }
                }
            }
            p_member_declaration = p_member_declaration->next;
        }
        return p_object;
    }
    else _catch_label_1:
    {
        object_delete(p_object);
        p_object = ((void *)0);
    }
    return ((void *)0);
}

struct object *make_object_ptr(struct type * p_type)
{
    return make_object_ptr_core(p_type, "");
}

int make_object(struct type * p_type, struct object * obj)
{
    struct object * p = make_object_ptr_core(p_type, "");
    if (p)
    {
         *obj =  *p;
        object_fix_parent(obj, obj);
        free(p);
    }
    return p == 0 ? 1 : 0;
}

int  type_specifier_to_object_type(int   type_specifier_flags)
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

int  type_to_object_type(struct type * type)
{
    if (type_is_pointer(type))
    {
        return 1;
    }
    return type_specifier_to_object_type(type->type_specifier_flags);
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

void object_print_value_debug(struct object * a)
{
    a = object_get_referenced(a);
    switch (a->value_type)
    {
        case 2 :
        printf("%s (bool)", a->value.bool_value ? "true" : "false");
        break;
        case 3 :
        printf("%d (signed char)", (int)a->value.signed_char_value);
        break;
        case 4 :
        printf("%d (unsigned char)", (int)a->value.unsigned_char_value);
        break;
        case 5 :
        printf("%d (short)", a->value.signed_short_value);
        break;
        case 6 :
        printf("%d (unsigned short)", a->value.unsigned_short_value);
        break;
        case 0 :
        printf("%d (int)", a->value.signed_int_value);
        break;
        case 1 :
        printf("%du (unsigned int)", a->value.unsigned_int_value);
        break;
        case 7 :
        printf("%ld (long)", a->value.signed_long_value);
        break;
        case 8 :
        printf("%lu (unsigned long)", a->value.unsigned_long_value);
        break;
        case 9 :
        printf("%lld (long long)", a->value.signed_long_long_value);
        break;
        case 10 :
        printf("%llu (unsigned long long)", a->value.unsigned_long_long_value);
        break;
        case 11 :
        printf("%f (float)", a->value.float_value);
        break;
        case 12 :
        printf("%lf (double)", a->value.double_value);
        break;
        case 13 :
        printf("%Lf (long double)", a->value.long_double_value);
        break;
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
        int i = 0;
        for (; i < n; i++)
        printf("  ");
    }
    if (object->members != ((void *)0))
    {
        type_print(&object->type2);
        printf(" {\n");
        struct object * member = object->members;
        while (member)
        {
            object_print_to_debug_core(member, n + 1);
            member = member->next;
        }
        {
            int i = 0;
            for (; i < n; i++)
            printf("  ");
        }
        printf("}\n");
    }
    else
    {
        type_print(&object->type2);
        printf(" = ");
        object_print_value_debug(object);
        switch (object->state)
        {
            case 0 :
            printf(" uninitialized ");
            break;
            case 1 :
            printf(" unknown ");
            break;
            case 2 :
            printf(" exact ");
            break;
            case 3 :
            printf(" constant_exact ");
            break;
            case 4 :
            ;
            break;
        }
        printf("\n");
    }
}

void object_print_to_debug(struct object * object)
{
    int n = 0;
    object_print_to_debug_core(object, n);
}

struct object *object_extend_array_to_index(struct type * p_type, struct object * a, int max_index, unsigned char   is_constant)
{
    struct object * it = a->members;
    if (1)
    {
        int count = 0;
        while (it)
        {
            count++;
            if (it->next == ((void *)0))
            {
                break;
            }
            it = it->next;
        }
        while (count < (max_index + 1))
        {
            if (it == ((void *)0))
            {
                ;
                a->members = make_object_ptr(p_type);
                if (a->members == ((void *)0))
                {
                    goto _catch_label_1;
                }
                object_default_initialization(a->members, is_constant);
                it = a->members;
                it->parent = a;
                count++;
            }
            else
            {
                struct object * p = make_object_ptr(p_type);
                if (p == ((void *)0))
                {
                    goto _catch_label_1;
                }
                p->parent = a;
                object_default_initialization(p, is_constant);
                ;
                it->next = p;
                it = p;
                count++;
            }
        }
    }
    else _catch_label_1:
    {
    }
    return it;
}

