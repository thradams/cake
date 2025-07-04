struct diagnostic {
    unsigned long long errors;
    unsigned long long warnings;
    unsigned long long notes;
};

struct diagnostic_stack {
    int top_index;
    struct diagnostic stack[10];
};

struct stream {
    char * source;
    char * current;
    int line;
    int col;
    int line_continuation_count;
    char * path;
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

struct token_list {
    struct token * head;
    struct token * tail;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
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

struct tokenizer_ctx {
    struct options  options;
    int n_warnings;
    int n_errors;
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

struct osstream {
    char * c_str;
    int size;
    int capacity;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

unsigned char  token_is_blank(struct token * p);

unsigned char  style_has_space(struct token * token)
{
    return token_is_blank(token->prev);
}

unsigned char  style_has_one_space(struct token * token)
{
    return !!(token->prev && token->prev->type == 143);
}

void token_delete(struct token * p);

void token_list_clear(struct token_list * list)
{
    struct token * p;

    p = list->head;
    while (p)
    {
        struct token * next;

        next = p->next;
        p->next = 0U;
        token_delete(p);
        p = next;
    }
    list->head = 0U;
    list->tail = 0U;
}

void token_range_add_show(struct token * first, struct token * last)
{
    {
        struct token * current;
        current = first;
        for (; current != last->next; current = current->next)
        {
            current->flags = current->flags & -65;
            if (current->next == 0U)
            {
                break;
            }
        }
    }
}

void token_range_remove_flag(struct token * first, struct token * last, int   flag)
{
    {
        struct token * current;
        current = first;
        for (; current && current != last->next; current = current->next)
        {
            current->flags = current->flags & ~flag;
        }
    }
}

void token_range_add_flag(struct token * first, struct token * last, int   flag)
{
    {
        struct token * current;
        current = first;
        for (; current && current != last->next; current = current->next)
        {
            current->flags |= flag;
        }
    }
}

void token_list_pop_back(struct token_list * list)
{
    if (list->head == 0U)
    {
        return;
    }
    if (list->head == list->tail)
    {
        token_delete(list->head);
        list->head = 0U;
        list->tail = 0U;
    }
    else
    {
        ;
        ;
        list->tail = list->tail->prev;
        token_delete(list->tail->next);
        list->tail->next = 0U;
        if (list->tail == list->head)
        {
            list->tail->prev = 0U;
        }
    }
}

void token_list_pop_front(struct token_list * list)
{
    struct token * p;

    if (list->head == 0U)
    {
        return;
    }
    p = list->head;
    if (list->head == list->tail)
    {
        list->head = 0U;
        list->tail = 0U;
    }
    else
    {
        list->head = p->next;
    }
    p->next = 0U;
    p->prev = 0U;
    token_delete(p);
}

struct token *token_list_pop_front_get(struct token_list * list)
{
    struct token * head;

