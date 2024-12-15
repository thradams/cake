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

struct expression2;
struct struct_or_union_specifier0;
struct declarator;
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

struct struct_entry;
struct hash_map {
    struct map_entry ** table;
    int capacity;
    int size;
};

struct struct_or_union_specifier;
struct param_list {
    unsigned char   is_var_args;
    unsigned char   is_void;
    struct param * head;
    struct param * tail;
};

struct include_dir_list {
    struct include_dir * head;
    struct include_dir * tail;
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

struct token_list {
    struct token * head;
    struct token * tail;
};

struct preprocessor_ctx {
    struct options  options;
    int   flags;
    struct hash_map  macros;
    struct include_dir_list  include_dir;
    struct hash_map  pragma_once_map;
    struct token * current;
    struct token_list  input_list;
    unsigned int count_macro_value;
    unsigned char   conditional_inclusion;
    int n_warnings;
    int n_errors;
};

struct expression;
struct enumerator;
struct pre_expression_ctx {
    long long value;
};

struct enum_specifier;
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

struct type {
    int   category;
    int   attributes_flags;
    int   type_specifier_flags;
    int   type_qualifier_flags;
    int   storage_class_specifier_flags;
    char * name_opt;
    struct struct_or_union_specifier0 * struct_or_union_specifier;
    struct enum_specifier1 * enum_specifier;
    struct expression * array_num_elements_expression;
    int num_of_elements;
    unsigned char   static_array;
    unsigned char   address_of;
    struct param_list  params;
    struct type * next;
};

struct object {
    int   state;
    int   value_type;
    struct type  type2;
    char * debug_name;
    union _struct_tag_6  value;
    struct object * parent;
    struct expression2 * p_init_expression;
    struct object * members;
    struct object * next;
};

struct include_dir {
    char * path;
    struct include_dir * next;
};

struct macro;
struct init_declarator;
struct param {
    struct type  type;
    struct param * next;
};

struct enum_specifier1;

int  parse_number(char * lexeme, char suffix[4], char erromsg[100]);
unsigned char  preprocessor_diagnostic_message(int   w, struct preprocessor_ctx * ctx, struct token * p_token, char * fmt, ...);
unsigned long long strtoull(char * _String, char ** _EndPtr, int _Radix);
int *_errno(void);
struct object object_make_unsigned_int(unsigned int value);
struct object object_make_unsigned_long(unsigned long value);
struct object object_make_unsigned_long_long(unsigned long long value);
struct object object_make_signed_int(signed int value);
struct object object_make_signed_long(signed long value);
struct object object_make_signed_long_long(signed long long value);
signed long long object_to_signed_long_long(struct object * a);

static int ppnumber_to_longlong(struct preprocessor_ctx * ctx, struct token * token, long long * result)
{
    int c = 0;
    char buffer[260] = {0};
    char * s = token->lexeme;
    while ( *s)
    {
        if ( *s != 39)
        {
            buffer[c] =  *s;
            c++;
        }
        s++;
    }
    char errormsg[100];
    char suffix[4] = {0};
    int   type = parse_number(token->lexeme, suffix, errormsg);
    if (type == 0)
    {
        preprocessor_diagnostic_message(1380, ctx, token, "%s", errormsg);
        return 0;
    }
    struct object  cv = {0};
    switch (type)
    {
        case 136 :
        case 137 :
        case 138 :
        case 139 :
        {
            unsigned long long value = 0;
            switch (type)
            {
                case 136 :
                value = strtoull(buffer, ((void *)0), 10);
                break;
                case 137 :
                value = strtoull(buffer + 1, ((void *)0), 8);
                break;
                case 138 :
                value = strtoull(buffer + 2, ((void *)0), 16);
                break;
                case 139 :
                value = strtoull(buffer + 2, ((void *)0), 2);
                break;
                default:
                break;
            }
            if (value == 18446744073709551615ULL && ( *_errno()) == 34)
            {
            }
            if (suffix[0] == 85)
            {
                if (value <= 4294967295LL && suffix[1] != 76)
                {
                    cv = object_make_unsigned_int((unsigned int)value);
                }
                else
                {
                    if (value <= 4294967295UL && suffix[2] != 76)
                    {
                        cv = object_make_unsigned_long((unsigned long)value);
                    }
                    else
                    {
                        cv = object_make_unsigned_long_long((unsigned long long)value);
                    }
                }
            }
            else
            {
                if (value <= 2147483647 && suffix[0] != 76)
                {
                    cv = object_make_signed_int((int)value);
                }
                else
                {
                    if (value <= 2147483647L && suffix[1] != 76)
                    {
                        cv = object_make_signed_long((long)value);
                    }
                    else
                    {
                        if (value <= 9223372036854775807LL)
                        {
                            cv = object_make_signed_long_long((long long)value);
                        }
                        else
                        {
                            cv = object_make_signed_long_long(value);
                        }
                    }
                }
            }
        }
        break;
        case 140 :
        case 141 :
        break;
        default:
        ;
    }
     *result = object_to_signed_long_long(&cv);
    return 0;
}

unsigned char  token_is_blank(struct token * p);

static struct token *pre_match(struct preprocessor_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return ((void *)0);
    }
    ctx->current = ctx->current->next;
    while (ctx->current && token_is_blank(ctx->current))
    {
        ctx->current = ctx->current->next;
    }
    return ctx->current;
}

