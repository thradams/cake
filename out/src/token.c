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

struct tokenizer_ctx {
    struct options  options;
    int n_warnings;
    int n_errors;
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

struct osstream {
    char * c_str;
    int size;
    int capacity;
};

struct token_list {
    struct token * head;
    struct token * tail;
};

struct _iobuf {
    void * _Placeholder;
};

struct __crt_locale_data;
struct stream {
    char * source;
    char * current;
    int line;
    int col;
    int line_continuation_count;
    char * path;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct __crt_multibyte_data;
struct marker {
    char * file;
    int line;
    int start_col;
    int end_col;
    struct token * p_token_caret;
    struct token * p_token_begin;
    struct token * p_token_end;
};


unsigned char  token_is_blank(struct token * p);

unsigned char  style_has_space(struct token * token)
{
    return token_is_blank(token->prev);
}

unsigned char  style_has_one_space(struct token * token)
{
    return token->prev && token->prev->type == 143;
}

void token_delete(struct token * p);

void token_list_clear(struct token_list * list)
{
    struct token * p = list->head;
    while (p)
    {
        struct token * next = p->next;
        p->next = ((void *)0);
        token_delete(p);
        p = next;
    }
    list->head = ((void *)0);
    list->tail = ((void *)0);
}

void token_range_add_show(struct token * first, struct token * last)
{
    {
        struct token * current = first;
        for (; current != last->next; current = current->next)
        {
            current->flags = current->flags &  ~64;
            if (current->next == ((void *)0))
            {
                break;
            }
        }
    }
}

void token_range_remove_flag(struct token * first, struct token * last, int   flag)
{
    {
        struct token * current = first;
        for (; current && current != last->next; current = current->next)
        {
            current->flags = current->flags &  ~flag;
        }
    }
}

void token_range_add_flag(struct token * first, struct token * last, int   flag)
{
    {
        struct token * current = first;
        for (; current && current != last->next; current = current->next)
        {
            current->flags = flag;
        }
    }
}

void token_list_pop_back(struct token_list * list)
{
    if (list->head == ((void *)0))
    {
        return;
    }
    if (list->head == list->tail)
    {
        token_delete(list->head);
        list->head = ((void *)0);
        list->tail = ((void *)0);
    }
    else
    {
        ;
        ;
        list->tail = list->tail->prev;
        token_delete(list->tail->next);
        list->tail->next = ((void *)0);
        if (list->tail == list->head)
        {
            list->tail->prev = ((void *)0);
        }
    }
}

void token_list_pop_front(struct token_list * list)
{
    if (list->head == ((void *)0))
    {
        return;
    }
    struct token * p = list->head;
    if (list->head == list->tail)
    {
        list->head = ((void *)0);
        list->tail = ((void *)0);
    }
    else
    {
        list->head = p->next;
    }
    p->next = ((void *)0);
    p->prev = ((void *)0);
    token_delete(p);
}

struct token *token_list_pop_front_get(struct token_list * list)
{
    if (list->head == ((void *)0))
    {
        return ((void *)0);
    }
    struct token * head = list->head;
    if (list->head == list->tail)
    {
        list->head = ((void *)0);
        list->tail = ((void *)0);
        head->next = ((void *)0);
        head->prev = ((void *)0);
        return head;
    }
    list->head = head->next;
    head->next = ((void *)0);
    head->prev = ((void *)0);
    return head;
}

void token_list_swap(struct token_list * a, struct token_list * b)
{
    struct token_list  temp =  *a;
     *a =  *b;
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
    struct token * p = list->head;
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
    struct token * p = list->head;
    while (p)
    {
        struct token * next = p->next;
        p->next = ((void *)0);
        token_delete(p);
        p = next;
    }
}

int ss_fprintf(struct osstream * stream, char * fmt, ...);
void ss_close(struct osstream * stream);

char *token_list_join_tokens(struct token_list * list, unsigned char   bliteral)
{
    struct osstream  ss = {0};
    if (bliteral)
    {
        ss_fprintf(&ss, "\"");
    }
    unsigned char   has_space = 0;
    struct token * current = list->head;
    while (current)
    {
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
        char * p = current->lexeme;
        while ( *p)
        {
            if ( *p == 34)
            {
                ss_fprintf(&ss, "\\\"");
            }
            else
            {
                ss_fprintf(&ss, "%c",  *p);
            }
            p++;
        }
        current = current->next;
        if (current)
        {
            has_space = current->flags & 4;
        }
    }
    if (bliteral)
    {
        ss_fprintf(&ss, "\"");
    }
    char * cstr = ss.c_str;
    ss.c_str = ((void *)0);
    ss_close(&ss);
    return cstr;
}

struct token_list tokenizer(struct tokenizer_ctx * ctx, char * text, char * filename_opt, int level, int   addflags);
void token_list_insert_after(struct token_list * list, struct token * after, struct token_list * append);

void token_list_paste_string_after(struct token_list * list, struct token * after, char * s)
{
    struct tokenizer_ctx  tctx = {0};
    struct token_list  l = tokenizer(&tctx, s, ((void *)0), 0, 1);
    token_list_insert_after(list, after, &l);
    token_list_destroy(&l);
}

void token_list_insert_before(struct token_list * token_list, struct token * after, struct token_list * append_list);

void token_list_paste_string_before(struct token_list * list, struct token * before, char * s)
{
    struct tokenizer_ctx  tctx = {0};
    struct token_list  l = tokenizer(&tctx, s, ((void *)0), 0, 1);
    token_list_insert_before(list, before, &l);
    token_list_destroy(&l);
}

void token_list_insert_after(struct token_list * token_list, struct token * after, struct token_list * append_list)
{
    if (append_list->head == ((void *)0))
    {
        return;
    }
    if (token_list->head == ((void *)0))
    {
        ;
        token_list->head = append_list->head;
        token_list->tail = append_list->tail;
        append_list->head = ((void *)0);
        append_list->tail = ((void *)0);
        return;
    }
    if (after == ((void *)0))
    {
        ;
        ;
        append_list->tail->next = token_list->head;
        token_list->head->prev = append_list->tail;
        token_list->head = append_list->head;
        append_list->head->prev = ((void *)0);
    }
    else
    {
        struct token * follow = after->next;
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
    append_list->head = ((void *)0);
    append_list->tail = ((void *)0);
}

void token_list_insert_before(struct token_list * token_list, struct token * after, struct token_list * append_list)
{
    token_list_insert_after(token_list, after->prev, append_list);
}

struct token *token_list_add(struct token_list * list, struct token * pnew)
{
    ;
    ;
    if (list->head == ((void *)0))
    {
        pnew->prev = ((void *)0);
        pnew->next = ((void *)0);
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

unsigned char  token_is_identifier_or_keyword(int   t)
{
    switch (t)
    {
        case 8996 :
        return 1;
        case 8999 :
        case 9000 :
        case 9001 :
        case 9002 :
        case 9003 :
        case 9004 :
        case 9005 :
        case 9006 :
        case 9007 :
        case 9008 :
        case 9009 :
        case 9010 :
        case 9011 :
        case 9012 :
        case 9013 :
        case 9014 :
        case 9015 :
        case 9016 :
        case 9017 :
        case 9018 :
        case 9019 :
        case 9020 :
        case 9021 :
        case 9022 :
        case 9023 :
        case 9024 :
        case 9025 :
        case 9026 :
        case 9027 :
        case 9028 :
        case 9029 :
        case 9030 :
        case 9032 :
        case 9033 :
        case 9034 :
        case 9035 :
        case 9036 :
        case 9037 :
        case 9038 :
        case 9039 :
        case 9040 :
        case 9041 :
        case 9042 :
        case 9043 :
        case 9044 :
        case 9045 :
        case 9046 :
        case 9047 :
        case 9048 :
        case 9049 :
        case 9050 :
        case 9051 :
        case 9052 :
        case 9053 :
        case 9054 :
        case 9055 :
        case 9056 :
        case 9057 :
        case 9058 :
        case 9059 :
        case 9060 :
        case 9061 :
        case 9062 :
        case 9063 :
        case 9064 :
        case 9065 :
        case 9066 :
        case 9067 :
        case 9068 :
        case 9069 :
        case 9070 :
        case 9071 :
        case 9072 :
        case 9073 :
        case 9074 :
        case 9075 :
        case 9076 :
        case 9077 :
        case 9078 :
        case 9079 :
        case 9080 :
        case 9081 :
        case 9082 :
        return 1;
        default:
        break;
    }
    return 0;
}

unsigned char  token_is_blank(struct token * p)
{
    return p->type == 8998 || p->type == 143 || p->type == 132 || p->type == 133;
}

struct token *clone_token(struct token * p);

struct token *token_list_clone_and_add(struct token_list * list, struct token * pnew)
{
    struct token * clone = clone_token(pnew);
    if (clone == ((void *)0))
    {
        return ((void *)0);
    }
    return token_list_add(list, clone);
}

void token_list_append_list_at_beginning(struct token_list * dest, struct token_list * source)
{
    if (source->head == ((void *)0))
    {
        return;
    }
    if (dest->head == ((void *)0))
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
    source->head = ((void *)0);
    source->tail = ((void *)0);
}

void token_list_append_list(struct token_list * dest, struct token_list * source)
{
    if (source->head == ((void *)0))
    {
        return;
    }
    if (dest->head == ((void *)0))
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
    source->head = ((void *)0);
    source->tail = ((void *)0);
}

void *calloc(int nmemb, unsigned int size);
char *strdup(char * src);

struct token *clone_token(struct token * p)
{
    struct token * token = calloc(1, sizeof  *token);
    if (token == ((void *)0))
    {
        return ((void *)0);
    }
    char * lexeme = strdup(p->lexeme);
    if (lexeme == ((void *)0))
    {
        free(token);
        return ((void *)0);
    }
     *token =  *p;
    token->lexeme = lexeme;
    token->next = ((void *)0);
    token->prev = ((void *)0);
    return token;
}

struct token_list token_list_remove_get(struct token_list * list, struct token * first, struct token * last)
{
    struct token_list  r = {0};
    struct token * before_first = first->prev;
    struct token * after_last = last->next;
    last->next = ((void *)0);
    if (before_first)
    {
        before_first->next = after_last;
    }
    if (after_last)
    {
        after_last->prev = before_first;
    }
    r.head = (struct token *)first;
    first->prev = ((void *)0);
    r.tail = last;
    return r;
}

void token_list_remove(struct token_list * list, struct token * first, struct token * last)
{
    struct token_list  r = token_list_remove_get(list, first, last);
    token_list_destroy(&r);
}

unsigned char  token_list_is_empty(struct token_list * p)
{
    ;
    return p->head == ((void *)0);
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
void print_literal2(char * s);

void print_list(struct token_list * list)
{
    struct token * current = list->head;
    while (current)
    {
        if (current != list->head)
        {
            printf("\xcb\xb0");
        }
        print_literal2(current->lexeme);
        printf("\x1b");
        if (current == list->tail)
        {
        }
        current = current->next;
    }
    printf("\n");
}

void print_literal2(char * s)
{
    while ( *s)
    {
        switch ( *s)
        {
            case 10 :
            printf("\\n");
            break;
            default:
            printf("%c",  *s);
        }
        s++;
    }
}

int snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...);
char *get_token_name(int   tk);
char *strcat(char * _Destination, char * _Source);

void print_token(struct token * p_token)
{
    {
        int i = 0;
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
    char buffer0[50] = {0};
    snprintf(buffer0, sizeof buffer0, "%d:%d", p_token->line, p_token->col);
    printf("%-6s ", buffer0);
    printf("%-20s ", get_token_name(p_token->type));
    if (p_token->flags & 2)
    {
        printf("\x1b[36;1m");
    }
    char buffer[50] = {0};
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
    printf("\x1b");
}

void print_tokens(struct token * p_token)
{
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    struct token * current = p_token;
    while (current)
    {
        print_token(current);
        current = current->next;
    }
    printf("\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("\x1b");
}

void print_token_html(struct token * p_token)
{
    printf("<span class=\"");
    if ( !(p_token->flags & 1))
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
    printf("<pre>\n");
    struct token * current = p_token;
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
        printf("\x1b[97m", path ? path : "<>", line, col);
    }
}

int putc(int _Character, struct _iobuf * _Stream);

void print_line_and_token(struct marker * p_marker, unsigned char   visual_studio_ouput_format)
{
    if (1)
    {
        struct token * p_token = p_marker->p_token_caret ? p_marker->p_token_caret : p_marker->p_token_begin;
        if (p_token == ((void *)0))
        {
            goto _catch_label_1;
        }
        int line = p_marker->line;
        if ( !visual_studio_ouput_format)
        {
            printf("\x1b");
        }
        char nbuffer[20] = {0};
        int n = snprintf(nbuffer, sizeof nbuffer, "%d", line);
        printf(" %s |", nbuffer);
        struct token * p_line_begin = p_token;
        while (p_line_begin->prev && (p_line_begin->prev->type != 10 && p_line_begin->prev->type != 8998))
        {
            p_line_begin = p_line_begin->prev;
        }
        struct token * p_token_begin = p_marker->p_token_begin ? p_marker->p_token_begin : p_marker->p_token_caret;
        struct token * p_token_end = p_marker->p_token_end ? p_marker->p_token_end : p_marker->p_token_caret;
        if (p_token_begin == ((void *)0))
        {
            goto _catch_label_1;
        }
        unsigned char   expand_macro = p_token_begin->flags & 2;
        if ( !visual_studio_ouput_format)
        {
            printf("\x1b[34;1m");
        }
        struct token * p_item = p_line_begin;
        while (p_item)
        {
            if ( !visual_studio_ouput_format)
            {
                if (p_item->flags & 2)
                {
                    printf("\x1b[90m");
                }
                else
                {
                    if (p_item->type >= 8999 && p_item->type <= 9082)
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
            if ( !(p_item->flags & 2) || expand_macro)
            {
                char * p = p_item->lexeme;
                while ( *p)
                {
                    putc( *p, (__acrt_iob_func(1)));
                    p++;
                }
            }
            if ( !visual_studio_ouput_format)
            {
                printf("\x1b");
            }
            if (p_item->type == 10)
            {
                break;
            }
            p_item = p_item->next;
        }
        if ( !visual_studio_ouput_format)
        {
            printf("\x1b");
        }
        if (p_item == ((void *)0))
        {
            printf("\n");
        }
        printf(" %*s |", n, " ");
        unsigned char   complete = 0;
        int start_col = 1;
        int end_col = 1;
        unsigned char   onoff = 0;
        p_item = p_line_begin;
        while (p_item)
        {
            if (p_item == p_token_begin)
            {
                if ( !visual_studio_ouput_format)
                {
                    printf("\x1b[92m");
                }
                onoff = 1;
                end_col = start_col;
            }
            if ( !(p_item->flags & 2) || expand_macro)
            {
                char * p = p_item->lexeme;
                while ( *p)
                {
                    if (onoff)
                    {
                        putc(126, (__acrt_iob_func(1)));
                        end_col++;
                    }
                    else
                    {
                        putc(32, (__acrt_iob_func(1)));
                        if ( !complete)
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
                if ( !visual_studio_ouput_format)
                {
                    printf("\x1b");
                }
            }
            p_item = p_item->next;
        }
        if ( !visual_studio_ouput_format)
        {
            printf("\x1b");
        }
        printf("\n");
        p_marker->start_col = start_col;
        p_marker->end_col = end_col;
    }
    else _catch_label_1:
    {
    }
}

int is_digit(struct stream * p);
void stream_match(struct stream * stream);

static void digit_sequence(struct stream * stream)
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
    digit_sequence(stream);
}

static unsigned char  is_hexadecimal_digit(struct stream * stream)
{
    return (stream->current[0] >= 48 && stream->current[0] <= 57) || (stream->current[0] >= 97 && stream->current[0] <= 102) || (stream->current[0] >= 65 && stream->current[0] <= 70);
}

static unsigned char  is_octal_digit(struct stream * stream)
{
    return stream->current[0] >= 48 && stream->current[0] <= 55;
}

static void hexadecimal_digit_sequence(struct stream * stream)
{
    stream_match(stream);
    while (stream->current[0] == 39 || is_hexadecimal_digit(stream))
    {
        if (stream->current[0] == 39)
        {
            stream_match(stream);
            if ( !is_hexadecimal_digit(stream))
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
        digit_sequence(stream);
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
    return stream->current[0] >= 48 && stream->current[0] <= 49;
}

static unsigned char  is_nonzero_digit(struct stream * stream)
{
    return stream->current[0] >= 49 && stream->current[0] <= 57;
}

int  parse_number_core(struct stream * stream, char suffix[4], char errmsg[100])
{
    errmsg[0] = 0;
    int   type = 0;
    if (stream->current[0] == 46)
    {
        type = 140;
        stream_match(stream);
        digit_sequence(stream);
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
                                digit_sequence(stream);
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
    struct stream  stream = {0, 0, 0, 0, 0, 0};
    stream.source = lexeme;
    stream.current = lexeme;
    stream.line = 1;
    stream.col = 1;
    stream.path = "";
    return parse_number_core(&stream, suffix, errmsg);
}

unsigned char *utf8_decode(unsigned char * s, unsigned int * c)
{
     *c = 0;
    if (s[0] == 0)
    {
         *c = 0;
        return ((void *)0);
    }
    unsigned char * next = ((void *)0);
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
    if ( *c >= 55296 &&  *c <= 57343)
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
    if ( *p == 120)
    {
        p++;
        int result = 0;
        while (is_hex_digit( *p))
        {
            int byte = 0;
            if ( *p >= 48 &&  *p <= 57)
            {
                byte = ( *p - 48);
            }
            else
            {
                if ( *p >= 97 &&  *p <= 102)
                {
                    byte = ( *p - 97) + 10;
                }
                else
                {
                    if ( *p >= 65 &&  *p <= 70)
                    {
                        byte = ( *p - 65) + 10;
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
        if ( *p == 117 ||  *p == 85)
        {
            int num_of_hex_digits =  *p == 85 ? 8 : 4;
            p++;
            unsigned long long result = 0;
            {
                int i = 0;
                for (; i < num_of_hex_digits; i++)
                {
                    int byte = 0;
                    if ( *p >= 48 &&  *p <= 57)
                    {
                        byte = ( *p - 48);
                    }
                    else
                    {
                        if ( *p >= 97 &&  *p <= 102)
                        {
                            byte = ( *p - 97) + 10;
                        }
                        else
                        {
                            if ( *p >= 65 &&  *p <= 70)
                            {
                                byte = ( *p - 65) + 10;
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
            if ( *p == 48)
            {
                p++;
                int result = 0;
                while (( *p >= 48 &&  *p <= 55))
                {
                    int byte;
                    byte = ( *p - 48);
                    result = (result << 4) | (byte & 15);
                    p++;
                }
                 *out_value = result;
            }
            else
            {
                switch ( *p)
                {
                    case 97 :
                     *out_value = 7;
                    break;
                    case 98 :
                     *out_value = 8;
                    break;
                    case 102 :
                     *out_value = 12;
                    break;
                    case 110 :
                     *out_value = 10;
                    break;
                    case 114 :
                     *out_value = 13;
                    break;
                    ;
                    case 116 :
                     *out_value = 9;
                    break;
                    case 39 :
                     *out_value = 39;
                    break;
                    case 92 :
                     *out_value = 92;
                    break;
                    case 34 :
                     *out_value = 34;
                    break;
                    default:
                    ;
                    return ((void *)0);
                }
                p++;
            }
        }
    }
    return p;
}