    if (list->head == 0U)
    {
        return 0U;
    }
    head = list->head;
    if (list->head == list->tail)
    {
        list->head = 0U;
        list->tail = 0U;
        head->next = 0U;
        head->prev = 0U;
        return head;
    }
    list->head = head->next;
    head->next = 0U;
    head->prev = 0U;
    return head;
}

void token_list_swap(struct token_list * a, struct token_list * b)
{
    struct token_list  temp;

    temp = *a;
    *a = *b;
    *b = temp;
}

void free(void * ptr);

void token_delete(struct token * p)
{
    if (p)
    {
        ;
        free(p->lexeme);
        free(p);
    }
}

void token_list_set_file(struct token_list * list, struct token * filetoken, int line, int col)
{
    struct token * p;

    p = list->head;
    while (p)
    {
        p->token_origin = filetoken;
        p->line = line;
        p->col = col;
        p = p->next;
    }
}

void token_list_destroy(struct token_list * list)
{
    struct token * p;

    p = list->head;
    while (p)
    {
        struct token * next;

        next = p->next;
        p->next = 0U;
        token_delete(p);
        p = next;
    }
}

int ss_fprintf(struct osstream * stream, char * fmt, ...);
void ss_close(struct osstream * stream);

char *token_list_join_tokens(struct token_list * list, unsigned char   bliteral)
{
    struct osstream  ss;
    unsigned char   has_space;
    struct token * current;
    char * cstr;

    _cake_zmem(&ss, 12);
    if (bliteral)
    {
        ss_fprintf(&ss, "\"");
    }
    has_space = 0;
    current = list->head;
    while (current)
    {
        char * p;

        if (token_is_blank(current))
        {
            has_space = 1;
            current = current->next;
            continue;
        }
        if (has_space)
        {
            ss_fprintf(&ss, " ");
        }
        p = current->lexeme;
        while (*p)
        {
            if (*p == 34)
            {
                ss_fprintf(&ss, "\\\"");
            }
            else
            {
                ss_fprintf(&ss, "%c", *p);
            }
            p++;
        }
        current = current->next;
        if (current)
        {
            has_space = !!(current->flags & 4);
        }
    }
    if (bliteral)
    {
        ss_fprintf(&ss, "\"");
    }
    cstr = ss.c_str;
    ss.c_str = 0U;
    ss_close(&ss);
    return cstr;
}

struct token_list tokenizer(struct tokenizer_ctx * ctx, char * text, char * filename_opt, int level, int   addflags);
void token_list_insert_after(struct token_list * list, struct token * after, struct token_list * append);

void token_list_paste_string_after(struct token_list * list, struct token * after, char * s)
{
    struct tokenizer_ctx  tctx;
    struct token_list  l;

    _cake_zmem(&tctx, 696);
    l = tokenizer(&tctx, s, 0U, 0, 1);
    token_list_insert_after(list, after, &l);
    token_list_destroy(&l);
}

void token_list_insert_before(struct token_list * token_list, struct token * after, struct token_list * append_list);

void token_list_paste_string_before(struct token_list * list, struct token * before, char * s)
{
    struct tokenizer_ctx  tctx;
    struct token_list  l;

    _cake_zmem(&tctx, 696);
    l = tokenizer(&tctx, s, 0U, 0, 1);
    token_list_insert_before(list, before, &l);
    token_list_destroy(&l);
}

void token_list_insert_after(struct token_list * token_list, struct token * after, struct token_list * append_list)
{
    if (append_list->head == 0U)
    {
        return;
    }
    if (token_list->head == 0U)
    {
        ;
        token_list->head = append_list->head;
        token_list->tail = append_list->tail;
        append_list->head = 0U;
        append_list->tail = 0U;
        return;
    }
    if (after == 0U)
    {
        ;
        ;
        append_list->tail->next = token_list->head;
        token_list->head->prev = append_list->tail;
        token_list->head = append_list->head;
        append_list->head->prev = 0U;
    }
    else
    {
        struct token * follow;

        follow = after->next;
        if (token_list->tail == after)
        {
            token_list->tail = append_list->tail;
        }
        else
        {
            if (token_list->head == after)
            {
            }
        }
        ;
        ;
        append_list->tail->next = follow;
        follow->prev = append_list->tail;
        after->next = append_list->head;
        append_list->head->prev = after;
    }
    append_list->head = 0U;
    append_list->tail = 0U;
}

void token_list_insert_before(struct token_list * token_list, struct token * after, struct token_list * append_list)
{
    token_list_insert_after(token_list, after->prev, append_list);
}

int __cdecl strcmp(char * _Str1, char * _Str2);

unsigned char  token_list_is_equal(struct token_list * list_a, struct token_list * list_b)
{
    struct token * p_tka;
    struct token * p_tkb;

    p_tka = list_a->head;
    p_tkb = list_b->head;
    while (p_tka && p_tkb)
    {
        if (p_tka->type != p_tkb->type)
        {
            return 0;
        }
        if (strcmp(p_tka->lexeme, p_tkb->lexeme) != 0)
        {
            return 0;
        }
        p_tka = p_tka->next;
        p_tkb = p_tkb->next;
    }
    return !!(p_tka == 0U && p_tkb == 0U);
}

struct token *token_list_add(struct token_list * list, struct token * pnew)
{
    ;
    ;
    if (list->head == 0U)
    {
        pnew->prev = 0U;
        pnew->next = 0U;
        list->head = pnew;
        list->tail = pnew;
    }
    else
    {
        ;
        ;
        pnew->prev = list->tail;
        list->tail->next = pnew;
        list->tail = pnew;
    }
    ;
    ;
    return list->tail;
}

int is_digit(struct stream * p)
{
    return (p->current[0] >= 48 && p->current[0] <= 57);
}

unsigned char  token_is_identifier_or_keyword(int   t)
{
    /*switch*/
    {
        register int   _R0 = t;
        if (_R0 == 8996) goto _CKL1; /*case 8996*/
        if (_R0 == 8999) goto _CKL2; /*case 8999*/
        if (_R0 == 9000) goto _CKL3; /*case 9000*/
        if (_R0 == 9001) goto _CKL4; /*case 9001*/
        if (_R0 == 9002) goto _CKL5; /*case 9002*/
        if (_R0 == 9003) goto _CKL6; /*case 9003*/
        if (_R0 == 9004) goto _CKL7; /*case 9004*/
        if (_R0 == 9005) goto _CKL8; /*case 9005*/
        if (_R0 == 9006) goto _CKL9; /*case 9006*/
        if (_R0 == 9007) goto _CKL10; /*case 9007*/
        if (_R0 == 9008) goto _CKL11; /*case 9008*/
        if (_R0 == 9009) goto _CKL12; /*case 9009*/
        if (_R0 == 9010) goto _CKL13; /*case 9010*/
        if (_R0 == 9011) goto _CKL14; /*case 9011*/
        if (_R0 == 9012) goto _CKL15; /*case 9012*/
        if (_R0 == 9013) goto _CKL16; /*case 9013*/
        if (_R0 == 9014) goto _CKL17; /*case 9014*/
        if (_R0 == 9015) goto _CKL18; /*case 9015*/
        if (_R0 == 9016) goto _CKL19; /*case 9016*/
        if (_R0 == 9017) goto _CKL20; /*case 9017*/
        if (_R0 == 9018) goto _CKL21; /*case 9018*/
        if (_R0 == 9019) goto _CKL22; /*case 9019*/
        if (_R0 == 9020) goto _CKL23; /*case 9020*/
        if (_R0 == 9021) goto _CKL24; /*case 9021*/
        if (_R0 == 9022) goto _CKL25; /*case 9022*/
        if (_R0 == 9023) goto _CKL26; /*case 9023*/
        if (_R0 == 9024) goto _CKL27; /*case 9024*/
        if (_R0 == 9025) goto _CKL28; /*case 9025*/
        if (_R0 == 9026) goto _CKL29; /*case 9026*/
        if (_R0 == 9027) goto _CKL30; /*case 9027*/
        if (_R0 == 9028) goto _CKL31; /*case 9028*/
        if (_R0 == 9029) goto _CKL32; /*case 9029*/
        if (_R0 == 9030) goto _CKL33; /*case 9030*/
        if (_R0 == 9032) goto _CKL34; /*case 9032*/
        if (_R0 == 9033) goto _CKL35; /*case 9033*/
        if (_R0 == 9034) goto _CKL36; /*case 9034*/
        if (_R0 == 9035) goto _CKL37; /*case 9035*/
        if (_R0 == 9036) goto _CKL38; /*case 9036*/
        if (_R0 == 9037) goto _CKL39; /*case 9037*/
        if (_R0 == 9038) goto _CKL40; /*case 9038*/
        if (_R0 == 9039) goto _CKL41; /*case 9039*/
        if (_R0 == 9040) goto _CKL42; /*case 9040*/
        if (_R0 == 9041) goto _CKL43; /*case 9041*/
        if (_R0 == 9042) goto _CKL44; /*case 9042*/
        if (_R0 == 9043) goto _CKL45; /*case 9043*/
        if (_R0 == 9044) goto _CKL46; /*case 9044*/
        if (_R0 == 9045) goto _CKL47; /*case 9045*/
        if (_R0 == 9049) goto _CKL48; /*case 9049*/
        if (_R0 == 9050) goto _CKL49; /*case 9050*/
        if (_R0 == 9051) goto _CKL50; /*case 9051*/
        if (_R0 == 9052) goto _CKL51; /*case 9052*/
        if (_R0 == 9053) goto _CKL52; /*case 9053*/
        if (_R0 == 9054) goto _CKL53; /*case 9054*/
        if (_R0 == 9055) goto _CKL54; /*case 9055*/
        if (_R0 == 9056) goto _CKL55; /*case 9056*/
        if (_R0 == 9057) goto _CKL56; /*case 9057*/
        if (_R0 == 9058) goto _CKL57; /*case 9058*/
        if (_R0 == 9059) goto _CKL58; /*case 9059*/
        if (_R0 == 9060) goto _CKL59; /*case 9060*/
        if (_R0 == 9061) goto _CKL60; /*case 9061*/
        if (_R0 == 9062) goto _CKL61; /*case 9062*/
        if (_R0 == 9063) goto _CKL62; /*case 9063*/
        if (_R0 == 9064) goto _CKL63; /*case 9064*/
        if (_R0 == 9065) goto _CKL64; /*case 9065*/
        if (_R0 == 9066) goto _CKL65; /*case 9066*/
        if (_R0 == 9067) goto _CKL66; /*case 9067*/
        if (_R0 == 9068) goto _CKL67; /*case 9068*/
        if (_R0 == 9069) goto _CKL68; /*case 9069*/
        if (_R0 == 9070) goto _CKL69; /*case 9070*/
        if (_R0 == 9071) goto _CKL70; /*case 9071*/
        if (_R0 == 9072) goto _CKL71; /*case 9072*/
        if (_R0 == 9073) goto _CKL72; /*case 9073*/
        if (_R0 == 9074) goto _CKL73; /*case 9074*/
        if (_R0 == 9075) goto _CKL74; /*case 9075*/
        if (_R0 == 9076) goto _CKL75; /*case 9076*/
        if (_R0 == 9077) goto _CKL76; /*case 9077*/
        if (_R0 == 9078) goto _CKL77; /*case 9078*/
        if (_R0 == 9079) goto _CKL78; /*case 9079*/
        if (_R0 == 9080) goto _CKL79; /*case 9080*/
        if (_R0 == 9081) goto _CKL80; /*case 9081*/
        if (_R0 == 9082) goto _CKL81; /*case 9082*/
        if (_R0 == 9083) goto _CKL82; /*case 9083*/
        if (_R0 == 9084) goto _CKL83; /*case 9084*/
        if (_R0 == 9085) goto _CKL84; /*case 9085*/
        goto /*default*/ _CKL85;

        {
            /*case 8996*/ _CKL1:
            return 1;
            /*case 8999*/ _CKL2:
            /*case 9000*/ _CKL3:
            /*case 9001*/ _CKL4:
            /*case 9002*/ _CKL5:
            /*case 9003*/ _CKL6:
            /*case 9004*/ _CKL7:
            /*case 9005*/ _CKL8:
            /*case 9006*/ _CKL9:
            /*case 9007*/ _CKL10:
            /*case 9008*/ _CKL11:
            /*case 9009*/ _CKL12:
            /*case 9010*/ _CKL13:
            /*case 9011*/ _CKL14:
            /*case 9012*/ _CKL15:
            /*case 9013*/ _CKL16:
            /*case 9014*/ _CKL17:
            /*case 9015*/ _CKL18:
            /*case 9016*/ _CKL19:
            /*case 9017*/ _CKL20:
            /*case 9018*/ _CKL21:
            /*case 9019*/ _CKL22:
            /*case 9020*/ _CKL23:
            /*case 9021*/ _CKL24:
            /*case 9022*/ _CKL25:
            /*case 9023*/ _CKL26:
            /*case 9024*/ _CKL27:
            /*case 9025*/ _CKL28:
            /*case 9026*/ _CKL29:
            /*case 9027*/ _CKL30:
            /*case 9028*/ _CKL31:
            /*case 9029*/ _CKL32:
            /*case 9030*/ _CKL33:
            /*case 9032*/ _CKL34:
            /*case 9033*/ _CKL35:
            /*case 9034*/ _CKL36:
            /*case 9035*/ _CKL37:
            /*case 9036*/ _CKL38:
            /*case 9037*/ _CKL39:
            /*case 9038*/ _CKL40:
            /*case 9039*/ _CKL41:
            /*case 9040*/ _CKL42:
            /*case 9041*/ _CKL43:
            /*case 9042*/ _CKL44:
            /*case 9043*/ _CKL45:
            /*case 9044*/ _CKL46:
            /*case 9045*/ _CKL47:
            /*case 9049*/ _CKL48:
            /*case 9050*/ _CKL49:
            /*case 9051*/ _CKL50:
            /*case 9052*/ _CKL51:
            /*case 9053*/ _CKL52:
            /*case 9054*/ _CKL53:
            /*case 9055*/ _CKL54:
            /*case 9056*/ _CKL55:
            /*case 9057*/ _CKL56:
            /*case 9058*/ _CKL57:
            /*case 9059*/ _CKL58:
            /*case 9060*/ _CKL59:
            /*case 9061*/ _CKL60:
            /*case 9062*/ _CKL61:
            /*case 9063*/ _CKL62:
            /*case 9064*/ _CKL63:
            /*case 9065*/ _CKL64:
            /*case 9066*/ _CKL65:
            /*case 9067*/ _CKL66:
            /*case 9068*/ _CKL67:
            /*case 9069*/ _CKL68:
            /*case 9070*/ _CKL69:
            /*case 9071*/ _CKL70:
            /*case 9072*/ _CKL71:
            /*case 9073*/ _CKL72:
            /*case 9074*/ _CKL73:
            /*case 9075*/ _CKL74:
            /*case 9076*/ _CKL75:
            /*case 9077*/ _CKL76:
            /*case 9078*/ _CKL77:
            /*case 9079*/ _CKL78:
            /*case 9080*/ _CKL79:
            /*case 9081*/ _CKL80:
            /*case 9082*/ _CKL81:
            /*case 9083*/ _CKL82:
            /*case 9084*/ _CKL83:
            /*case 9085*/ _CKL84:
            return 1;
            /*default*/ _CKL85:
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    return 0;
}

unsigned char  token_is_blank(struct token * p)
{
    return !!(p->type == 8998 || p->type == 143 || p->type == 132 || p->type == 133);
}

struct token *clone_token(struct token * p);

struct token *token_list_clone_and_add(struct token_list * list, struct token * pnew)
{
    struct token * clone;

    clone = clone_token(pnew);
    if (clone == 0U)
    {
        return 0U;
    }
    return token_list_add(list, clone);
}

void token_list_append_list_at_beginning(struct token_list * dest, struct token_list * source)
{
    if (source->head == 0U)
    {
        return;
    }
    if (dest->head == 0U)
    {
        dest->head = source->head;
        dest->tail = source->tail;
    }
    else
    {
        ;
        ;
        source->tail->next = dest->head;
        dest->head = source->head;
    }
    source->head = 0U;
    source->tail = 0U;
}

void token_list_append_list(struct token_list * dest, struct token_list * source)
{
    if (source->head == 0U)
    {
        return;
    }
    if (dest->head == 0U)
    {
        dest->head = source->head;
        dest->tail = source->tail;
    }
    else
    {
        ;
        ;
        dest->tail->next = source->head;
        source->head->prev = dest->tail;
        dest->tail = source->tail;
    }
    source->head = 0U;
    source->tail = 0U;
}

void *calloc(unsigned int nmemb, unsigned int size);
char *strdup(char * src);

struct token *clone_token(struct token * p)
{
    struct token * token;
    char * lexeme;

    token = calloc(1, 40U);
    if (token == 0U)
    {
        return 0U;
    }
    lexeme = strdup(p->lexeme);
    if (lexeme == 0U)
    {
        free(token);
        return 0U;
    }
    *token = *p;
    token->lexeme = lexeme;
    token->next = 0U;
    token->prev = 0U;
    return token;
}

struct token_list token_list_remove_get(struct token_list * list, struct token * first, struct token * last)
{
    struct token_list  r;
    struct token * before_first;
    struct token * after_last;

    _cake_zmem(&r, 8);
    before_first = first->prev;
    after_last = last->next;
    if (before_first)
    {
        before_first->next = after_last;
    }
    else
    {
        list->head = last->next;
    }
    if (after_last)
    {
        after_last->prev = before_first;
    }
    else
    {
        list->tail = 0U;
    }
    last->next = 0U;
    r.head = (struct token *)first;
    first->prev = 0U;
    r.tail = last;
    return r;
}

void token_list_remove(struct token_list * list, struct token * first, struct token * last)
{
    struct token_list  r;

    r = token_list_remove_get(list, first, last);
    token_list_destroy(&r);
}

unsigned char  token_list_is_empty(struct token_list * p)
{
    ;
    return !!(p->head == 0U);
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
void print_literal2(char * s);

void print_list(struct token_list * list)
{
    struct token * current;

    current = list->head;
    while (current)
    {
        if (current != list->head)
        {
            printf("\xcb\xb0");
        }
        print_literal2(current->lexeme);
        printf("\x1b[0m");
        if (current == list->tail)
        {
        }
        current = current->next;
    }
    printf("\n");
}

void print_literal2(char * s)
{
    while (*s)
    {
        /*switch*/
        {
            register char _R1 = *s;
            if (_R1 == 10) goto _CKL1; /*case 10*/
            goto /*default*/ _CKL2;

            {
                /*case 10*/ _CKL1:
                printf("\\n");
                /*break*/ goto _CKL0;

                /*default*/ _CKL2:
                printf("%c", *s);
            }
            _CKL0:;
        }
        s++;
    }
}

int snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...);
char *get_token_name(int   tk);
char *__cdecl strcat(char * _Destination, char * _Source);

void print_token(struct token * p_token)
{
    char buffer0[50];
    char buffer[50];

    {
        int i;
        i = 0;
        for (; i < p_token->level; i++)
        {
            printf("  ");
        }
    }
    if (p_token->flags & 1)
    {
        printf("\x1b[92m");
    }
    else
    {
        printf("\x1b[37m");
    }
    _cake_zmem(&buffer0, 50);
    snprintf(buffer0, 50U, "%d:%d", p_token->line, p_token->col);
    printf("%-6s ", buffer0);
    printf("%-20s ", get_token_name(p_token->type));
    if (p_token->flags & 2)
    {
        printf("\x1b[36;1m");
    }
    _cake_zmem(&buffer, 50);
    strcat(buffer, "[");
    if (p_token->flags & 1)
    {
        strcat(buffer, "final ");
    }
    if (p_token->flags & 64)
    {
        strcat(buffer, "hide ");
    }
    if (p_token->flags & 2)
    {
        strcat(buffer, "expanded ");
    }
    if (p_token->flags & 4)
    {
        strcat(buffer, "space ");
    }
    if (p_token->flags & 8)
    {
        strcat(buffer, "newline ");
    }
    strcat(buffer, "]");
    printf("%-20s ", buffer);
    print_literal2(p_token->lexeme);
    printf("\n");
    printf("\x1b[0m");
}

void print_tokens(struct token * p_token)
{
    struct token * current;

    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n""\x1b[0m");
    current = p_token;
    while (current)
    {
        print_token(current);
        current = current->next;
    }
    printf("\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n""\x1b[0m");
    printf("\x1b[0m");
}

void print_token_html(struct token * p_token)
{
    printf("<span class=\"");
    if (!(p_token->flags & 1))
    {
        printf("notfinal ");
    }
    if (p_token->flags & 1)
    {
        printf("final ");
    }
    if (p_token->flags & 64)
    {
        printf("hide ");
    }
    if (p_token->flags & 2)
    {
        printf("expanded ");
    }
    if (p_token->flags & 4)
    {
        printf("space ");
    }
    if (p_token->flags & 8)
    {
        printf("newline ");
    }
    printf("\">");
    print_literal2(p_token->lexeme);
    printf("</span>");
    if (p_token->type == 10 || p_token->type == 8998)
    {
        printf("<br>\n");
    }
}

void print_tokens_html(struct token * p_token)
{
    struct token * current;

    printf("<pre>\n");
    current = p_token;
    while (current)
    {
        print_token_html(current);
        current = current->next;
    }
    printf("\n</pre>");
}

void print_position(char * path, int line, int col, unsigned char   visual_studio_ouput_format)
{
    if (visual_studio_ouput_format)
    {
        printf("%s(%d,%d): ", path ? path : "<>", line, col);
    }
    else
    {
        printf("\x1b[97m""%s:%d:%d: ", path ? path : "<>", line, col);
    }
}

int __cdecl putc(int _Character, struct _iobuf * _Stream);

void print_line_and_token(struct marker * p_marker, unsigned char   visual_studio_ouput_format)
{
    /*try*/ if (1)
    {
        struct token * p_token;
        int line;
        char nbuffer[20];
        int n;
        struct token * p_line_begin;
        struct token * p_token_begin;
        struct token * p_token_end;
        unsigned char   expand_macro;
        struct token * p_item;
        unsigned char   complete;
        int start_col;
        int end_col;
        unsigned char   onoff;

        p_token = p_marker->p_token_caret ? p_marker->p_token_caret : p_marker->p_token_begin;
        if (p_token == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        line = p_marker->line;
        if (!visual_studio_ouput_format)
        {
            printf("\x1b[0m");
        }
        _cake_zmem(&nbuffer, 20);
        n = snprintf(nbuffer, 20U, "%d", line);
        printf(" %s |", nbuffer);
        p_line_begin = p_token;
        while (p_line_begin->prev && (p_line_begin->prev->type != 10 && p_line_begin->prev->type != 8998))
        {
            p_line_begin = p_line_begin->prev;
        }
        p_token_begin = p_marker->p_token_begin ? p_marker->p_token_begin : p_marker->p_token_caret;
        p_token_end = p_marker->p_token_end ? p_marker->p_token_end : p_marker->p_token_caret;
        if (p_token_begin == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        expand_macro = !!(p_token_begin->flags & 2);
        if (!visual_studio_ouput_format)
        {
            printf("\x1b[34;1m");
        }
        p_item = p_line_begin;
        while (p_item)
        {
            if (!visual_studio_ouput_format)
            {
                if (p_item->flags & 2)
                {
                    printf("\x1b[90m");
                }
                else
                {
                    if (p_item->type >= 8999 && p_item->type <= 9085)
                    {
                        printf("\x1b[34m");
                    }
                    else
                    {
                        if (p_item->type == 133 || p_item->type == 132)
                        {
                            printf("\x1b[93m");
                        }
                    }
                }
            }
            if (!(p_item->flags & 2) || expand_macro)
            {
                char * p;

                p = p_item->lexeme;
                while (*p)
                {
                    putc(*p, (__acrt_iob_func(1)));
                    p++;
                }
            }
            if (!visual_studio_ouput_format)
            {
                printf("\x1b[0m");
            }
            if (p_item->type == 10)
            {
                break;
            }
            p_item = p_item->next;
        }
        if (!visual_studio_ouput_format)
        {
            printf("\x1b[0m");
        }
        if (p_item == 0U)
        {
            printf("\n");
        }
        printf(" %*s |", n, " ");
        complete = 0;
        start_col = 1;
        end_col = 1;
        onoff = 0;
        p_item = p_line_begin;
        while (p_item)
        {
            if (p_item == p_token_begin)
            {
                if (!visual_studio_ouput_format)
                {
                    printf("\x1b[92m");
                }
                onoff = 1;
                end_col = start_col;
            }
            if (!(p_item->flags & 2) || expand_macro)
            {
                char * p;

                p = p_item->lexeme;
                while (*p)
                {
                    if (onoff)
                    {
                        putc(126, (__acrt_iob_func(1)));
                        end_col++;
                    }
                    else
                    {
                        putc(32, (__acrt_iob_func(1)));
                        if (!complete)
                        {
                            start_col++;
                        }
                    }
                    p++;
                }
            }
            if (p_item->type == 10)
            {
                break;
            }
            if (p_item == p_token_end)
            {
                complete = 1;
                onoff = 0;
                if (!visual_studio_ouput_format)
                {
                    printf("\x1b[0m");
                }
            }
            p_item = p_item->next;
        }
        if (!visual_studio_ouput_format)
        {
            printf("\x1b[0m");
        }
        printf("\n");
        p_marker->start_col = start_col;
        p_marker->end_col = end_col;
    }
    /*catch*/ else _CKL0:
    {
    }
}

void stream_match(struct stream * stream);

static void digit_sequence_opt(struct stream * stream)
{
    while (is_digit(stream))
    {
        stream_match(stream);
    }
}

static void binary_exponent_part(struct stream * stream)
{
    stream_match(stream);
    if (stream->current[0] == 43 || stream->current[0] == 45)
    {
        stream_match(stream);
    }
    digit_sequence_opt(stream);
}

static unsigned char  is_hexadecimal_digit(struct stream * stream)
{
    return !!((stream->current[0] >= 48 && stream->current[0] <= 57) || (stream->current[0] >= 97 && stream->current[0] <= 102) || (stream->current[0] >= 65 && stream->current[0] <= 70));
}

static unsigned char  is_octal_digit(struct stream * stream)
{
    return !!(stream->current[0] >= 48 && stream->current[0] <= 55);
}

static void hexadecimal_digit_sequence(struct stream * stream)
{
    stream_match(stream);
    while (stream->current[0] == 39 || is_hexadecimal_digit(stream))
    {
        if (stream->current[0] == 39)
        {
            stream_match(stream);
            if (!is_hexadecimal_digit(stream))
            {
            }
            stream_match(stream);
        }
        else
        {
            stream_match(stream);
        }
    }
}

static void integer_suffix_opt(struct stream * stream, char suffix[4])
{
    if (stream->current[0] == 85 || stream->current[0] == 117)
    {
        suffix[0] = 85;
        stream_match(stream);
        if (stream->current[0] == 108 || stream->current[0] == 76)
        {
            suffix[1] = 76;
            stream_match(stream);
        }
        if (stream->current[0] == 108 || stream->current[0] == 76)
        {
            suffix[2] = 76;
            stream_match(stream);
        }
    }
    else
    {
        if ((stream->current[0] == 108 || stream->current[0] == 76))
        {
            suffix[0] = 76;
            stream_match(stream);
            if ((stream->current[0] == 108 || stream->current[0] == 76))
            {
                suffix[1] = 76;
                stream_match(stream);
            }
            if (stream->current[0] == 85 || stream->current[0] == 117)
            {
                suffix[3] = suffix[2];
                suffix[2] = suffix[1];
                suffix[1] = suffix[0];
                suffix[0] = 85;
                stream_match(stream);
            }
        }
        else
        {
            if (stream->current[0] == 105 && stream->current[1] == 56)
            {
                stream_match(stream);
                stream_match(stream);
                stream_match(stream);
                suffix[0] = 105;
                suffix[1] = 56;
            }
            else
            {
                if (stream->current[0] == 105 && stream->current[1] == 51 && stream->current[2] == 50)
                {
                    stream_match(stream);
                    stream_match(stream);
                    stream_match(stream);
                    suffix[0] = 105;
                    suffix[1] = 51;
                    suffix[2] = 50;
                }
                else
                {
                    if (stream->current[0] == 105 && stream->current[1] == 54 && stream->current[2] == 52)
                    {
                        stream_match(stream);
                        stream_match(stream);
                        stream_match(stream);
                        suffix[0] = 105;
                        suffix[1] = 54;
                        suffix[2] = 52;
                    }
                }
            }
        }
    }
}

static void exponent_part_opt(struct stream * stream)
{
    if (stream->current[0] == 101 || stream->current[0] == 69)
    {
        stream_match(stream);
        if (stream->current[0] == 45 || stream->current[0] == 43)
        {
            stream_match(stream);
        }
        digit_sequence_opt(stream);
    }
}

static void floating_suffix_opt(struct stream * stream, char suffix[4])
{
    if (stream->current[0] == 108 || stream->current[0] == 76)
    {
        suffix[0] = 76;
        stream_match(stream);
    }
    else
    {
        if (stream->current[0] == 102 || stream->current[0] == 70)
        {
            suffix[0] = 70;
            stream_match(stream);
        }
    }
}

static unsigned char  is_binary_digit(struct stream * stream)
{
    return !!(stream->current[0] >= 48 && stream->current[0] <= 49);
}

static unsigned char  is_nonzero_digit(struct stream * stream)
{
    return !!(stream->current[0] >= 49 && stream->current[0] <= 57);
}

int  parse_number_core(struct stream * stream, char suffix[4], char errmsg[100])
{
    int   type;

    errmsg[0] = 0;
    type = 0;
    if (stream->current[0] == 46)
    {
        type = 140;
        stream_match(stream);
        if (stream->current[0] == 46)
        {
            snprintf(errmsg, 100, "too many decimal points in number");
            return 0;
        }
        digit_sequence_opt(stream);
        exponent_part_opt(stream);
        floating_suffix_opt(stream, suffix);
    }
    else
    {
        if (stream->current[0] == 48 && (stream->current[1] == 120 || stream->current[1] == 88))
        {
            type = 138;
            stream_match(stream);
            stream_match(stream);
            if (is_hexadecimal_digit(stream))
            {
                while (is_hexadecimal_digit(stream))
                {
                    stream_match(stream);
                }
            }
            else
            {
                snprintf(errmsg, 100, "expected hexadecimal digit");
                return 0;
            }
            integer_suffix_opt(stream, suffix);
            if (stream->current[0] == 46)
            {
                type = 141;
                hexadecimal_digit_sequence(stream);
            }
            if (stream->current[0] == 112 || stream->current[0] == 80)
            {
                type = 141;
                binary_exponent_part(stream);
            }
            if (type == 141)
            {
                floating_suffix_opt(stream, suffix);
            }
        }
        else
        {
            if (stream->current[0] == 48 && (stream->current[1] == 98 || stream->current[1] == 66))
            {
                type = 139;
                stream_match(stream);
                stream_match(stream);
                if (is_binary_digit(stream))
                {
                    while (is_binary_digit(stream))
                    {
                        stream_match(stream);
                    }
                }
                else
                {
                    snprintf(errmsg, 100, "expected binary digit");
                    return 0;
                }
                integer_suffix_opt(stream, suffix);
            }
            else
            {
                if (stream->current[0] == 48)
                {
                    type = 137;
                    stream_match(stream);
                    if (stream->current[0] == 79 || stream->current[0] == 111)
                    {
                        stream_match(stream);
                    }
                    while (is_octal_digit(stream))
                    {
                        stream_match(stream);
                    }
                    integer_suffix_opt(stream, suffix);
                    if (stream->current[0] == 46)
                    {
                        type = 140;
                        hexadecimal_digit_sequence(stream);
                        floating_suffix_opt(stream, suffix);
                    }
                }
                else
                {
                    if (is_nonzero_digit(stream))
                    {
                        type = 136;
                        stream_match(stream);
                        while (is_digit(stream))
                        {
                            stream_match(stream);
                        }
                        integer_suffix_opt(stream, suffix);
                        if (stream->current[0] == 101 || stream->current[0] == 69)
                        {
                            exponent_part_opt(stream);
                            floating_suffix_opt(stream, suffix);
                        }
                        else
                        {
                            if (stream->current[0] == 46)
                            {
                                stream_match(stream);
                                type = 140;
                                if (stream->current[0] == 46)
                                {
                                    snprintf(errmsg, 100, "too many decimal points in number");
                                    return 0;
                                }
                                digit_sequence_opt(stream);
                                exponent_part_opt(stream);
                                floating_suffix_opt(stream, suffix);
                            }
                        }
                    }
                }
            }
        }
    }
    return type;
}

int  parse_number(char * lexeme, char suffix[4], char errmsg[100])
{
    struct stream  stream;

    stream.source = lexeme;
    stream.current = lexeme;
    stream.line = 1;
    stream.col = 1;
    stream.line_continuation_count = 0;
    stream.path = "";
    return parse_number_core(&stream, suffix, errmsg);
}

unsigned char *utf8_decode(unsigned char * s, unsigned int * c)
{
    unsigned char * next;

    *c = 0;
    if (s[0] == 0)
    {
        *c = 0;
        return 0U;
    }
    next = 0U;
    if (s[0] < 128)
    {
        *c = s[0];
        ;
        next = s + 1;
    }
    else
    {
        if ((s[0] & 224) == 192)
        {
            *c = ((int)(s[0] & 31) << 6) | ((int)(s[1] & 63) << 0);
            ;
            next = s + 2;
        }
        else
        {
            if ((s[0] & 240) == 224)
            {
                *c = ((int)(s[0] & 15) << 12) | ((int)(s[1] & 63) << 6) | ((int)(s[2] & 63) << 0);
                ;
                next = s + 3;
            }
            else
            {
                if ((s[0] & 248) == 240 && (s[0] <= 244))
                {
                    *c = ((int)(s[0] & 7) << 18) | ((int)(s[1] & 63) << 12) | ((int)(s[2] & 63) << 6) | ((int)(s[3] & 63) << 0);
                    ;
                    next = s + 4;
                }
                else
                {
                    *c = 0;
                    next = s + 1;
                }
            }
        }
    }
    if (*c >= 55296 && *c <= 57343)
    {
        *c = 0;
    }
    return next;
}

static unsigned char  is_hex_digit(unsigned char c)
{
    if (c >= 48 && c <= 57)
    {
        return 1;
    }
    else
    {
        if (c >= 97 && c <= 102)
        {
            return 1;
        }
        else
        {
            if (c >= 65 && c <= 70)
            {
                return 1;
            }
        }
    }
    return 0;
}

unsigned char *escape_sequences_decode_opt(unsigned char * p, unsigned int * out_value)
{
    if (*p == 120)
    {
        int result;

        p++;
        result = 0;
        while (is_hex_digit(*p))
        {
            int byte;

            byte = 0;
            if (*p >= 48 && *p <= 57)
            {
                byte = (*p - 48);
            }
            else
            {
                if (*p >= 97 && *p <= 102)
                {
                    byte = (*p - 97) + 10;
                }
                else
                {
                    if (*p >= 65 && *p <= 70)
                    {
                        byte = (*p - 65) + 10;
                    }
                }
            }
            result = (result << 4) | (byte & 15);
            p++;
        }
        *out_value = result;
    }
    else
    {
        if (*p == 117 || *p == 85)
        {
            int num_of_hex_digits;
            unsigned long long result;

            num_of_hex_digits = *p == 85 ? 8 : 4;
            p++;
            result = 0;
            {
                int i;
                i = 0;
                for (; i < num_of_hex_digits; i++)
                {
                    int byte;

                    byte = 0;
                    if (*p >= 48 && *p <= 57)
                    {
                        byte = (*p - 48);
                    }
                    else
                    {
                        if (*p >= 97 && *p <= 102)
                        {
                            byte = (*p - 97) + 10;
                        }
                        else
                        {
                            if (*p >= 65 && *p <= 70)
                            {
                                byte = (*p - 65) + 10;
                            }
                        }
                    }
                    result = (result << 4) | (byte & 15);
                    p++;
                }
            }
            *out_value = (int)result;
        }
        else
        {
            if (*p == 48)
            {
                int result;

                p++;
                result = 0;
                while ((*p >= 48 && *p <= 55))
                {
                    int byte;

                    byte = (*p - 48);
                    result = (result << 4) | (byte & 15);
                    p++;
                }
                *out_value = result;
            }
            else
            {
                /*switch*/
                {
                    register unsigned char _R2 = *p;
                    if (_R2 == 97) goto _CKL10; /*case 97*/
                    if (_R2 == 98) goto _CKL11; /*case 98*/
                    if (_R2 == 102) goto _CKL12; /*case 102*/
                    if (_R2 == 110) goto _CKL13; /*case 110*/
                    if (_R2 == 114) goto _CKL14; /*case 114*/
                    if (_R2 == 116) goto _CKL15; /*case 116*/
                    if (_R2 == 39) goto _CKL16; /*case 39*/
                    if (_R2 == 92) goto _CKL17; /*case 92*/
                    if (_R2 == 34) goto _CKL18; /*case 34*/
                    goto /*default*/ _CKL19;

                    {
                        /*case 97*/ _CKL10:
                        *out_value = 7;
                        /*break*/ goto _CKL9;

                        /*case 98*/ _CKL11:
                        *out_value = 8;
                        /*break*/ goto _CKL9;

                        /*case 102*/ _CKL12:
                        *out_value = 12;
                        /*break*/ goto _CKL9;

                        /*case 110*/ _CKL13:
                        *out_value = 10;
                        /*break*/ goto _CKL9;

                        /*case 114*/ _CKL14:
                        *out_value = 13;
                        /*break*/ goto _CKL9;

                        ;
                        /*case 116*/ _CKL15:
                        *out_value = 9;
                        /*break*/ goto _CKL9;

                        /*case 39*/ _CKL16:
                        *out_value = 39;
                        /*break*/ goto _CKL9;

                        /*case 92*/ _CKL17:
                        *out_value = 92;
                        /*break*/ goto _CKL9;

                        /*case 34*/ _CKL18:
                        *out_value = 34;
                        /*break*/ goto _CKL9;

                        /*default*/ _CKL19:
                        ;
                        return 0U;
                    }
                    _CKL9:;
                }
                p++;
            }
        }
    }
    return p;
}

