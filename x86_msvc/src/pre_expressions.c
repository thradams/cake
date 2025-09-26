// Cake 0.12.05 target=x86_msvc
struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
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

struct include_dir {
    char * path;
    struct include_dir * next;
};

union _struct_tag_17 {
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

struct param {
    struct type  type;
    struct param * next;
};

struct object {
    int state;
    int value_type;
    struct type  type;
    char * debug_name;
    union _struct_tag_17  value;
    struct object * parent;
    struct object * p_ref;
    struct expression * p_init_expression;
    struct object * members;
    struct object * next;
};

struct pre_expression_ctx {
    long long value;
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

struct hash_map {
    struct map_entry ** table;
    int capacity;
    int size;
};

struct token_list {
    struct token * head;
    struct token * tail;
};

struct preprocessor_ctx {
    struct options  options;
    int flags;
    struct hash_map  macros;
    struct include_dir_list  include_dir;
    struct hash_map  pragma_once_map;
    struct token * current;
    struct token_list  input_list;
    unsigned int count_macro_value;
    unsigned char conditional_inclusion;
    int n_warnings;
    int n_errors;
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

struct map_entry {
    struct map_entry * next;
    unsigned int hash;
    char * key;
    int type;
    union _struct_tag_5  data;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

__declspec(selectany) int _Avx2WmemEnabledWeakValue = 0;
static void pre_conditional_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx);
static void pre_logical_or_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx);
static void pre_logical_and_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx);
static void pre_inclusive_or_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx);
static void pre_exclusive_or_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx);
static void pre_and_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx);
static void pre_equality_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx);
static void pre_relational_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx);
static void pre_shift_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx);
static void pre_additive_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx);
static void pre_multiplicative_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx);
static void pre_cast_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx);
static void pre_unary_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx);
unsigned char preprocessor_diagnostic(int w, struct preprocessor_ctx * ctx, struct token * p_token, char * fmt, ...);
static struct token *pre_match(struct preprocessor_ctx * ctx);
unsigned char token_is_blank(struct token * p);
static void pre_postfix_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx);
static void pre_primary_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx);
void pre_unexpected_end_of_file(struct token * p_token, struct preprocessor_ctx * ctx);
static struct object char_constant_to_value(char * s, char error_message[], int error_message_sz_bytes, int target);
unsigned char *utf8_decode(unsigned char * s, unsigned int * c);
unsigned char *escape_sequences_decode_opt(unsigned char * p, unsigned int * out_value);
static int __cdecl snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...);
static int __cdecl vsnprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, char * _ArgList);
int __cdecl __stdio_common_vsprintf(unsigned long long _Options, char * _Buffer, unsigned int _BufferCount, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __Ck0__OptionsStorage;
struct object object_make_wchar_t(int target, int value);
unsigned int target_get_wchar_max(int target);
signed long long object_to_signed_long_long(struct object * a);
void object_destroy(struct object * p);
static int ppnumber_to_longlong(struct preprocessor_ctx * ctx, struct token * token, long long * result, int target);
int parse_number(char * lexeme, char suffix[4], char erromsg[100]);
unsigned long long strtoull(char * _String, char ** _EndPtr, int _Radix);
int *__cdecl _errno(void);
struct object object_make_unsigned_int(unsigned int value);
unsigned long long target_get_unsigned_long_max(int target);
struct object object_make_unsigned_long(unsigned long long value, int target);
struct object object_make_unsigned_long_long(unsigned long long value);
struct object object_make_signed_int(signed int value);
long long target_get_signed_long_max(int target);
struct object object_make_signed_long(signed long long value, int target);
long long target_get_signed_long_long_max(int target);
struct object object_make_signed_long_long(signed long long value);
static void pre_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx);
static void pre_assignment_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx);

int pre_constant_expression(struct preprocessor_ctx * ctx, long long * pvalue)
{
    struct pre_expression_ctx  ectx;

    _cake_zmem(&ectx, 8);
    pre_conditional_expression(ctx, &ectx);
    *pvalue = ectx.value;
    return ctx->n_errors > 0;
}



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

static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &__Ck0__OptionsStorage;
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

static struct object char_constant_to_value(char * s, char error_message[], int error_message_sz_bytes, int target)
{
    unsigned char * p;
    struct object  empty;