unsigned char *utf8_decode(unsigned char * s, unsigned int * c);
unsigned char *escape_sequences_decode_opt(unsigned char * p, unsigned int * out_value);
int snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...);
struct object object_make_wchar_t(unsigned short value);

static struct object char_constant_to_value(char * s, char error_message[], int error_message_sz_bytes)
{
    error_message[0] = 0;
    unsigned char * p = (unsigned char *)s;
    if (1)
    {
        if (p[0] == 117 && p[1] == 56)
        {
            p++;
            p++;
            p++;
            unsigned int c = 0;
            p = utf8_decode(p, &c);
            if (p == ((void *)0))
            {
                goto _catch_label_1;
            }
            if (c == 92)
            {
                p = escape_sequences_decode_opt(p, &c);
                if (p == ((void *)0))
                {
                    goto _catch_label_1;
                }
            }
            if ( *p != 39)
            {
                snprintf(error_message, error_message_sz_bytes, "Unicode character literals may not contain multiple characters.");
            }
            if (c > 128)
            {
                snprintf(error_message, error_message_sz_bytes, "Character too large for enclosing character literal type.");
            }
            return object_make_wchar_t((unsigned short)c);
        }
        else
        {
            if (p[0] == 117)
            {
                p++;
                p++;
                unsigned int c = 0;
                p = utf8_decode(p, &c);
                if (p == ((void *)0))
                {
                    goto _catch_label_1;
                }
                if (c == 92)
                {
                    p = escape_sequences_decode_opt(p, &c);
                    if (p == ((void *)0))
                    {
                        goto _catch_label_1;
                    }
                }
                if ( *p != 39)
                {
                    snprintf(error_message, error_message_sz_bytes, "Unicode character literals may not contain multiple characters.");
                }
                if (c > 65535)
                {
                    snprintf(error_message, error_message_sz_bytes, "Character too large for enclosing character literal type.");
                }
                return object_make_wchar_t((unsigned short)c);
            }
            else
            {
                if (p[0] == 85)
                {
                    p++;
                    p++;
                    unsigned int c = 0;
                    p = utf8_decode(p, &c);
                    if (p == ((void *)0))
                    {
                        goto _catch_label_1;
                    }
                    if (c == 92)
                    {
                        p = escape_sequences_decode_opt(p, &c);
                        if (p == ((void *)0))
                        {
                            goto _catch_label_1;
                        }
                    }
                    if ( *p != 39)
                    {
                        snprintf(error_message, error_message_sz_bytes, "Unicode character literals may not contain multiple characters.");
                    }
                    if (c > 4294967295LL)
                    {
                        snprintf(error_message, error_message_sz_bytes, "Character too large for enclosing character literal type.");
                    }
                    return object_make_wchar_t((unsigned short)c);
                }
                else
                {
                    if (p[0] == 76)
                    {
                        p++;
                        p++;
                        long long value = 0;
                        while ( *p != 39)
                        {
                            unsigned int c = 0;
                            p = utf8_decode(p, &c);
                            if (p == ((void *)0))
                            {
                                goto _catch_label_1;
                            }
                            if (c == 92)
                            {
                                p = escape_sequences_decode_opt(p, &c);
                                if (p == ((void *)0))
                                {
                                    goto _catch_label_1;
                                }
                            }
                            value = value * 256 + c;
                            if (value > 65535)
                            {
                                snprintf(error_message, error_message_sz_bytes, "character constant too long for its type");
                                break;
                            }
                        }
                        return object_make_wchar_t((unsigned short)value);
                    }
                    else
                    {
                        p++;
                        long long value = 0;
                        while ( *p != 39)
                        {
                            unsigned int c = 0;
                            p = utf8_decode(p, &c);
                            if (p == ((void *)0))
                            {
                                goto _catch_label_1;
                            }
                            if (c == 92)
                            {
                                p = escape_sequences_decode_opt(p, &c);
                                if (p == ((void *)0))
                                {
                                    goto _catch_label_1;
                                }
                            }
                            if (c < 128)
                            {
                                value = value * 256 + c;
                            }
                            else
                            {
                                value = c;
                            }
                            if (value > 2147483647)
                            {
                                snprintf(error_message, error_message_sz_bytes, "character constant too long for its type");
                                break;
                            }
                        }
                        return object_make_wchar_t((unsigned short)value);
                    }
                }
            }
        }
    }
    else _catch_label_1:
    {
    }
    struct object  empty = {0};
    return empty;
}

