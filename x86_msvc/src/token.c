/* Cake 0.12.26 x86_msvc */
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

struct osstream {
    char * c_str;
    int size;
    int capacity;
};

struct stream {
    char * source;
    char * current;
    int line;
    int col;
    int line_continuation_count;
    char * path;
};

struct diagnostic {
    unsigned long long errors;
    unsigned long long warnings;
    unsigned long long notes;
};

struct token_list {
    struct token * head;
    struct token * tail;
};

struct diagnostic_stack {
    int top_index;
    struct diagnostic stack[10];
};

struct _iobuf {
    void * _Placeholder;
};

struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
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

struct tokenizer_ctx {
    struct options  options;
    int n_warnings;
    int n_errors;
};


static void _cake_zmem(void *dest, unsigned int len)
{
  unsigned char *ptr;

  ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

unsigned char token_is_blank(struct token * p);

unsigned char style_has_space(struct token * token)
{
    return token_is_blank(token->prev);
}


unsigned char style_has_one_space(struct token * token)
{
    return token->prev && token->prev->type == 143;
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
        p->next = 0;
        token_delete(p);
        p = next;
    }
    list->head = 0;
    list->tail = 0;
}


void token_range_add_show(struct token * first, struct token * last)
{
    {
        struct token * current;

        current = first;
        for (; current != last->next; current = current->next)
        {
            current->flags = current->flags & -65;
            if (current->next == 0)
            {
                break;
            }
        }
    }
}


void token_range_remove_flag(struct token * first, struct token * last, int flag)
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


void token_range_add_flag(struct token * first, struct token * last, int flag)
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
    if (list->head == 0)
    {
        return;
    }
    if (list->head == list->tail)
    {
        token_delete(list->head);
        list->head = 0;
        list->tail = 0;
    }
    else
    {
        ;
        ;
        list->tail = list->tail->prev;
        token_delete(list->tail->next);
        list->tail->next = 0;
        if (list->tail == list->head)
        {
            list->tail->prev = 0;
        }
    }
    ;
}


void token_list_pop_front(struct token_list * list)
{
    struct token * p;

    if (list->head == 0)
    {
        return;
    }
    p = list->head;
    ;
    if (list->head == list->tail)
    {
        list->head = 0;
        list->tail = 0;
    }
    else
    {
        list->head = p->next;
        if (list->head)
        {
            list->head->prev = 0;
        }
    }
    p->next = 0;
    p->prev = 0;
    token_delete(p);
    ;
}


struct token *token_list_pop_front_get(struct token_list * list)
{
    struct token * old_head;

    if (list->head == 0)
    {
        return 0;
    }
    old_head = list->head;
    list->head = old_head->next;
    if (list->head != 0)
    {
        list->head->prev = 0;
    }
    else
    {
        list->tail = 0;
    }
    ;
    old_head->prev = 0;
    old_head->next = 0;
    return old_head;
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
        p->next = 0;
        token_delete(p);
        p = next;
    }
}


int ss_fprintf(struct osstream * stream, char * fmt, ...);
void ss_close(struct osstream * stream);

char *token_list_join_tokens(struct token_list * list, unsigned char  bliteral)
{
    struct osstream  ss;
    unsigned char  has_space;
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
            has_space = ((current->flags & 4) != 0);
        }
    }
    if (bliteral)
    {
        ss_fprintf(&ss, "\"");
    }
    cstr = ss.c_str;
    ss.c_str = 0;
    ss_close(&ss);
    return cstr;
}


struct token_list tokenizer(struct tokenizer_ctx * ctx, char * text, char * filename_opt, int level, int addflags);
void token_list_insert_after(struct token_list * list, struct token * after, struct token_list * append);

void token_list_paste_string_after(struct token_list * list, struct token * after, char * s)
{
    struct tokenizer_ctx  tctx;
    struct token_list  l;

    _cake_zmem(&tctx, 696);
    l = tokenizer(&tctx, s, 0, 0, 1);
    token_list_insert_after(list, after, &l);
    token_list_destroy(&l);
}