    error_message[0] = 0;
    p = (unsigned char *)s;
    if (1) /*try*/
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
                goto _CKL0;/*throw*/
            }
            if (c == 92)
            {
                p = escape_sequences_decode_opt(p, &c);
                if (p == 0U)
                {
                    goto _CKL0;/*throw*/
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
            return object_make_wchar_t(target, c);
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
                    goto _CKL0;/*throw*/
                }
                if (c == 92)
                {
                    p = escape_sequences_decode_opt(p, &c);
                    if (p == 0U)
                    {
                        goto _CKL0;/*throw*/
                    }
                }
                if (*p != 39)
                {
                    snprintf(error_message, error_message_sz_bytes, "Unicode character literals may not contain multiple characters.");
                }
                if ((int)c > target_get_wchar_max(target))
                {
                    snprintf(error_message, error_message_sz_bytes, "Character too large for enclosing character literal type.");
                }
                return object_make_wchar_t(target, c);
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
                        goto _CKL0;/*throw*/
                    }
                    if (c == 92)
                    {
                        p = escape_sequences_decode_opt(p, &c);
                        if (p == 0U)
                        {
                            goto _CKL0;/*throw*/
                        }
                    }
                    if (*p != 39)
                    {
                        snprintf(error_message, error_message_sz_bytes, "Unicode character literals may not contain multiple characters.");
                    }
                    if (c > 4294967295U)
                    {
                        snprintf(error_message, error_message_sz_bytes, "Character too large for enclosing character literal type.");
                    }
                    return object_make_wchar_t(target, c);
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
                                goto _CKL0;/*throw*/
                            }
                            if (c == 92)
                            {
                                p = escape_sequences_decode_opt(p, &c);
                                if (p == 0U)
                                {
                                    goto _CKL0;/*throw*/
                                }
                            }
                            value = value * 256 + c;
                            if (value > target_get_wchar_max(target))
                            {
                                snprintf(error_message, error_message_sz_bytes, "character constant too long for its type");
                                break;
                            }
                        }
                        return object_make_wchar_t(target, (int)value);
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
                                goto _CKL0;/*throw*/
                            }
                            if (c == 92)
                            {
                                p = escape_sequences_decode_opt(p, &c);
                                if (p == 0U)
                                {
                                    goto _CKL0;/*throw*/
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
                        return object_make_wchar_t(target, (int)value);
                    }
                }
            }
        }
    }
    else _CKL0: /*catch*/ 
    {
    }
    _cake_zmem(&empty, 112);
    return empty;
}

static int ppnumber_to_longlong(struct preprocessor_ctx * ctx, struct token * token, long long * result, int target)
{
    int c;
    char buffer[260];
    char * s;
    char errormsg[100];
    char suffix[4];
    int type;
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
    _cake_zmem(&cv, 112);
    /*switch*/
    {
        register int __Ck1_temp = type;
        if (__Ck1_temp == 136) goto _CKL3; /*case 136*/
        if (__Ck1_temp == 137) goto _CKL4; /*case 137*/
        if (__Ck1_temp == 138) goto _CKL5; /*case 138*/
        if (__Ck1_temp == 139) goto _CKL6; /*case 139*/
        if (__Ck1_temp == 140) goto _CKL20; /*case 140*/
        if (__Ck1_temp == 141) goto _CKL21; /*case 141*/
        goto _CKL22;/*default*/

        {
            _CKL3: /*case 136*/ 
            _CKL4: /*case 137*/ 
            _CKL5: /*case 138*/ 
            _CKL6: /*case 139*/ 
            {
                unsigned long long value;

                value = 0;
                /*switch*/
                {
                    register int __Ck2_temp = type;
                    if (__Ck2_temp == 136) goto _CKL8; /*case 136*/
                    if (__Ck2_temp == 137) goto _CKL9; /*case 137*/
                    if (__Ck2_temp == 138) goto _CKL10; /*case 138*/
                    if (__Ck2_temp == 139) goto _CKL11; /*case 139*/
                    goto _CKL12;/*default*/

                    {
                        _CKL8: /*case 136*/ 
                        value = strtoull(buffer, 0U, 10);
                        goto _CKL7; /*break*/

                        _CKL9: /*case 137*/ 
                        value = strtoull(buffer + 1, 0U, 8);
                        goto _CKL7; /*break*/

                        _CKL10: /*case 138*/ 
                        value = strtoull(buffer + 2, 0U, 16);
                        goto _CKL7; /*break*/

                        _CKL11: /*case 139*/ 
                        value = strtoull(buffer + 2, 0U, 2);
                        goto _CKL7; /*break*/

                        _CKL12: /*default*/ 
                        goto _CKL7; /*break*/

                    }
                    _CKL7:;
                }
                if (value == 18446744073709551615ULL && (*_errno()) == 34)
                {
                }
                if (suffix[0] == 85)
                {
                    if (value <= 4294967295U && suffix[1] != 76)
                    {
                        cv = object_make_unsigned_int((unsigned int)value);
                    }
                    else
                    {
                        if (value <= target_get_unsigned_long_max(target) && suffix[2] != 76)
                        {
                            cv = object_make_unsigned_long((unsigned long)value, target);
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
                        if (value <= target_get_signed_long_max(target) && suffix[1] != 76)
                        {
                            cv = object_make_signed_long((long)value, target);
                        }
                        else
                        {
                            if (value <= target_get_signed_long_long_max(target))
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
            goto _CKL2; /*break*/

            _CKL20: /*case 140*/ 
            _CKL21: /*case 141*/ 
            goto _CKL2; /*break*/

            _CKL22: /*default*/ 
            ;
        }
        _CKL2:;
    }
    *result = object_to_signed_long_long(&cv);
    return 0;
}

static void pre_assignment_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1) /*try*/
    {
        pre_conditional_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _CKL0;/*throw*/
        }
        while (ctx->current != 0U && (ctx->current->type == 61 || ctx->current->type == 10813 || ctx->current->type == 12093 || ctx->current->type == 11069 || ctx->current->type == 11581 || ctx->current->type == 3947581 || ctx->current->type == 4079165 || ctx->current->type == 9789 || ctx->current->type == 24125 || ctx->current->type == 31805))
        {
            preprocessor_diagnostic(1140, ctx, ctx->current, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
            goto _CKL0;/*throw*/
        }
    }
    else _CKL0: /*catch*/ 
    {
    }
}

static void pre_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1) /*try*/
    {
        pre_assignment_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _CKL0;/*throw*/
        }
        while (ctx->current && ctx->current->type == 44)
        {
            pre_match(ctx);
            pre_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                goto _CKL0;/*throw*/
            }
        }
    }
    else _CKL0: /*catch*/ 
    {
    }
}