void pre_unexpected_end_of_file(struct token * p_token, struct preprocessor_ctx * ctx);
static void pre_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx);

static void pre_primary_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            pre_unexpected_end_of_file(ctx->input_list.tail, ctx);
            goto _catch_label_1;
        }
        if (ctx->current->type == 131)
        {
            char * p = ctx->current->lexeme + 1;
            char errmsg[200] = {0};
            struct object  v = char_constant_to_value(p, errmsg, sizeof errmsg);
            if (errmsg[0] != 0)
            {
                preprocessor_diagnostic_message(650, ctx, ctx->current, "%s", errmsg);
            }
            ectx->value = object_to_signed_long_long(&v);
            pre_match(ctx);
        }
        else
        {
            if (ctx->current->type == 134)
            {
                ppnumber_to_longlong(ctx, ctx->current, &ectx->value);
                pre_match(ctx);
            }
            else
            {
                if (ctx->current->type == 40)
                {
                    pre_match(ctx);
                    pre_expression(ctx, ectx);
                    if (ctx->n_errors > 0)
                    {
                        goto _catch_label_1;
                    }
                    if (ctx->current && ctx->current->type != 41)
                    {
                        preprocessor_diagnostic_message(650, ctx, ctx->current, "expected )");
                        goto _catch_label_1;
                    }
                    pre_match(ctx);
                }
                else
                {
                    preprocessor_diagnostic_message(1140, ctx, ctx->current, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
                    goto _catch_label_1;
                }
            }
        }
    }
    else _catch_label_1:
    {
    }
}

static void pre_postfix_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1)
    {
        pre_primary_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _catch_label_1;
        }
    }
    else _catch_label_1:
    {
    }
}

static void pre_cast_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx);