void token_list_insert_before(struct token_list * token_list, struct token * after, struct token_list * append_list);

void token_list_paste_string_before(struct token_list * list, struct token * before, char * s)
{
    struct tokenizer_ctx  tctx;
    struct token_list  l;

    _cake_zmem(&tctx, 696);
    l = tokenizer(&tctx, s, 0, 0, 1);
    token_list_insert_before(list, before, &l);
    token_list_destroy(&l);
}


void token_list_insert_after(struct token_list * token_list, struct token * after, struct token_list * append_list)
{
    if (append_list->head == 0)
    {
        return;
    }
    if (token_list->head == 0)
    {
        ;
        token_list->head = append_list->head;
        token_list->tail = append_list->tail;
        append_list->head = 0;
        append_list->tail = 0;
        return;
    }
    if (after == 0)
    {
        ;
        ;
        append_list->tail->next = token_list->head;
        token_list->head->prev = append_list->tail;
        token_list->head = append_list->head;
        append_list->head->prev = 0;
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
    append_list->head = 0;
    append_list->tail = 0;
    ;
}


void token_list_insert_before(struct token_list * token_list, struct token * after, struct token_list * append_list)
{
    token_list_insert_after(token_list, after->prev, append_list);
}


int __cdecl strcmp(char * _Str1, char * _Str2);

unsigned char token_list_is_equal(struct token_list * list_a, struct token_list * list_b)
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
    return p_tka == 0 && p_tkb == 0;
}