static void pre_primary_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1) /*try*/
    {
        if (ctx->current == 0U)
        {
            pre_unexpected_end_of_file(ctx->input_list.tail, ctx);
            goto _CKL0;/*throw*/
        }
        if (ctx->current->type == 131)
        {
            char * p;
            char errmsg[200];
            struct object  v;

            p = ctx->current->lexeme + 1;
            _cake_zmem(&errmsg, 200);
            v = char_constant_to_value(p, errmsg, 200U, ctx->options.target);
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
                ppnumber_to_longlong(ctx, ctx->current, &ectx->value, ctx->options.target);
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
                        goto _CKL0;/*throw*/
                    }
                    if (ctx->current && ctx->current->type != 41)
                    {
                        preprocessor_diagnostic(650, ctx, ctx->current, "expected )");
                        goto _CKL0;/*throw*/
                    }
                    pre_match(ctx);
                }
                else
                {
                    preprocessor_diagnostic(1140, ctx, ctx->current, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
                    goto _CKL0;/*throw*/
                }
            }
        }
    }
    else _CKL0: /*catch*/ 
    {
    }
}

static void pre_postfix_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1) /*try*/
    {
        pre_primary_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _CKL0;/*throw*/
        }
    }
    else _CKL0: /*catch*/ 
    {
    }
}

static void pre_unary_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1) /*try*/
    {
        if (ctx->current && (ctx->current->type == 11051 || ctx->current->type == 11565))
        {
            preprocessor_diagnostic(1140, ctx, ctx->current, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
            goto _CKL0;/*throw*/
        }
        else
        {
            if (ctx->current != 0U && (ctx->current->type == 38 || ctx->current->type == 42 || ctx->current->type == 43 || ctx->current->type == 45 || ctx->current->type == 126 || ctx->current->type == 33))
            {
                struct token * p_old;
                int op;

                p_old = ctx->current;
                op = ctx->current->type;
                pre_match(ctx);
                pre_cast_expression(ctx, ectx);
                if (ctx->n_errors > 0)
                {
                    goto _CKL0;/*throw*/
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
    else _CKL0: /*catch*/ 
    {
    }
}

static void pre_cast_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    pre_unary_expression(ctx, ectx);
}

static void pre_multiplicative_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1) /*try*/
    {
        pre_cast_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _CKL0;/*throw*/
        }
        while (ctx->current != 0U && (ctx->current->type == 42 || ctx->current->type == 47 || ctx->current->type == 37))
        {
            struct token * op_token;
            int op;
            long long left_value;

            op_token = ctx->current;
            op = ctx->current->type;
            pre_match(ctx);
            left_value = ectx->value;
            pre_cast_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                goto _CKL0;/*throw*/
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
                        goto _CKL0;/*throw*/
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
    else _CKL0: /*catch*/ 
    {
    }
}

static void pre_additive_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1) /*try*/
    {
        pre_multiplicative_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _CKL0;/*throw*/
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
                goto _CKL0;/*throw*/
            }
            left_value = ectx->value;
            pre_multiplicative_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                goto _CKL0;/*throw*/
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
                    goto _CKL0;/*throw*/
                }
            }
        }
    }
    else _CKL0: /*catch*/ 
    {
    }
}

