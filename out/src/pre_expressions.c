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

struct param {
    struct type  type;
    struct param * next;
};

struct include_dir {
    char * path;
    struct include_dir * next;
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

struct hash_map {
    struct map_entry ** table;
    int capacity;
    int size;
};

struct include_dir_list {
    struct include_dir * head;
    struct include_dir * tail;
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

struct pre_expression_ctx {
    long long value;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

int _Avx2WmemEnabledWeakValue = 0;
int  parse_number(char * lexeme, char suffix[4], char erromsg[100]);
unsigned char  preprocessor_diagnostic(int   w, struct preprocessor_ctx * ctx, struct token * p_token, char * fmt, ...);
unsigned long long strtoull(char * _String, char ** _EndPtr, int _Radix);
int *__cdecl _errno(void);
struct object object_make_unsigned_int(unsigned int value);
struct object object_make_unsigned_long(unsigned long value);
struct object object_make_unsigned_long_long(unsigned long long value);
struct object object_make_signed_int(signed int value);
struct object object_make_signed_long(signed long value);
struct object object_make_signed_long_long(signed long long value);
signed long long object_to_signed_long_long(struct object * a);

static int ppnumber_to_longlong(struct preprocessor_ctx * ctx, struct token * token, long long * result)
{
    int c;
    char buffer[260];
    char * s;
    char errormsg[100];
    char suffix[4];
    int   type;
    struct object  cv;

    c = 0;
    _cake_zmem(&buffer, 260);
    s = token->lexeme;
    while (*s)
    {
        if (*s != 39)
        {
            buffer[c] = *s;
            c++;
        }
        s++;
    }
    _cake_zmem(&suffix, 4);
    type = parse_number(token->lexeme, suffix, errormsg);
    if (type == 0)
    {
        preprocessor_diagnostic(1380, ctx, token, "%s", errormsg);
        return 0;
    }
    _cake_zmem(&cv, 96);
    /*switch*/
    {
        register int   _R0 = type;
        if (_R0 == 136) goto _CKL3; /*case 136*/
        if (_R0 == 137) goto _CKL4; /*case 137*/
        if (_R0 == 138) goto _CKL5; /*case 138*/
        if (_R0 == 139) goto _CKL6; /*case 139*/
        if (_R0 == 140) goto _CKL20; /*case 140*/
        if (_R0 == 141) goto _CKL21; /*case 141*/
        goto /*default*/ _CKL22;

        {
            /*case 136*/ _CKL3:
            /*case 137*/ _CKL4:
            /*case 138*/ _CKL5:
            /*case 139*/ _CKL6:
            {
                unsigned long long value;

                value = 0;
                /*switch*/
                {
                    register int   _R1 = type;
                    if (_R1 == 136) goto _CKL8; /*case 136*/
                    if (_R1 == 137) goto _CKL9; /*case 137*/
                    if (_R1 == 138) goto _CKL10; /*case 138*/
                    if (_R1 == 139) goto _CKL11; /*case 139*/
                    goto /*default*/ _CKL12;

                    {
                        /*case 136*/ _CKL8:
                        value = strtoull(buffer, 0U, 10);
                        /*break*/ goto _CKL7;

                        /*case 137*/ _CKL9:
                        value = strtoull(buffer + 1, 0U, 8);
                        /*break*/ goto _CKL7;

                        /*case 138*/ _CKL10:
                        value = strtoull(buffer + 2, 0U, 16);
                        /*break*/ goto _CKL7;

                        /*case 139*/ _CKL11:
                        value = strtoull(buffer + 2, 0U, 2);
                        /*break*/ goto _CKL7;

                        /*default*/ _CKL12:
                        /*break*/ goto _CKL7;

                    }
                    _CKL7:;
                }
                if (value == 18446744073709551615ULL && (*_errno()) == 34)
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
            /*break*/ goto _CKL2;

            /*case 140*/ _CKL20:
            /*case 141*/ _CKL21:
            /*break*/ goto _CKL2;

            /*default*/ _CKL22:
            ;
        }
        _CKL2:;
    }
    *result = object_to_signed_long_long(&cv);
    return 0;
}

unsigned char  token_is_blank(struct token * p);

static struct token *pre_match(struct preprocessor_ctx * ctx)
{
    if (ctx->current == 0U)
    {
        return 0U;
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
    unsigned char * p;
    struct object  empty;

    error_message[0] = 0;
    p = (unsigned char *)s;
    /*try*/ if (1)
    {
        if (p[0] == 117 && p[1] == 56)
        {
            unsigned int c;

            p++;
            p++;
            p++;
            c = 0;
            p = utf8_decode(p, &c);
            if (p == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            if (c == 92)
            {
                p = escape_sequences_decode_opt(p, &c);
                if (p == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
            }
            if (*p != 39)
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
                unsigned int c;

                p++;
                p++;
                c = 0;
                p = utf8_decode(p, &c);
                if (p == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
                if (c == 92)
                {
                    p = escape_sequences_decode_opt(p, &c);
                    if (p == 0U)
                    {
                        /*throw*/ goto _CKL0;
                    }
                }
                if (*p != 39)
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
                    unsigned int c;

                    p++;
                    p++;
                    c = 0;
                    p = utf8_decode(p, &c);
                    if (p == 0U)
                    {
                        /*throw*/ goto _CKL0;
                    }
                    if (c == 92)
                    {
                        p = escape_sequences_decode_opt(p, &c);
                        if (p == 0U)
                        {
                            /*throw*/ goto _CKL0;
                        }
                    }
                    if (*p != 39)
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
                        long long value;

                        p++;
                        p++;
                        value = 0;
                        while (*p != 39)
                        {
                            unsigned int c;

                            c = 0;
                            p = utf8_decode(p, &c);
                            if (p == 0U)
                            {
                                /*throw*/ goto _CKL0;
                            }
                            if (c == 92)
                            {
                                p = escape_sequences_decode_opt(p, &c);
                                if (p == 0U)
                                {
                                    /*throw*/ goto _CKL0;
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
                        long long value;

                        p++;
                        value = 0;
                        while (*p != 39)
                        {
                            unsigned int c;

                            c = 0;
                            p = utf8_decode(p, &c);
                            if (p == 0U)
                            {
                                /*throw*/ goto _CKL0;
                            }
                            if (c == 92)
                            {
                                p = escape_sequences_decode_opt(p, &c);
                                if (p == 0U)
                                {
                                    /*throw*/ goto _CKL0;
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
    /*catch*/ else _CKL0:
    {
    }
    _cake_zmem(&empty, 96);
    return empty;
}

void pre_unexpected_end_of_file(struct token * p_token, struct preprocessor_ctx * ctx);
void object_destroy(struct object * p);
static void pre_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx);

static void pre_primary_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    /*try*/ if (1)
    {
        if (ctx->current == 0U)
        {
            pre_unexpected_end_of_file(ctx->input_list.tail, ctx);
            /*throw*/ goto _CKL0;
        }
        if (ctx->current->type == 131)
        {
            char * p;
            char errmsg[200];
            struct object  v;

            p = ctx->current->lexeme + 1;
            _cake_zmem(&errmsg, 200);
            v = char_constant_to_value(p, errmsg, 200U);
            if (errmsg[0] != 0)
            {
                preprocessor_diagnostic(650, ctx, ctx->current, "%s", errmsg);
            }
            ectx->value = object_to_signed_long_long(&v);
            pre_match(ctx);
            object_destroy(&v);
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
                        /*throw*/ goto _CKL0;
                    }
                    if (ctx->current && ctx->current->type != 41)
                    {
                        preprocessor_diagnostic(650, ctx, ctx->current, "expected )");
                        /*throw*/ goto _CKL0;
                    }
                    pre_match(ctx);
                }
                else
                {
                    preprocessor_diagnostic(1140, ctx, ctx->current, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
                    /*throw*/ goto _CKL0;
                }
            }
        }
    }
    /*catch*/ else _CKL0:
    {
    }
}

static void pre_postfix_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    /*try*/ if (1)
    {
        pre_primary_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            /*throw*/ goto _CKL0;
        }
    }
    /*catch*/ else _CKL0:
    {
    }
}

static void pre_cast_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx);

static void pre_unary_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    /*try*/ if (1)
    {
        if (ctx->current && (ctx->current->type == 11051 || ctx->current->type == 11565))
        {
            preprocessor_diagnostic(1140, ctx, ctx->current, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
            /*throw*/ goto _CKL0;
        }
        else
        {
            if (ctx->current != 0U && (ctx->current->type == 38 || ctx->current->type == 42 || ctx->current->type == 43 || ctx->current->type == 45 || ctx->current->type == 126 || ctx->current->type == 33))
            {
                struct token * p_old;
                int   op;

                p_old = ctx->current;
                op = ctx->current->type;
                pre_match(ctx);
                pre_cast_expression(ctx, ectx);
                if (ctx->n_errors > 0)
                {
                    /*throw*/ goto _CKL0;
                }
                if (op == 33)
                {
                    ectx->value = !ectx->value;
                }
                else
                {
                    if (op == 126)
                    {
                        ectx->value = ~ectx->value;
                    }
                    else
                    {
                        if (op == 45)
                        {
                            ectx->value = -ectx->value;
                        }
                        else
                        {
                            if (op == 43)
                            {
                                ectx->value = +ectx->value;
                            }
                            else
                            {
                                if (op == 42)
                                {
                                    preprocessor_diagnostic(1140, ctx, p_old, "token '%s' is not valid in preprocessor expressions", p_old->lexeme);
                                }
                                else
                                {
                                    if (op == 38)
                                    {
                                        preprocessor_diagnostic(1140, ctx, p_old, "token '%s' is not valid in preprocessor expressions", p_old->lexeme);
                                    }
                                    else
                                    {
                                        preprocessor_diagnostic(1140, ctx, p_old, "token '%s' is not valid in preprocessor expressions", p_old->lexeme);
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
    /*catch*/ else _CKL0:
    {
    }
}

static void pre_cast_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    pre_unary_expression(ctx, ectx);
}

static void pre_multiplicative_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    /*try*/ if (1)
    {
        pre_cast_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            /*throw*/ goto _CKL0;
        }
        while (ctx->current != 0U && (ctx->current->type == 42 || ctx->current->type == 47 || ctx->current->type == 37))
        {
            struct token * op_token;
            int   op;
            long long left_value;

            op_token = ctx->current;
            op = ctx->current->type;
            pre_match(ctx);
            left_value = ectx->value;
            pre_cast_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                /*throw*/ goto _CKL0;
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
                        preprocessor_diagnostic(1330, ctx, op_token, "division by zero");
                        /*throw*/ goto _CKL0;
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
    /*catch*/ else _CKL0:
    {
    }
}

static void pre_additive_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    /*try*/ if (1)
    {
        pre_multiplicative_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            /*throw*/ goto _CKL0;
        }
        while (ctx->current != 0U && (ctx->current->type == 43 || ctx->current->type == 45))
        {
            struct token * p_op_token;
            long long left_value;

            p_op_token = ctx->current;
            pre_match(ctx);
            if (ctx->current == 0U)
            {
                pre_unexpected_end_of_file(ctx->input_list.tail, ctx);
                /*throw*/ goto _CKL0;
            }
            left_value = ectx->value;
            pre_multiplicative_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                /*throw*/ goto _CKL0;
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
                    /*throw*/ goto _CKL0;
                }
            }
        }
    }
    /*catch*/ else _CKL0:
    {
    }
}

static void pre_shift_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    /*try*/ if (1)
    {
        pre_additive_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            /*throw*/ goto _CKL0;
        }
        while (ctx->current != 0U && (ctx->current->type == 15934 || ctx->current->type == 15420))
        {
            int   op;
            long long left_value;

            op = ctx->current->type;
            pre_match(ctx);
            left_value = ectx->value;
            pre_multiplicative_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                /*throw*/ goto _CKL0;
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
    /*catch*/ else _CKL0:
    {
    }
}

static void pre_relational_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    /*try*/ if (1)
    {
        pre_shift_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            /*throw*/ goto _CKL0;
        }
        while (ctx->current != 0U && (ctx->current->type == 62 || ctx->current->type == 60 || ctx->current->type == 15933 || ctx->current->type == 15421))
        {
            int   op;
            long long left_value;

            op = ctx->current->type;
            pre_match(ctx);
            left_value = ectx->value;
            pre_shift_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                /*throw*/ goto _CKL0;
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
    /*catch*/ else _CKL0:
    {
    }
}

static void pre_equality_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    /*try*/ if (1)
    {
        pre_relational_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            /*throw*/ goto _CKL0;
        }
        while (ctx->current != 0U && (ctx->current->type == 15677 || ctx->current->type == 8509))
        {
            int   op;
            long long left_value;

            op = ctx->current->type;
            pre_match(ctx);
            left_value = ectx->value;
            pre_multiplicative_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                /*throw*/ goto _CKL0;
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
    /*catch*/ else _CKL0:
    {
    }
}

static void pre_and_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    /*try*/ if (1)
    {
        pre_equality_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            /*throw*/ goto _CKL0;
        }
        while (ctx->current != 0U && (ctx->current->type == 38))
        {
            long long left_value;

            pre_match(ctx);
            left_value = ectx->value;
            pre_equality_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                /*throw*/ goto _CKL0;
            }
            ectx->value = left_value & ectx->value;
        }
    }
    /*catch*/ else _CKL0:
    {
    }
}

static void pre_exclusive_or_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    /*try*/ if (1)
    {
        pre_and_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            /*throw*/ goto _CKL0;
        }
        while (ctx->current != 0U && (ctx->current->type == 94))
        {
            long long left_value;

            pre_match(ctx);
            left_value = ectx->value;
            pre_and_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                /*throw*/ goto _CKL0;
            }
            ectx->value = left_value ^ ectx->value;
        }
    }
    /*catch*/ else _CKL0:
    {
    }
}

static void pre_inclusive_or_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    /*try*/ if (1)
    {
        pre_exclusive_or_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            /*throw*/ goto _CKL0;
        }
        while (ctx->current != 0U && (ctx->current->type == 124))
        {
            long long left_value;

            pre_match(ctx);
            left_value = ectx->value;
            pre_exclusive_or_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                /*throw*/ goto _CKL0;
            }
            ectx->value = left_value | ectx->value;
        }
    }
    /*catch*/ else _CKL0:
    {
    }
}

static void pre_logical_and_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    /*try*/ if (1)
    {
        pre_inclusive_or_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            /*throw*/ goto _CKL0;
        }
        while (ctx->current != 0U && (ctx->current->type == 9766))
        {
            long long left_value;

            pre_match(ctx);
            left_value = ectx->value;
            pre_inclusive_or_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                /*throw*/ goto _CKL0;
            }
            ectx->value = left_value && ectx->value;
        }
    }
    /*catch*/ else _CKL0:
    {
    }
}

static void pre_logical_or_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    /*try*/ if (1)
    {
        pre_logical_and_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            /*throw*/ goto _CKL0;
        }
        while (ctx->current != 0U && (ctx->current->type == 31868))
        {
            long long left_value;

            pre_match(ctx);
            left_value = ectx->value;
            pre_logical_and_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                /*throw*/ goto _CKL0;
            }
            ectx->value = left_value || ectx->value;
        }
    }
    /*catch*/ else _CKL0:
    {
    }
}