struct token *token_list_add(struct token_list * list, struct token * pnew)
{
    ;
    ;
    if (list->head == 0)
    {
        pnew->prev = 0;
        pnew->next = 0;
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
    ;
    return list->tail;
}


int is_digit(struct stream * p)
{
    return (p->current[0] >= 48 && p->current[0] <= 57);
}


unsigned char token_is_identifier_or_keyword(int t)
{
    /*switch*/
    {
        int __v0 = t;
        if (__v0 == 8996) goto __L1; /*case 8996*/
        if (__v0 == 8999) goto __L2; /*case 8999*/
        if (__v0 == 9000) goto __L3; /*case 9000*/
        if (__v0 == 9001) goto __L4; /*case 9001*/
        if (__v0 == 9002) goto __L5; /*case 9002*/
        if (__v0 == 9003) goto __L6; /*case 9003*/
        if (__v0 == 9004) goto __L7; /*case 9004*/
        if (__v0 == 9005) goto __L8; /*case 9005*/
        if (__v0 == 9006) goto __L9; /*case 9006*/
        if (__v0 == 9007) goto __L10; /*case 9007*/
        if (__v0 == 9008) goto __L11; /*case 9008*/
        if (__v0 == 9009) goto __L12; /*case 9009*/
        if (__v0 == 9010) goto __L13; /*case 9010*/
        if (__v0 == 9011) goto __L14; /*case 9011*/
        if (__v0 == 9012) goto __L15; /*case 9012*/
        if (__v0 == 9013) goto __L16; /*case 9013*/
        if (__v0 == 9014) goto __L17; /*case 9014*/
        if (__v0 == 9015) goto __L18; /*case 9015*/
        if (__v0 == 9016) goto __L19; /*case 9016*/
        if (__v0 == 9017) goto __L20; /*case 9017*/
        if (__v0 == 9018) goto __L21; /*case 9018*/
        if (__v0 == 9019) goto __L22; /*case 9019*/
        if (__v0 == 9020) goto __L23; /*case 9020*/
        if (__v0 == 9021) goto __L24; /*case 9021*/
        if (__v0 == 9022) goto __L25; /*case 9022*/
        if (__v0 == 9023) goto __L26; /*case 9023*/
        if (__v0 == 9024) goto __L27; /*case 9024*/
        if (__v0 == 9025) goto __L28; /*case 9025*/
        if (__v0 == 9026) goto __L29; /*case 9026*/
        if (__v0 == 9027) goto __L30; /*case 9027*/
        if (__v0 == 9028) goto __L31; /*case 9028*/
        if (__v0 == 9029) goto __L32; /*case 9029*/
        if (__v0 == 9030) goto __L33; /*case 9030*/
        if (__v0 == 9032) goto __L34; /*case 9032*/
        if (__v0 == 9033) goto __L35; /*case 9033*/
        if (__v0 == 9034) goto __L36; /*case 9034*/
        if (__v0 == 9035) goto __L37; /*case 9035*/
        if (__v0 == 9036) goto __L38; /*case 9036*/
        if (__v0 == 9037) goto __L39; /*case 9037*/
        if (__v0 == 9038) goto __L40; /*case 9038*/
        if (__v0 == 9039) goto __L41; /*case 9039*/
        if (__v0 == 9040) goto __L42; /*case 9040*/
        if (__v0 == 9041) goto __L43; /*case 9041*/
        if (__v0 == 9042) goto __L44; /*case 9042*/
        if (__v0 == 9043) goto __L45; /*case 9043*/
        if (__v0 == 9044) goto __L46; /*case 9044*/
        if (__v0 == 9045) goto __L47; /*case 9045*/
        if (__v0 == 9059) goto __L48; /*case 9059*/
        if (__v0 == 9060) goto __L49; /*case 9060*/
        if (__v0 == 9061) goto __L50; /*case 9061*/
        if (__v0 == 9062) goto __L51; /*case 9062*/
        if (__v0 == 9063) goto __L52; /*case 9063*/
        if (__v0 == 9064) goto __L53; /*case 9064*/
        if (__v0 == 9065) goto __L54; /*case 9065*/
        if (__v0 == 9066) goto __L55; /*case 9066*/
        if (__v0 == 9067) goto __L56; /*case 9067*/
        if (__v0 == 9068) goto __L57; /*case 9068*/
        if (__v0 == 9069) goto __L58; /*case 9069*/
        if (__v0 == 9070) goto __L59; /*case 9070*/
        if (__v0 == 9071) goto __L60; /*case 9071*/
        if (__v0 == 9072) goto __L61; /*case 9072*/
        if (__v0 == 9073) goto __L62; /*case 9073*/
        if (__v0 == 9074) goto __L63; /*case 9074*/
        if (__v0 == 9075) goto __L64; /*case 9075*/
        if (__v0 == 9076) goto __L65; /*case 9076*/
        if (__v0 == 9077) goto __L66; /*case 9077*/
        if (__v0 == 9078) goto __L67; /*case 9078*/
        if (__v0 == 9079) goto __L68; /*case 9079*/
        if (__v0 == 9080) goto __L69; /*case 9080*/
        if (__v0 == 9081) goto __L70; /*case 9081*/
        if (__v0 == 9082) goto __L71; /*case 9082*/
        if (__v0 == 9083) goto __L72; /*case 9083*/
        if (__v0 == 9084) goto __L73; /*case 9084*/
        if (__v0 == 9085) goto __L74; /*case 9085*/
        if (__v0 == 9086) goto __L75; /*case 9086*/
        if (__v0 == 9087) goto __L76; /*case 9087*/
        if (__v0 == 9088) goto __L77; /*case 9088*/
        if (__v0 == 9089) goto __L78; /*case 9089*/
        if (__v0 == 9090) goto __L79; /*case 9090*/
        if (__v0 == 9091) goto __L80; /*case 9091*/
        if (__v0 == 9092) goto __L81; /*case 9092*/
        if (__v0 == 9093) goto __L82; /*case 9093*/
        if (__v0 == 9094) goto __L83; /*case 9094*/
        if (__v0 == 9095) goto __L84; /*case 9095*/
        goto __L85; /* default */

        {
            __L1: /*case 8996*/ 
            return 1;
            __L2: /*case 8999*/ 
            __L3: /*case 9000*/ 
            __L4: /*case 9001*/ 
            __L5: /*case 9002*/ 
            __L6: /*case 9003*/ 
            __L7: /*case 9004*/ 
            __L8: /*case 9005*/ 
            __L9: /*case 9006*/ 
            __L10: /*case 9007*/ 
            __L11: /*case 9008*/ 
            __L12: /*case 9009*/ 
            __L13: /*case 9010*/ 
            __L14: /*case 9011*/ 
            __L15: /*case 9012*/ 
            __L16: /*case 9013*/ 
            __L17: /*case 9014*/ 
            __L18: /*case 9015*/ 
            __L19: /*case 9016*/ 
            __L20: /*case 9017*/ 
            __L21: /*case 9018*/ 
            __L22: /*case 9019*/ 
            __L23: /*case 9020*/ 
            __L24: /*case 9021*/ 
            __L25: /*case 9022*/ 
            __L26: /*case 9023*/ 
            __L27: /*case 9024*/ 
            __L28: /*case 9025*/ 
            __L29: /*case 9026*/ 
            __L30: /*case 9027*/ 
            __L31: /*case 9028*/ 
            __L32: /*case 9029*/ 
            __L33: /*case 9030*/ 
            __L34: /*case 9032*/ 
            __L35: /*case 9033*/ 
            __L36: /*case 9034*/ 
            __L37: /*case 9035*/ 
            __L38: /*case 9036*/ 
            __L39: /*case 9037*/ 
            __L40: /*case 9038*/ 
            __L41: /*case 9039*/ 
            __L42: /*case 9040*/ 
            __L43: /*case 9041*/ 
            __L44: /*case 9042*/ 
            __L45: /*case 9043*/ 
            __L46: /*case 9044*/ 
            __L47: /*case 9045*/ 
            __L48: /*case 9059*/ 
            __L49: /*case 9060*/ 
            __L50: /*case 9061*/ 
            __L51: /*case 9062*/ 
            __L52: /*case 9063*/ 
            __L53: /*case 9064*/ 
            __L54: /*case 9065*/ 
            __L55: /*case 9066*/ 
            __L56: /*case 9067*/ 
            __L57: /*case 9068*/ 
            __L58: /*case 9069*/ 
            __L59: /*case 9070*/ 
            __L60: /*case 9071*/ 
            __L61: /*case 9072*/ 
            __L62: /*case 9073*/ 
            __L63: /*case 9074*/ 
            __L64: /*case 9075*/ 
            __L65: /*case 9076*/ 
            __L66: /*case 9077*/ 
            __L67: /*case 9078*/ 
            __L68: /*case 9079*/ 
            __L69: /*case 9080*/ 
            __L70: /*case 9081*/ 
            __L71: /*case 9082*/ 
            __L72: /*case 9083*/ 
            __L73: /*case 9084*/ 
            __L74: /*case 9085*/ 
            __L75: /*case 9086*/ 
            __L76: /*case 9087*/ 
            __L77: /*case 9088*/ 
            __L78: /*case 9089*/ 
            __L79: /*case 9090*/ 
            __L80: /*case 9091*/ 
            __L81: /*case 9092*/ 
            __L82: /*case 9093*/ 
            __L83: /*case 9094*/ 
            __L84: /*case 9095*/ 
            return 1;
            __L85: /* default */ 
            goto __L0; /* break */

        }
        __L0:;
    }
    return 0;
}


unsigned char token_is_blank(struct token * p)
{
    return p->type == 8998 || p->type == 143 || p->type == 132 || p->type == 133;
}


struct token *clone_token(struct token * p);

struct token *token_list_clone_and_add(struct token_list * list, struct token * pnew)
{
    struct token * clone;