static void pre_shift_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1) /*try*/
    {
        pre_additive_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _CKL0;/*throw*/
        }
        while (ctx->current != 0U && (ctx->current->type == 15934 || ctx->current->type == 15420))
        {
            int op;
            long long left_value;

            op = ctx->current->type;
            pre_match(ctx);
            left_value = ectx->value;
            pre_multiplicative_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                goto _CKL0;/*throw*/
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
    else _CKL0: /*catch*/ 
    {
    }
}

static void pre_relational_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1) /*try*/
    {
        pre_shift_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _CKL0;/*throw*/
        }
        while (ctx->current != 0U && (ctx->current->type == 62 || ctx->current->type == 60 || ctx->current->type == 15933 || ctx->current->type == 15421))
        {
            int op;
            long long left_value;

            op = ctx->current->type;
            pre_match(ctx);
            left_value = ectx->value;
            pre_shift_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                goto _CKL0;/*throw*/
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
    else _CKL0: /*catch*/ 
    {
    }
}

static void pre_equality_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1) /*try*/
    {
        pre_relational_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _CKL0;/*throw*/
        }
        while (ctx->current != 0U && (ctx->current->type == 15677 || ctx->current->type == 8509))
        {
            int op;
            long long left_value;

            op = ctx->current->type;
            pre_match(ctx);
            left_value = ectx->value;
            pre_multiplicative_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                goto _CKL0;/*throw*/
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
    else _CKL0: /*catch*/ 
    {
    }
}

static void pre_and_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1) /*try*/
    {
        pre_equality_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _CKL0;/*throw*/
        }
        while (ctx->current != 0U && (ctx->current->type == 38))
        {
            long long left_value;

            pre_match(ctx);
            left_value = ectx->value;
            pre_equality_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                goto _CKL0;/*throw*/
            }
            ectx->value = left_value & ectx->value;
        }
    }
    else _CKL0: /*catch*/ 
    {
    }
}

static void pre_exclusive_or_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1) /*try*/
    {
        pre_and_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _CKL0;/*throw*/
        }
        while (ctx->current != 0U && (ctx->current->type == 94))
        {
            long long left_value;

            pre_match(ctx);
            left_value = ectx->value;
            pre_and_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                goto _CKL0;/*throw*/
            }
            ectx->value = left_value ^ ectx->value;
        }
    }
    else _CKL0: /*catch*/ 
    {
    }
}

static void pre_inclusive_or_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1) /*try*/
    {
        pre_exclusive_or_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _CKL0;/*throw*/
        }
        while (ctx->current != 0U && (ctx->current->type == 124))
        {
            long long left_value;

            pre_match(ctx);
            left_value = ectx->value;
            pre_exclusive_or_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                goto _CKL0;/*throw*/
            }
            ectx->value = left_value | ectx->value;
        }
    }
    else _CKL0: /*catch*/ 
    {
    }
}

static void pre_logical_and_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1) /*try*/
    {
        pre_inclusive_or_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _CKL0;/*throw*/
        }
        while (ctx->current != 0U && (ctx->current->type == 9766))
        {
            long long left_value;

            pre_match(ctx);
            left_value = ectx->value;
            pre_inclusive_or_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                goto _CKL0;/*throw*/
            }
            ectx->value = left_value && ectx->value;
        }
    }
    else _CKL0: /*catch*/ 
    {
    }
}

static void pre_logical_or_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1) /*try*/
    {
        pre_logical_and_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _CKL0;/*throw*/
        }
        while (ctx->current != 0U && (ctx->current->type == 31868))
        {
            long long left_value;

            pre_match(ctx);
            left_value = ectx->value;
            pre_logical_and_expression(ctx, ectx);
            if (ctx->n_errors > 0)
            {
                goto _CKL0;/*throw*/
            }
            ectx->value = left_value || ectx->value;
        }
    }
    else _CKL0: /*catch*/ 
    {
    }
}

static void pre_conditional_expression(struct preprocessor_ctx * ctx, struct pre_expression_ctx * ectx)
{
    if (1) /*try*/
    {
        pre_logical_or_expression(ctx, ectx);
        if (ctx->n_errors > 0)
        {
            goto _CKL0;/*throw*/
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
                    goto _CKL0;/*throw*/
                }
                pre_match(ctx);
                _cake_zmem(&temp, 8);
                pre_conditional_expression(ctx, &temp);
                if (ctx->n_errors > 0)
                {
                    goto _CKL0;/*throw*/
                }
            }
            else
            {
                struct pre_expression_ctx  temp;

                _cake_zmem(&temp, 8);
                pre_expression(ctx, &temp);
                if (ctx->n_errors > 0)
                {
                    goto _CKL0;/*throw*/
                }
                pre_match(ctx);
                pre_conditional_expression(ctx, ectx);
                if (ctx->n_errors > 0)
                {
                    goto _CKL0;/*throw*/
                }
            }
        }
    }
    else _CKL0: /*catch*/ 
    {
    }
}