static void pre_conditional_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx);

static void pre_assignment_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    /*try*/ if (1)
    {
        pre_conditional_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            /*throw*/ goto _CKL0;
        }
        while (ctx->current != 0U && (ctx->current->type == 61 || ctx->current->type == 10813 || ctx->current->type == 12093 || ctx->current->type == 11069 || ctx->current->type == 11581 || ctx->current->type == 15421 || ctx->current->type == 15933 || ctx->current->type == 9789 || ctx->current->type == 24125 || ctx->current->type == 31805))
        {
            preprocessor_diagnostic(1140, ctx, ctx->current, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
            /*throw*/ goto _CKL0;
        }
    }
    /*catch*/ else _CKL0:
    {
    }
}

static void pre_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    /*try*/ if (1)
    {
        pre_assignment_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            /*throw*/ goto _CKL0;
        }
        while (ctx->current && ctx->current->type == 44)
        {
            pre_match(ctx);
            pre_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                /*throw*/ goto _CKL0;
            }
        }
    }
    /*catch*/ else _CKL0:
    {
    }
}

static void pre_conditional_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    /*try*/ if (1)
    {
        pre_logical_or_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            /*throw*/ goto _CKL0;
        }
        if (ctx->current && ctx->current->type == 63)
        {
            pre_match(ctx);
            if (ectx->value)
            {
                struct pre_expression_ctx  temp;

                pre_expression(ctx, ectx);
                if (ctx->n_errors > 0)
                {
                    /*throw*/ goto _CKL0;
                }
                pre_match(ctx);
                _cake_zmem(&temp, 8);
                pre_conditional_expression(ctx, &temp);
                if (ctx->n_errors > 0)
                {
                    /*throw*/ goto _CKL0;
                }
            }
            else
            {
                struct pre_expression_ctx  temp;

                _cake_zmem(&temp, 8);
                pre_expression(ctx, &temp);
                if (ctx->n_errors > 0)
                {
                    /*throw*/ goto _CKL0;
                }
                pre_match(ctx);
                pre_conditional_expression(ctx, ectx);
                if (ctx->n_errors > 0)
                {
                    /*throw*/ goto _CKL0;
                }
            }
        }
    }
    /*catch*/ else _CKL0:
    {
    }
}

int pre_constant_expression(struct preprocessor_ctx * ctx, long long * pvalue)
{
    struct pre_expression_ctx  ectx;

    _cake_zmem(&ectx, 8);
    pre_conditional_expression(ctx, &ectx);
    *pvalue = ectx.value;
    return ctx->n_errors > 0;
}