    clone = clone_token(pnew);
    if (clone == 0)
    {
        return 0;
    }
    return token_list_add(list, clone);
}


void token_list_append_list_at_beginning(struct token_list * dest, struct token_list * source)
{
    if (source->head == 0)
    {
        return;
    }
    if (dest->head == 0)
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
    source->head = 0;
    source->tail = 0;
    ;
}


void token_list_append_list(struct token_list * dest, struct token_list * source)
{
    if (source->head == 0)
    {
        return;
    }
    if (dest->head == 0)
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
    source->head = 0;
    source->tail = 0;
    ;
}


void *calloc(unsigned int nmemb, unsigned int size);
char *strdup(char * src);

struct token *clone_token(struct token * p)
{
    struct token * token;
    char * lexeme;

    token = calloc(1, 40);
    if (token == 0)
    {
        return 0;
    }
    lexeme = strdup(p->lexeme);
    if (lexeme == 0)
    {
        free(token);
        return 0;
    }
    *token = *p;
    token->lexeme = lexeme;
    token->next = 0;
    token->prev = 0;
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
        list->tail = 0;
    }
    last->next = 0;
    r.head = (struct token *)first;
    first->prev = 0;
    r.tail = last;
    return r;
}


void token_list_remove(struct token_list * list, struct token * first, struct token * last)
{
    struct token_list  r;

    r = token_list_remove_get(list, first, last);
    token_list_destroy(&r);
}