static void pre_unary_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1)
    {
        if (ctx->current && (ctx->current->type == 11051 || ctx->current->type == 11565))
        {
            preprocessor_diagnostic_message(1140, ctx, ctx->current, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
            goto _catch_label_1;
        }
        else
        {
            if (ctx->current != ((void *)0) && (ctx->current->type == 38 || ctx->current->type == 42 || ctx->current->type == 43 || ctx->current->type == 45 || ctx->current->type == 126 || ctx->current->type == 33))
            {
                struct token * p_old = ctx->current;
                int   op = ctx->current->type;
                pre_match(ctx);
                pre_cast_expression(ctx, ectx);
                if (ctx->n_errors > 0)
                {
                    goto _catch_label_1;
                }
                if (op == 33)
                {
                    ectx->value =  !ectx->value;
                }
                else
                {
                    if (op == 126)
                    {
                        ectx->value =  ~ectx->value;
                    }
                    else
                    {
                        if (op == 45)
                        {
                            ectx->value =  -ectx->value;
                        }
                        else
                        {
                            if (op == 43)
                            {
                                ectx->value =  +ectx->value;
                            }
                            else
                            {
                                if (op == 42)
                                {
                                    preprocessor_diagnostic_message(1140, ctx, p_old, "token '%s' is not valid in preprocessor expressions", p_old->lexeme);
                                }
                                else
                                {
                                    if (op == 38)
                                    {
                                        preprocessor_diagnostic_message(1140, ctx, p_old, "token '%s' is not valid in preprocessor expressions", p_old->lexeme);
                                    }
                                    else
                                    {
                                        preprocessor_diagnostic_message(1140, ctx, p_old, "token '%s' is not valid in preprocessor expressions", p_old->lexeme);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                pre_postfix_expression(ctx, ectx);
            }
        }
    }
    else _catch_label_1:
    {
    }
}

static void pre_cast_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    pre_unary_expression(ctx, ectx);
}

static void pre_multiplicative_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1)
    {
        pre_cast_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _catch_label_1;
        }
        while (ctx->current != ((void *)0) && (ctx->current->type == 42 || ctx->current->type == 47 || ctx->current->type == 37))
        {
            struct token * op_token = ctx->current;
            int   op = ctx->current->type;
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_cast_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                goto _catch_label_1;
            }
            if (op == 42)
            {
                ectx->value = (left_value * ectx->value);
            }
            else
            {
                if (op == 47)
                {
                    if (ectx->value == 0)
                    {
                        preprocessor_diagnostic_message(1330, ctx, op_token, "division by zero");
                        goto _catch_label_1;
                    }
                    else
                    {
                        ectx->value = (left_value / ectx->value);
                    }
                }
                else
                {
                    if (op == 37)
                    {
                        ectx->value = (left_value % ectx->value);
                    }
                }
            }
        }
    }
    else _catch_label_1:
    {
    }
}

static void pre_additive_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1)
    {
        pre_multiplicative_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _catch_label_1;
        }
        while (ctx->current != ((void *)0) && (ctx->current->type == 43 || ctx->current->type == 45))
        {
            struct token * p_op_token = ctx->current;
            pre_match(ctx);
            if (ctx->current == ((void *)0))
            {
                pre_unexpected_end_of_file(ctx->input_list.tail, ctx);
                goto _catch_label_1;
            }
            long long left_value = ectx->value;
            pre_multiplicative_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                goto _catch_label_1;
            }
            if (p_op_token->type == 43)
            {
                ectx->value = left_value + ectx->value;
            }
            else
            {
                if (p_op_token->type == 45)
                {
                    ectx->value = left_value - ectx->value;
                }
                else
                {
                    goto _catch_label_1;
                }
            }
        }
    }
    else _catch_label_1:
    {
    }
}

static void pre_shift_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1)
    {
        pre_additive_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _catch_label_1;
        }
        while (ctx->current != ((void *)0) && (ctx->current->type == 15934 || ctx->current->type == 15420))
        {
            int   op = ctx->current->type;
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_multiplicative_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                goto _catch_label_1;
            }
            if (op == 15934)
            {
                ectx->value = left_value >> ectx->value;
            }
            else
            {
                if (op == 15420)
                {
                    ectx->value = left_value << ectx->value;
                }
            }
        }
    }
    else _catch_label_1:
    {
    }
}

static void pre_relational_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1)
    {
        pre_shift_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _catch_label_1;
        }
        while (ctx->current != ((void *)0) && (ctx->current->type == 62 || ctx->current->type == 60 || ctx->current->type == 15933 || ctx->current->type == 15421))
        {
            int   op = ctx->current->type;
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_shift_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                goto _catch_label_1;
            }
            if (op == 62)
            {
                ectx->value = left_value > ectx->value;
            }
            else
            {
                if (op == 60)
                {
                    ectx->value = left_value < ectx->value;
                }
                else
                {
                    if (op == 15933)
                    {
                        ectx->value = left_value >= ectx->value;
                    }
                    else
                    {
                        if (op == 15421)
                        {
                            ectx->value = left_value <= ectx->value;
                        }
                    }
                }
            }
        }
    }
    else _catch_label_1:
    {
    }
}