unsigned char token_list_is_empty(struct token_list * p)
{
    ;
    return p->head == 0;
}


static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
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



static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &__c0__OptionsStorage;
}

static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}

static int __cdecl printf(char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}
void print_literal2(char * s)
{
    while (*s)
    {
        /*switch*/
        {
            char __v0 = *s;
            if (__v0 == 10) goto __L1; /*case 10*/
            goto __L2; /* default */

            {
                __L1: /*case 10*/ 
                printf("\\n");
                goto __L0; /* break */

                __L2: /* default */ 
                printf("%c", *s);
            }
            __L0:;
        }
        s++;
    }
}


static int __cdecl snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...);
static int __cdecl vsnprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, char * _ArgList);
int __cdecl __stdio_common_vsprintf(unsigned long long _Options, char * _Buffer, unsigned int _BufferCount, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
char *get_token_name(int tk);
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
    snprintf(buffer0, 50, "%d:%d", p_token->line, p_token->col);
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
void print_tokens(struct token * p_token)
{
    struct token * current;

    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\x1b[0m");
    current = p_token;
    while (current)
    {
        print_token(current);
        current = current->next;
    }
    printf("\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\x1b[0m");
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


void print_position(char * path, int line, int col, unsigned char  visual_studio_ouput_format)
{
    if (visual_studio_ouput_format)
    {
        printf("%s(%d,%d): ", path ? path : "<>", line, col);
    }
    else
    {
        printf("\x1b[97m%s:%d:%d: ", path ? path : "<>", line, col);
    }
}


int __cdecl putc(int _Character, struct _iobuf * _Stream);

void print_line_and_token(struct marker * p_marker, unsigned char  visual_studio_ouput_format)
{
    if (1) /*try*/
    {
        struct token * p_token;
        int line;
        char nbuffer[20];
        int n;
        struct token * p_line_begin;
        struct token * p_token_begin;
        struct token * p_token_end;
        unsigned char  expand_macro;
        struct token * p_item;
        unsigned char  complete;
        int start_col;
        int end_col;
        unsigned char  onoff;

        p_token = p_marker->p_token_caret ? p_marker->p_token_caret : p_marker->p_token_begin;
        if (p_token == 0)
        {
            goto __L0; /* throw */
        }
        line = p_marker->line;
        if (!visual_studio_ouput_format)
        {
            printf("\x1b[0m");
        }
        _cake_zmem(&nbuffer, 20);
        n = snprintf(nbuffer, 20, "%d", line);
        printf(" %s |", nbuffer);
        p_line_begin = p_token;
        while (p_line_begin->prev && (p_line_begin->prev->type != 10 && p_line_begin->prev->type != 8998))
        {
            p_line_begin = p_line_begin->prev;
        }
        p_token_begin = p_marker->p_token_begin ? p_marker->p_token_begin : p_marker->p_token_caret;
        p_token_end = p_marker->p_token_end ? p_marker->p_token_end : p_marker->p_token_caret;
        if (p_token_begin == 0)
        {
            goto __L0; /* throw */
        }
        expand_macro = ((p_token_begin->flags & 2) != 0);
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
                    if (p_item->type >= 8999 && p_item->type <= 9095)
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
        if (p_item == 0)
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
    else __L0: /*catch*/ 
    {
    }
}


void stream_match(struct stream * stream);
static void digit_sequence_opt(struct stream * stream);
static void exponent_part_opt(struct stream * stream);
static void floating_suffix_opt(struct stream * stream, char suffix[4]);
static unsigned char is_hexadecimal_digit(struct stream * stream);
static void integer_suffix_opt(struct stream * stream, char suffix[4]);
static void hexadecimal_digit_sequence(struct stream * stream);
static void binary_exponent_part(struct stream * stream);
static unsigned char is_binary_digit(struct stream * stream);
static unsigned char is_octal_digit(struct stream * stream);
static unsigned char is_nonzero_digit(struct stream * stream);

int parse_number_core(struct stream * stream, char suffix[4], char errmsg[100])
{
    int type;

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
                            type = 140;
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



static void digit_sequence_opt(struct stream * stream)
{
    while (is_digit(stream))
    {
        stream_match(stream);
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

static unsigned char is_hexadecimal_digit(struct stream * stream)
{
    return (stream->current[0] >= 48 && stream->current[0] <= 57) || (stream->current[0] >= 97 && stream->current[0] <= 102) || (stream->current[0] >= 65 && stream->current[0] <= 70);
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

static void binary_exponent_part(struct stream * stream)
{
    stream_match(stream);
    if (stream->current[0] == 43 || stream->current[0] == 45)
    {
        stream_match(stream);
    }
    digit_sequence_opt(stream);
}

static unsigned char is_binary_digit(struct stream * stream)
{
    return stream->current[0] >= 48 && stream->current[0] <= 49;
}

static unsigned char is_octal_digit(struct stream * stream)
{
    return stream->current[0] >= 48 && stream->current[0] <= 55;
}

static unsigned char is_nonzero_digit(struct stream * stream)
{
    return stream->current[0] >= 49 && stream->current[0] <= 57;
}
int parse_number(char * lexeme, char suffix[4], char errmsg[100])
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
        return 0;
    }
    next = 0;
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


static unsigned char is_hex_digit(unsigned char c);

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
                    unsigned char __v0 = *p;
                    if (__v0 == 97) goto __L10; /*case 97*/
                    if (__v0 == 98) goto __L11; /*case 98*/
                    if (__v0 == 102) goto __L12; /*case 102*/
                    if (__v0 == 110) goto __L13; /*case 110*/
                    if (__v0 == 114) goto __L14; /*case 114*/
                    if (__v0 == 116) goto __L15; /*case 116*/
                    if (__v0 == 118) goto __L16; /*case 118*/
                    if (__v0 == 39) goto __L17; /*case 39*/
                    if (__v0 == 92) goto __L18; /*case 92*/
                    if (__v0 == 34) goto __L19; /*case 34*/
                    goto __L20; /* default */

                    {
                        __L10: /*case 97*/ 
                        *out_value = 7;
                        goto __L9; /* break */

                        __L11: /*case 98*/ 
                        *out_value = 8;
                        goto __L9; /* break */

                        __L12: /*case 102*/ 
                        *out_value = 12;
                        goto __L9; /* break */

                        __L13: /*case 110*/ 
                        *out_value = 10;
                        goto __L9; /* break */

                        __L14: /*case 114*/ 
                        *out_value = 13;
                        goto __L9; /* break */

                        ;
                        __L15: /*case 116*/ 
                        *out_value = 9;
                        goto __L9; /* break */

                        __L16: /*case 118*/ 
                        *out_value = 11;
                        goto __L9; /* break */

                        __L17: /*case 39*/ 
                        *out_value = 39;
                        goto __L9; /* break */

                        __L18: /*case 92*/ 
                        *out_value = 92;
                        goto __L9; /* break */

                        __L19: /*case 34*/ 
                        *out_value = 34;
                        goto __L9; /* break */

                        __L20: /* default */ 
                        ;
                        return 0;
                    }
                    __L9:;
                }
                p++;
            }
        }
    }
    return p;
}



static unsigned char is_hex_digit(unsigned char c)
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