static void pre_equality_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1)
    {
        pre_relational_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _catch_label_1;
        }
        while (ctx->current != ((void *)0) && (ctx->current->type == 15677 || ctx->current->type == 8509))
        {
            int   op = ctx->current->type;
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_multiplicative_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                goto _catch_label_1;
            }
            if (op == 15677)
            {
                ectx->value = left_value == ectx->value;
            }
            else
            {
                if (op == 8509)
                {
                    ectx->value = left_value != ectx->value;
                }
            }
        }
    }
    else _catch_label_1:
    {
    }
}

static void pre_and_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1)
    {
        pre_equality_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _catch_label_1;
        }
        while (ctx->current != ((void *)0) && (ctx->current->type == 38))
        {
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_equality_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                goto _catch_label_1;
            }
            ectx->value = left_value & ectx->value;
        }
    }
    else _catch_label_1:
    {
    }
}

static void pre_exclusive_or_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1)
    {
        pre_and_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _catch_label_1;
        }
        while (ctx->current != ((void *)0) && (ctx->current->type == 94))
        {
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_and_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                goto _catch_label_1;
            }
            ectx->value = left_value ^ ectx->value;
        }
    }
    else _catch_label_1:
    {
    }
}

static void pre_inclusive_or_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1)
    {
        pre_exclusive_or_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _catch_label_1;
        }
        while (ctx->current != ((void *)0) && (ctx->current->type == 124))
        {
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_exclusive_or_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                goto _catch_label_1;
            }
            ectx->value = left_value | ectx->value;
        }
    }
    else _catch_label_1:
    {
    }
}

static void pre_logical_and_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1)
    {
        pre_inclusive_or_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _catch_label_1;
        }
        while (ctx->current != ((void *)0) && (ctx->current->type == 9766))
        {
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_inclusive_or_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                goto _catch_label_1;
            }
            ectx->value = left_value && ectx->value;
        }
    }
    else _catch_label_1:
    {
    }
}

static void pre_logical_or_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1)
    {
        pre_logical_and_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _catch_label_1;
        }
        while (ctx->current != ((void *)0) && (ctx->current->type == 31868))
        {
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_logical_and_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                goto _catch_label_1;
            }
            ectx->value = left_value || ectx->value;
        }
    }
    else _catch_label_1:
    {
    }
}

static void pre_conditional_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx);

static void pre_assignment_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1)
    {
        pre_conditional_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _catch_label_1;
        }
        while (ctx->current != ((void *)0) && (ctx->current->type == 61 || ctx->current->type == 10813 || ctx->current->type == 12093 || ctx->current->type == 11069 || ctx->current->type == 11581 || ctx->current->type == 15421 || ctx->current->type == 15933 || ctx->current->type == 9789 || ctx->current->type == 24125 || ctx->current->type == 31805))
        {
            preprocessor_diagnostic_message(1140, ctx, ctx->current, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
            goto _catch_label_1;
        }
    }
    else _catch_label_1:
    {
    }
}

static void pre_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1)
    {
        pre_assignment_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _catch_label_1;
        }
        while (ctx->current && ctx->current->type == 44)
        {
            pre_match(ctx);
            pre_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                goto _catch_label_1;
            }
        }
    }
    else _catch_label_1:
    {
    }
}

static void pre_conditional_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1)
    {
        pre_logical_or_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _catch_label_1;
        }
        if (ctx->current && ctx->current->type == 63)
        {
            pre_match(ctx);
            if (ectx->value)
            {
                pre_expression(ctx, ectx);
                if (ctx->n_errors > 0)
                {
                    goto _catch_label_1;
                }
                pre_match(ctx);
                struct pre_expression_ctx  temp = {0};
                pre_conditional_expression(ctx, &temp);
                if (ctx->n_errors > 0)
                {
                    goto _catch_label_1;
                }
            }
            else
            {
                struct pre_expression_ctx  temp = {0};
                pre_expression(ctx, &temp);
                if (ctx->n_errors > 0)
                {
                    goto _catch_label_1;
                }
                pre_match(ctx);
                pre_conditional_expression(ctx, ectx);
                if (ctx->n_errors > 0)
                {
                    goto _catch_label_1;
                }
            }
        }
    }
    else _catch_label_1:
    {
    }
}

int pre_constant_expression(struct preprocessor_ctx * ctx, long long * pvalue)
{
    struct pre_expression_ctx  ectx = {0};
    pre_conditional_expression(ctx, &ectx);
     *pvalue = ectx.value;
    return ctx->n_errors > 0;
}

