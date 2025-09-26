// Cake 0.12.05 target=x86_msvc
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

struct osstream {
    char * c_str;
    int size;
    int capacity;
};

struct macro_expanded {
    char * name;
    struct macro_expanded * p_previous;
};

struct macro_argument_list {
    struct token_list  tokens;
    struct macro_argument * head;
    struct macro_argument * tail;
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

struct macro_parameter {
    char * name;
    struct macro_parameter * next;
    struct token_list  expanded_list;
    unsigned char already_expanded;
};

struct stream {
    char * source;
    char * current;
    int line;
    int col;
    int line_continuation_count;
    char * path;
};

struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};

struct tokenizer_ctx {
    struct options  options;
    int n_warnings;
    int n_errors;
};

struct _iobuf {
    void * _Placeholder;
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
    int type;
    union _struct_tag_5  data;
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

struct include_dir {
    char * path;
    struct include_dir * next;
};

struct macro {
    struct token * p_name_token;
    char * name;
    struct token_list  replacement_list;
    struct macro_parameter * parameters;
    unsigned char is_function;
    int usage;
    unsigned char def_macro;
};

struct macro_argument {
    struct macro_parameter * macro_parameter;
    struct token_list  tokens;
    struct macro_argument * next;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

static void _cake_memcpy(void * dest, const void * src, unsigned int n)
{
  char *csrc = (char *)src;
  char *cdest = (char *)dest;
  unsigned int i; 
  for (i = 0; i < n; i++) cdest[i] = csrc[i]; 
}

void free(void * ptr);

void include_dir_list_destroy(struct include_dir_list * list)
{
    struct include_dir * p;

    p = list->head;
    while (p)
    {
        struct include_dir * next;

        next = p->next;
        free((void *)p->path);
        free(p);
        p = next;
    }
}


void hashmap_destroy(struct hash_map * map);
void token_list_destroy(struct token_list * list);

void preprocessor_ctx_destroy(struct preprocessor_ctx * p)
{
    hashmap_destroy(&p->macros);
    include_dir_list_destroy(&p->include_dir);
    hashmap_destroy(&p->pragma_once_map);
    token_list_destroy(&p->input_list);
}


unsigned char preprocessor_diagnostic(int w, struct preprocessor_ctx * ctx, struct token * p_token, char * fmt, ...);

void pre_unexpected_end_of_file(struct token * p_token, struct preprocessor_ctx * ctx)
{
    preprocessor_diagnostic(970, ctx, p_token, "unexpected end of file");
}


void print_position(char * path, int line, int col, unsigned char msvc_format);
static int __cdecl vsnprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, char * _ArgList);
int __cdecl __stdio_common_vsprintf(unsigned long long _Options, char * _Buffer, unsigned int _BufferCount, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __Ck0__OptionsStorage;
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
void print_line_and_token(struct marker * p_marker, unsigned char visual_studio_ouput_format);

unsigned char preprocessor_diagnostic(int w, struct preprocessor_ctx * ctx, struct token * p_token_opt, char * fmt, ...)
{
    struct marker  marker;
    unsigned char included_file_location;
    unsigned char is_error;
    unsigned char is_warning;
    unsigned char is_note;
    char buffer[200];
    char * args;

    _cake_zmem(&marker, 28);
    if (p_token_opt == 0U)
    {
        return 0;
    }
    marker.file = p_token_opt->token_origin->lexeme;
    marker.line = p_token_opt->line;
    marker.start_col = p_token_opt->col;
    marker.end_col = p_token_opt->col;
    marker.p_token_caret = p_token_opt;
    included_file_location = p_token_opt->level > 0;
    is_error = 0;
    is_warning = 0;
    is_note = 0;
    if (w > 63)
    {
        is_error = 1;
    }
    else
    {
        is_error = (ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors & (1ULL << w)) != 0;
        is_warning = (ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings & (1ULL << w)) != 0;
        is_note = ((ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes & (1ULL << w)) != 0);
    }
    if (is_error)
    {
        ctx->n_errors++;
    }
    else
    {
        if (is_warning)
        {
            ctx->n_warnings++;
        }
        else
        {
            if (is_note)
            {
            }
            else
            {
                return 0;
            }
        }
    }
    if (!is_error && included_file_location)
    {
        return 0;
    }
    print_position(marker.file, marker.line, marker.start_col, ctx->options.visual_studio_ouput_format);
    _cake_zmem(&buffer, 200);
    _cake_zmem(&args, 4);
    ((void)(args = (char *)(&(fmt)) + 4U));
    vsnprintf(buffer, 200U, fmt, args);
    ((void)(args = 0U));
    if (ctx->options.visual_studio_ouput_format)
    {
        if (is_warning)
        {
            printf("warning: %s\n", buffer);
        }
        else
        {
            if (is_error)
            {
                printf("warning: %s\n", buffer);
            }
            else
            {
                if (is_note)
                {
                    printf("note: %s\n", buffer);
                }
            }
        }
        print_line_and_token(&marker, ctx->options.visual_studio_ouput_format);
    }
    else
    {
        if (is_error)
        {
            printf("\x1b[91merror: \x1b[97m%s\n", buffer);
        }
        else
        {
            if (is_warning)
            {
                printf("\x1b[95mwarning: \x1b[97m%s\n", buffer);
            }
            else
            {
                if (is_note)
                {
                    printf("\x1b[36;1mnote: \x1b[97m%s\n", buffer);
                }
            }
        }
        print_line_and_token(&marker, ctx->options.visual_studio_ouput_format);
    }
    return 1;
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

static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}

static int __cdecl printf(char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4U));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0U, _ArgList);
    ((void)(_ArgList = 0U));
    return _Result;
}
void *calloc(unsigned int nmemb, unsigned int size);
unsigned int __cdecl strlen(char * _Str);
char *strdup(char * src);
static int __cdecl snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...);

struct include_dir *include_dir_add(struct include_dir_list * list, char * path)
{
    if (1) /*try*/
    {
        struct include_dir * p_new_include_dir;
        int len;

        p_new_include_dir = calloc(1, 8U);
        if (p_new_include_dir == 0U)
        {
            goto _CKL0;/*throw*/
        }
        len = strlen(path);
        if (path[len - 1] == 92)
        {
            char * temp;

            temp = strdup(path);
            if (temp == 0U)
            {
                free(p_new_include_dir);
                goto _CKL0;/*throw*/
            }
            p_new_include_dir->path = temp;
        }
        else
        {
            if (path[len - 1] != 47)
            {
                char * temp;

                temp = calloc(len + 2, 1U);
                if (temp == 0U)
                {
                    free(p_new_include_dir);
                    goto _CKL0;/*throw*/
                }
                p_new_include_dir->path = temp;
                snprintf((char *)p_new_include_dir->path, len + 2, "%s/", path);
            }
            else
            {
                char * temp;

                temp = strdup(path);
                if (temp == 0U)
                {
                    free(p_new_include_dir);
                    goto _CKL0;/*throw*/
                }
                p_new_include_dir->path = temp;
            }
        }
        if (list->head == 0U)
        {
            list->head = p_new_include_dir;
            list->tail = p_new_include_dir;
        }
        else
        {
            ;
            ;
            list->tail->next = p_new_include_dir;
            list->tail = p_new_include_dir;
        }
        return list->tail;
    }
    else _CKL0: /*catch*/ 
    {
    }
    return 0U;
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
unsigned char path_is_absolute(char * path);
void path_normalize(char * path);
static unsigned char pragma_once_already_included(struct preprocessor_ctx * ctx, char * path);
struct map_entry *hashmap_find(struct hash_map * map, char * key);
char *read_file(char * path, unsigned char append_newline);
char *realpath(char * path, char * resolved_path);

char *find_and_read_include_file(struct preprocessor_ctx * ctx, char * path, char * current_file_dir, unsigned char is_angle_bracket_form, unsigned char * p_already_included, char full_path_out[], int full_path_out_size, unsigned char include_next)
{
    char newpath[200];
    char * content;
    struct include_dir * current;

    _cake_zmem(&newpath, 200);
    full_path_out[0] = 0;
    if (path_is_absolute(path))
    {
        char * content;

        snprintf(newpath, 200U, "%s", path);
        path_normalize(newpath);
        if (pragma_once_already_included(ctx, newpath))
        {
            *p_already_included = 1;
            return 0U;
        }
        content = read_file(newpath, 1);
        if (content != 0U)
        {
            snprintf(full_path_out, full_path_out_size, "%s", path);
            return content;
        }
        return 0U;
    }
    content = 0U;
    if (!is_angle_bracket_form)
    {
        snprintf(newpath, 200U, "%s/%s", current_file_dir, path);
        if (!realpath(newpath, full_path_out))
        {
            full_path_out[0] = 0;
        }
        path_normalize(full_path_out);
        if (pragma_once_already_included(ctx, full_path_out))
        {
            *p_already_included = 1;
            return 0U;
        }
        if (full_path_out[0] != 0)
        {
            content = read_file(full_path_out, 1);
        }
        if (content != 0U)
        {
            return content;
        }
    }
    current = ctx->include_dir.head;
    while (current)
    {
        int len;

        len = strlen(current->path);
        if (current->path[len - 1] == 47)
        {
            snprintf(full_path_out, full_path_out_size, "%s%s", current->path, path);
        }
        else
        {
            snprintf(full_path_out, full_path_out_size, "%s/%s", current->path, path);
        }
        path_normalize(full_path_out);
        if (pragma_once_already_included(ctx, full_path_out))
        {
            *p_already_included = 1;
            return 0U;
        }
        content = read_file(full_path_out, 1);
        if (content != 0U)
        {
            if (include_next)
            {
                free(content);
                content = 0U;
                include_next = 0;
            }
            else
            {
                return content;
            }
        }
        current = current->next;
    }
    full_path_out[0] = 0;
    return 0U;
}



static unsigned char pragma_once_already_included(struct preprocessor_ctx * ctx, char * path)
{
    return hashmap_find(&ctx->pragma_once_map, path) != 0U;
}
int hashmap_set(struct hash_map * map, char * key, struct hash_item_set * item);
void hash_item_set_destroy(struct hash_item_set * p);

void add_macro(struct preprocessor_ctx * ctx, char * name)
{
    if (1) /*try*/
    {
        char * name_local;
        struct macro * macro;
        struct hash_item_set  item;

        name_local = strdup(name);
        if (name_local == 0U)
        {
            goto _CKL0;/*throw*/
        }
        macro = calloc(1, 32U);
        if (macro == 0U)
        {
            free(name_local);
            goto _CKL0;/*throw*/
        }
        macro->name = name_local;
        item.number = 0;
        item.p_enum_specifier = 0;
        item.p_enumerator = 0;
        item.p_struct_or_union_specifier = 0;
        item.p_declarator = 0;
        item.p_init_declarator = 0;
        item.p_macro = macro;
        item.p_struct_entry = 0;
        hashmap_set(&ctx->macros, name, &item);
        hash_item_set_destroy(&item);
    }
    else _CKL0: /*catch*/ 
    {
    }
}


unsigned char token_is_blank(struct token * p);
struct token *token_list_clone_and_add(struct token_list * list, struct token * pnew);
void remove_line_continuation(char * s);

struct token_list copy_argument_list_tokens(struct token_list * list)
{
    struct token_list  r;
    struct token * current;
    unsigned char is_first;

    _cake_zmem(&r, 8);
    current = list->head;
    while (current && (token_is_blank(current) || current->type == 10))
    {
        current = current->next;
    }
    is_first = 1;
    for (; current; )
    {
        struct token * token;

        if (current && (token_is_blank(current) || current->type == 10))
        {
            if (current == list->tail)
            {
                break;
            }
            current = current->next;
            continue;
        }
        token = token_list_clone_and_add(&r, current);
        if (token->flags & 8)
        {
            token->flags = token->flags & -9;
            token->flags |= 4;
        }
        if (is_first)
        {
            token->flags = token->flags & -5;
            token->flags = token->flags & -9;
            is_first = 0;
        }
        remove_line_continuation(token->lexeme);
        if (current == list->tail)
        {
            break;
        }
        current = current->next;
    }
    return r;
}


void macro_argument_delete(struct macro_argument * p)
{
    if (p)
    {
        ;
        token_list_destroy(&p->tokens);
        free(p);
    }
}


void token_delete(struct token * p);
struct token *token_list_add(struct token_list * list, struct token * pnew);

struct token_list copy_argument_list(struct macro_argument * p_macro_argument)
{
    struct token_list  empty;

    if (1) /*try*/
    {
        struct token_list  list;

        list = copy_argument_list_tokens(&p_macro_argument->tokens);
        if (list.head == 0U)
        {
            struct token * p_new_token;
            char * temp;

            p_new_token = calloc(1, 40U);
            if (p_new_token == 0U)
            {
                goto _CKL0;/*throw*/
            }
            temp = strdup("");
            if (temp == 0U)
            {
                token_delete(p_new_token);
                goto _CKL0;/*throw*/
            }
            p_new_token->lexeme = temp;
            p_new_token->type = 142;
            token_list_add(&list, p_new_token);
        }
        return list;
    }
    else _CKL0: /*catch*/ 
    {
    }
    _cake_zmem(&empty, 8);
    return empty;
}


void macro_argument_list_destroy(struct macro_argument_list * list)
{
    struct macro_argument * p;

    token_list_destroy(&list->tokens);
    p = list->head;
    while (p)
    {
        struct macro_argument * next;

        next = p->next;
        p->next = 0U;
        macro_argument_delete(p);
        p = next;
    }
}


void print_list(struct token_list * list);

void print_macro_arguments(struct macro_argument_list * arguments)
{
    struct macro_argument * p_argument;

    p_argument = arguments->head;
    while (p_argument)
    {
        if (p_argument->macro_parameter)
        {
            printf("%s:", p_argument->macro_parameter->name);
        }
        print_list(&p_argument->tokens);
        p_argument = p_argument->next;
    }
}


int __cdecl strcmp(char * _Str1, char * _Str2);

struct macro_argument *find_macro_argument_by_name(struct macro_argument_list * parameters, char * name)
{
    struct macro_argument * p_macro_argument;

    p_macro_argument = parameters->head;
    while (p_macro_argument)
    {
        if (strcmp(p_macro_argument->macro_parameter->name, name) == 0)
        {
            return p_macro_argument;
        }
        p_macro_argument = p_macro_argument->next;
    }
    return 0U;
}


void argument_list_add(struct macro_argument_list * list, struct macro_argument * pnew)
{
    ;
    if (list->head == 0U)
    {
        list->head = pnew;
        ;
        list->tail = pnew;
    }
    else
    {
        ;
        ;
        list->tail->next = pnew;
        list->tail = pnew;
    }
}


void print_macro(struct macro * macro)
{
    struct macro_parameter * parameter;

    printf("%s", macro->name);
    if (macro->is_function)
    {
        printf("(");
    }
    parameter = macro->parameters;
    while (parameter)
    {
        if (macro->parameters != parameter)
        {
            printf(",");
        }
        printf("%s", parameter->name);
        parameter = parameter->next;
    }
    if (macro->is_function)
    {
        printf(") ");
    }
    print_list(&macro->replacement_list);
}


void macro_parameters_delete(struct macro_parameter * parameters)
{
    struct macro_parameter * p;

    p = parameters;
    while (p)
    {
        struct macro_parameter * p_next;

        p_next = p->next;
        free((void *)p->name);
        free(p);
        p = p_next;
    }
}


unsigned char token_list_is_equal(struct token_list * list_a, struct token_list * list_b);

unsigned char macro_is_same(struct macro * macro_a, struct macro * macro_b)
{
    struct macro_parameter * p_a;
    struct macro_parameter * p_b;

    if (macro_a->is_function != macro_b->is_function)
    {
        return 0;
    }
    if (strcmp(macro_a->name, macro_b->name) != 0)
    {
        return 0;
    }
    if (!token_list_is_equal(&macro_a->replacement_list, &macro_b->replacement_list) != 0)
    {
        return 0;
    }
    p_a = macro_a->parameters;
    p_b = macro_b->parameters;
    while (p_a && p_b)
    {
        if (strcmp(p_a->name, p_b->name) != 0)
        {
            return 0;
        }
        p_a = p_a->next;
        p_b = p_b->next;
    }
    return p_a == 0U && p_b == 0U;
}


void macro_delete(struct macro * macro)
{
    if (macro)
    {
        struct macro_parameter * p_macro_parameter;

        token_list_destroy(&macro->replacement_list);
        p_macro_parameter = macro->parameters;
        while (p_macro_parameter)
        {
            struct macro_parameter * p_next;

            p_next = p_macro_parameter->next;
            free((void *)p_macro_parameter->name);
            token_list_destroy(&p_macro_parameter->expanded_list);
            free(p_macro_parameter);
            p_macro_parameter = p_next;
        }
        free((void *)macro->name);
        free(macro);
    }
}


struct macro *find_macro(struct preprocessor_ctx * ctx, char * name)
{
    struct map_entry * p_entry;

    p_entry = hashmap_find(&ctx->macros, name);
    if (p_entry == 0U)
    {
        return 0U;
    }
    return p_entry->data.p_macro;
}


void stream_print_line(struct stream * stream)
{
    char * p;

    p = stream->current;
    while ((p - 1) >= stream->source && *(p - 1) != 10)
    {
        p--;
    }
    while (*p && *(p + 1) != 10)
    {
        printf("%c", *p);
        p++;
    }
    printf("\n");
    {
        int i;

        i = 0;
        for (; i < stream->col - 1; i++)
        printf(" ");
    }
    printf("^\n");
}


void stream_match(struct stream * stream)
{
    if (stream->current[0] == 10)
    {
        stream->line++;
        stream->col = 1;
    }
    else
    {
        stream->col++;
    }
    if (stream->current[0] == 0)
    {
        return;
    }
    stream->current++;
    while (stream->current[0] == 92 && (stream->current[1] == 10 || (stream->current[1] == 13 && stream->current[2] == 10)))
    {
        if (stream->current[1] == 13 && stream->current[2] == 10)
        {
            stream->current++;
            stream->current++;
            stream->current++;
        }
        else
        {
            stream->current++;
            stream->current++;
        }
        stream->line++;
        stream->col = 1;
        stream->line_continuation_count++;
    }
}


void print_line(struct token * p)
{
    struct token * prev;
    struct token * next;

    printf("%s\n", p->token_origin->lexeme);
    prev = p;
    while (prev->prev && prev->prev->type != 10)
    {
        prev = prev->prev;
    }
    next = prev;
    while (next && next->type != 10)
    {
        printf("%s", next->lexeme);
        next = next->next;
    }
    printf("\n");
}


int is_nondigit(struct stream * p)
{
    return (p->current[0] >= 97 && p->current[0] <= 122) || (p->current[0] >= 65 && p->current[0] <= 90) || (p->current[0] == 95);
}


int is_punctuator(struct stream * stream)
{
    int type;

    type = 0;
    /*switch*/
    {
        register char __Ck1_temp = stream->current[0];
        if (__Ck1_temp == 91) goto _CKL1; /*case 91*/
        if (__Ck1_temp == 93) goto _CKL2; /*case 93*/
        if (__Ck1_temp == 40) goto _CKL3; /*case 40*/
        if (__Ck1_temp == 41) goto _CKL4; /*case 41*/
        if (__Ck1_temp == 123) goto _CKL5; /*case 123*/
        if (__Ck1_temp == 125) goto _CKL6; /*case 125*/
        if (__Ck1_temp == 59) goto _CKL7; /*case 59*/
        if (__Ck1_temp == 44) goto _CKL8; /*case 44*/
        if (__Ck1_temp == 33) goto _CKL9; /*case 33*/
        if (__Ck1_temp == 58) goto _CKL11; /*case 58*/
        if (__Ck1_temp == 126) goto _CKL13; /*case 126*/
        if (__Ck1_temp == 63) goto _CKL14; /*case 63*/
        if (__Ck1_temp == 47) goto _CKL15; /*case 47*/
        if (__Ck1_temp == 42) goto _CKL17; /*case 42*/
        if (__Ck1_temp == 37) goto _CKL19; /*case 37*/
        if (__Ck1_temp == 45) goto _CKL21; /*case 45*/
        if (__Ck1_temp == 124) goto _CKL25; /*case 124*/
        if (__Ck1_temp == 43) goto _CKL28; /*case 43*/
        if (__Ck1_temp == 61) goto _CKL31; /*case 61*/
        if (__Ck1_temp == 94) goto _CKL33; /*case 94*/
        if (__Ck1_temp == 38) goto _CKL35; /*case 38*/
        if (__Ck1_temp == 62) goto _CKL38; /*case 62*/
        if (__Ck1_temp == 60) goto _CKL42; /*case 60*/
        if (__Ck1_temp == 35) goto _CKL46; /*case 35*/
        if (__Ck1_temp == 46) goto _CKL48; /*case 46*/
        goto _CKL0;

        {
            _CKL1: /*case 91*/ 
            type = 91;
            stream_match(stream);
            goto _CKL0; /*break*/

            _CKL2: /*case 93*/ 
            type = 93;
            stream_match(stream);
            goto _CKL0; /*break*/

            _CKL3: /*case 40*/ 
            type = 40;
            stream_match(stream);
            goto _CKL0; /*break*/

            _CKL4: /*case 41*/ 
            type = 41;
            stream_match(stream);
            goto _CKL0; /*break*/

            _CKL5: /*case 123*/ 
            type = 123;
            stream_match(stream);
            goto _CKL0; /*break*/

            _CKL6: /*case 125*/ 
            type = 125;
            stream_match(stream);
            goto _CKL0; /*break*/

            _CKL7: /*case 59*/ 
            type = 59;
            stream_match(stream);
            goto _CKL0; /*break*/

            _CKL8: /*case 44*/ 
            type = 44;
            stream_match(stream);
            goto _CKL0; /*break*/

            _CKL9: /*case 33*/ 
            type = 33;
            stream_match(stream);
            if (stream->current[0] == 61)
            {
                type = 8509;
                stream_match(stream);
            }
            goto _CKL0; /*break*/

            _CKL11: /*case 58*/ 
            type = 58;
            stream_match(stream);
            if (stream->current[0] == 58)
            {
                type = 14906;
                stream_match(stream);
            }
            goto _CKL0; /*break*/

            _CKL13: /*case 126*/ 
            type = 126;
            stream_match(stream);
            goto _CKL0; /*break*/

            _CKL14: /*case 63*/ 
            type = 63;
            stream_match(stream);
            goto _CKL0; /*break*/

            _CKL15: /*case 47*/ 
            type = 47;
            stream_match(stream);
            if (stream->current[0] == 61)
            {
                type = 12093;
                stream_match(stream);
            }
            goto _CKL0; /*break*/

            _CKL17: /*case 42*/ 
            type = 42;
            stream_match(stream);
            if (stream->current[0] == 61)
            {
                type = 10813;
                stream_match(stream);
            }
            goto _CKL0; /*break*/

            _CKL19: /*case 37*/ 
            type = 37;
            stream_match(stream);
            if (stream->current[0] == 61)
            {
                type = 9533;
                stream_match(stream);
            }
            goto _CKL0; /*break*/

            _CKL21: /*case 45*/ 
            type = 45;
            stream_match(stream);
            if (stream->current[0] == 62)
            {
                type = 11582;
                stream_match(stream);
            }
            else
            {
                if (stream->current[0] == 45)
                {
                    type = 11565;
                    stream_match(stream);
                }
                else
                {
                    if (stream->current[0] == 61)
                    {
                        type = 11581;
                        stream_match(stream);
                    }
                }
            }
            goto _CKL0; /*break*/

            _CKL25: /*case 124*/ 
            type = 124;
            stream_match(stream);
            if (stream->current[0] == 124)
            {
                type = 31868;
                stream_match(stream);
            }
            else
            {
                if (stream->current[0] == 61)
                {
                    type = 31805;
                    stream_match(stream);
                }
            }
            goto _CKL0; /*break*/

            _CKL28: /*case 43*/ 
            type = 43;
            stream_match(stream);
            if (stream->current[0] == 43)
            {
                type = 11051;
                stream_match(stream);
            }
            else
            {
                if (stream->current[0] == 61)
                {
                    type = 11069;
                    stream_match(stream);
                }
            }
            goto _CKL0; /*break*/

            _CKL31: /*case 61*/ 
            type = 61;
            stream_match(stream);
            if (stream->current[0] == 61)
            {
                type = 15677;
                stream_match(stream);
            }
            goto _CKL0; /*break*/

            _CKL33: /*case 94*/ 
            type = 94;
            stream_match(stream);
            if (stream->current[0] == 61)
            {
                type = 24125;
                stream_match(stream);
            }
            goto _CKL0; /*break*/

            _CKL35: /*case 38*/ 
            type = 38;
            stream_match(stream);
            if (stream->current[0] == 38)
            {
                type = 9766;
                stream_match(stream);
            }
            else
            {
                if (stream->current[0] == 61)
                {
                    type = 9789;
                    stream_match(stream);
                }
            }
            goto _CKL0; /*break*/

            _CKL38: /*case 62*/ 
            type = 62;
            stream_match(stream);
            if (stream->current[0] == 62)
            {
                type = 15934;
                stream_match(stream);
                if (stream->current[0] == 61)
                {
                    type = 4079165;
                    stream_match(stream);
                }
            }
            else
            {
                if (stream->current[0] == 61)
                {
                    type = 15933;
                    stream_match(stream);
                }
            }
            goto _CKL0; /*break*/

            _CKL42: /*case 60*/ 
            type = 60;
            stream_match(stream);
            if (stream->current[0] == 60)
            {
                type = 15420;
                stream_match(stream);
                if (stream->current[0] == 61)
                {
                    type = 3947581;
                    stream_match(stream);
                }
            }
            else
            {
                if (stream->current[0] == 61)
                {
                    type = 15421;
                    stream_match(stream);
                }
            }
            goto _CKL0; /*break*/

            _CKL46: /*case 35*/ 
            type = 35;
            stream_match(stream);
            if (stream->current[0] == 35)
            {
                type = 8995;
                stream_match(stream);
            }
            goto _CKL0; /*break*/

            _CKL48: /*case 46*/ 
            type = 46;
            stream_match(stream);
            if (stream->current[0] == 46 && stream->current[1] == 46)
            {
                type = 3026478;
                stream_match(stream);
                stream_match(stream);
            }
            goto _CKL0; /*break*/

        }
        _CKL0:;
    }
    return type;
}


char *__cdecl strncpy(char * _Destination, char * _Source, unsigned int _Count);

struct token *new_token(char * lexeme_head, char * lexeme_tail, int type)
{
    struct token * p_new_token;

    p_new_token = 0U;
    if (1) /*try*/
    {
        unsigned int sz;
        char * temp;

        p_new_token = calloc(1, 40U);
        if (p_new_token == 0U)
        {
            goto _CKL0;/*throw*/
        }
        sz = lexeme_tail - lexeme_head;
        temp = calloc(sz + 1, 1U);
        if (temp == 0U)
        {
            goto _CKL0;/*throw*/
        }
        p_new_token->lexeme = temp;
        p_new_token->type = type;
        strncpy(p_new_token->lexeme, lexeme_head, sz);
    }
    else _CKL0: /*catch*/ 
    {
        token_delete(p_new_token);
        p_new_token = 0U;
    }
    return p_new_token;
}


int is_digit(struct stream * p);

struct token *identifier(struct stream * stream)
{
    char * start;
    struct token * p_new_token;

    start = stream->current;
    stream_match(stream);
    while (is_nondigit(stream) || is_digit(stream))
    {
        stream_match(stream);
    }
    p_new_token = new_token(start, stream->current, 8996);
    return p_new_token;
}


static void tokenizer_set_warning(struct tokenizer_ctx * ctx, struct stream * stream, char * fmt, ...);

struct token *character_constant(struct tokenizer_ctx * ctx, struct stream * stream)
{
    char * start;
    struct token * p_new_token;

    start = stream->current;
    if (stream->current[0] == 117)
    {
        stream_match(stream);
        if (stream->current[0] == 56)
        {
            stream_match(stream);
        }
    }
    else
    {
        if (stream->current[0] == 85 || stream->current[0] == 76)
        {
            stream_match(stream);
        }
    }
    stream_match(stream);
    while (stream->current[0] != 39)
    {
        if (stream->current[0] == 92)
        {
            stream_match(stream);
            stream_match(stream);
        }
        else
        {
            stream_match(stream);
        }
        if (stream->current[0] == 0 || stream->current[0] == 10)
        {
            tokenizer_set_warning(ctx, stream, "missing terminating ' character");
            break;
        }
    }
    stream_match(stream);
    p_new_token = new_token(start, stream->current, 131);
    return p_new_token;
}



static void tokenizer_set_warning(struct tokenizer_ctx * ctx, struct stream * stream, char * fmt, ...)
{
    char buffer[200];
    char * args;

    ctx->n_warnings++;
    _cake_zmem(&buffer, 200);
    _cake_zmem(&args, 4);
    ((void)(args = (char *)(&(fmt)) + 4U));
    vsnprintf(buffer, 200U, fmt, args);
    ((void)(args = 0U));
    print_position(stream->path, stream->line, stream->col, ctx->options.visual_studio_ouput_format);
    if (ctx->options.visual_studio_ouput_format)
    {
        printf("warning: %s\n", buffer);
    }
    else
    {
        printf("\x1b[95mwarning: \x1b[97m%s\n", buffer);
    }
}
static void tokenizer_set_error(struct tokenizer_ctx * ctx, struct stream * stream, char * fmt, ...);

struct token *string_literal(struct tokenizer_ctx * ctx, struct stream * stream)
{
    struct token * p_new_token;
    char * start;

    p_new_token = 0U;
    start = stream->current;
    if (1) /*try*/
    {
        if (stream->current[0] == 117)
        {
            stream_match(stream);
            if (stream->current[0] == 56)
            {
                stream_match(stream);
            }
        }
        else
        {
            if (stream->current[0] == 85 || stream->current[0] == 76)
            {
                stream_match(stream);
            }
        }
        stream_match(stream);
        while (stream->current[0] != 34)
        {
            if (stream->current[0] == 0 || stream->current[0] == 10)
            {
                tokenizer_set_error(ctx, stream, "missing terminating \" character");
                goto _CKL0;/*throw*/
            }
            if (stream->current[0] == 92)
            {
                stream_match(stream);
                stream_match(stream);
            }
            else
            {
                stream_match(stream);
            }
        }
        stream_match(stream);
        p_new_token = new_token(start, stream->current, 130);
    }
    else _CKL0: /*catch*/ 
    {
    }
    return p_new_token;
}



static void tokenizer_set_error(struct tokenizer_ctx * ctx, struct stream * stream, char * fmt, ...)
{
    char buffer[200];
    char * args;

    ctx->n_errors++;
    _cake_zmem(&buffer, 200);
    _cake_zmem(&args, 4);
    ((void)(args = (char *)(&(fmt)) + 4U));
    vsnprintf(buffer, 200U, fmt, args);
    ((void)(args = 0U));
    print_position(stream->path, stream->line, stream->col, ctx->options.visual_studio_ouput_format);
    if (ctx->options.visual_studio_ouput_format)
    {
        printf("error: %s\n", buffer);
    }
    else
    {
        printf("\x1b[91merror: \x1b[97m%s\n", buffer);
    }
}
int get_char_type(char * s)
{
    if (s[0] == 76)
    {
        return 2;
    }
    return 1;
}


int string_literal_char_byte_size(char * s)
{
    if (s[0] == 117)
    {
    }
    else
    {
        if (s[0] == 85 || s[0] == 76)
        {
            return 2;
        }
    }
    return 1;
}


int string_literal_byte_size_not_zero_included(char * s)
{
    struct stream  stream;
    int size;
    int charsize;

    stream.source = s;
    stream.current = 0;
    stream.line = 0;
    stream.col = 0;
    stream.line_continuation_count = 0;
    stream.path = 0;
    stream.current = s;
    stream.line = 1;
    stream.col = 1;
    stream.path = "";
    size = 0;
    charsize = string_literal_char_byte_size(s);
    if (1) /*try*/
    {
        if (stream.current[0] == 117)
        {
            stream_match(&stream);
            if (stream.current[0] == 56)
            {
                stream_match(&stream);
            }
        }
        else
        {
            if (stream.current[0] == 85 || stream.current[0] == 76)
            {
                stream_match(&stream);
            }
        }
        stream_match(&stream);
        while (stream.current[0] != 34)
        {
            if (stream.current[0] == 0 || stream.current[0] == 10)
            {
                goto _CKL0;/*throw*/
            }
            if (stream.current[0] == 92)
            {
                stream_match(&stream);
                stream_match(&stream);
                size++;
            }
            else
            {
                stream_match(&stream);
                size++;
            }
        }
        stream_match(&stream);
    }
    else _CKL0: /*catch*/ 
    {
    }
    return size * charsize;
}


struct _iobuf *fopen(char * _FileName, char * _Mode);
unsigned int fread(void * _Buffer, unsigned int _ElementSize, unsigned int _ElementCount, struct _iobuf * _Stream);
int fclose(struct _iobuf * _Stream);

struct token_list embed_tokenizer(struct preprocessor_ctx * ctx, struct token * position, char * filename_opt, int level, int addflags)
{
    struct token_list  list;
    struct _iobuf * file;
    unsigned char b_first;
    int line;
    int col;
    int count;

    _cake_zmem(&list, 8);
    file = 0U;
    b_first = 1;
    line = 1;
    col = 1;
    count = 0;
    if (1) /*try*/
    {
        unsigned char ch;
        char newline[2];
        struct token * p_new_token;

        file = (struct _iobuf *)fopen(filename_opt, "rb");
        if (file == 0U)
        {
            preprocessor_diagnostic(1150, ctx, position, "file '%s' not found", filename_opt);
            goto _CKL0;/*throw*/
        }
        ch = 0;
        while (fread(&ch, 1, 1, file))
        {
            char buffer[30];
            int c;
            struct token * p_new_token;

            if (b_first)
            {
                b_first = 0;
            }
            else
            {
                char b[2];
                struct token * p_new_token;

                _cake_memcpy(b, ",", 2);
                p_new_token = new_token(b, &b[1], 44);
                if (p_new_token == 0U)
                {
                    goto _CKL0;/*throw*/
                }
                p_new_token->flags |= addflags;
                p_new_token->level = level;
                p_new_token->token_origin = 0U;
                p_new_token->line = line;
                p_new_token->col = col;
                token_list_add(&list, p_new_token);
                if (count > 0 && count % 25 == 0)
                {
                    char newline[2];
                    struct token * p_new3;

                    _cake_memcpy(newline, "\n", 2);
                    p_new3 = new_token(newline, &newline[1], 10);
                    if (p_new3 == 0U)
                    {
                        goto _CKL0;/*throw*/
                    }
                    p_new3->level = level;
                    p_new3->token_origin = 0U;
                    p_new3->line = line;
                    p_new3->col = col;
                    token_list_add(&list, p_new3);
                }
            }
            _cake_zmem(&buffer, 30);
            c = snprintf(buffer, 30U, "%d", (int)ch);
            p_new_token = new_token(buffer, &buffer[c], 134);
            if (p_new_token == 0U)
            {
                goto _CKL0;/*throw*/
            }
            p_new_token->flags |= addflags;
            p_new_token->level = level;
            p_new_token->token_origin = 0U;
            p_new_token->line = line;
            p_new_token->col = col;
            token_list_add(&list, p_new_token);
            count++;
        }
        _cake_memcpy(newline, "\n", 2);
        p_new_token = new_token(newline, &newline[1], 10);
        if (p_new_token == 0U)
        {
            goto _CKL0;/*throw*/
        }
        p_new_token->level = level;
        p_new_token->token_origin = 0U;
        p_new_token->line = line;
        p_new_token->col = col;
        token_list_add(&list, p_new_token);
        ;
    }
    else _CKL0: /*catch*/ 
    {
    }
    if (file)
    {
        fclose(file);
    }
    return list;
}


int __cdecl isdigit(int _C);
static struct token *ppnumber(struct stream * stream);
static unsigned char set_sliced_flag(struct stream * stream, struct token * p_new_token);
static unsigned char first_of_string_literal(struct stream * stream);
static unsigned char first_of_character_constant(struct stream * stream);

struct token_list tokenizer(struct tokenizer_ctx * ctx, char * text, char * filename_opt, int level, int addflags)
{
    struct token_list  list;
    struct stream  stream;

    _cake_zmem(&list, 8);
    stream.source = text;
    stream.current = text;
    stream.line = 1;
    stream.col = 1;
    stream.line_continuation_count = 0;
    stream.path = filename_opt ? filename_opt : "";
    if (1) /*try*/
    {
        struct token * p_first;
        unsigned char new_line;
        unsigned char has_space;

        p_first = 0U;
        if (filename_opt != 0U)
        {
            char * begin;
            char * end;
            struct token * p_new;

            begin = filename_opt;
            end = filename_opt + strlen(filename_opt);
            p_new = new_token(begin, end, 8998);
            if (p_new == 0U)
            {
                goto _CKL0;/*throw*/
            }
            path_normalize(p_new->lexeme);
            p_new->level = level;
            p_first = token_list_add(&list, p_new);
        }
        new_line = 1;
        has_space = 0;
        while (1)
        {
            int line;
            int col;
            char * start;
            int t;

            line = stream.line;
            col = stream.col;
            stream.line_continuation_count = 0;
            if (stream.current[0] == 0)
            {
                stream_match(&stream);
                break;
            }
            if (is_digit(&stream) || (stream.current[0] == 46 && isdigit(stream.current[1])))
            {
                struct token * p_new_token;

                p_new_token = ppnumber(&stream);
                if (p_new_token == 0U)
                {
                    goto _CKL0;/*throw*/
                }
                p_new_token->flags |= has_space ? 4 : 0;
                p_new_token->flags |= new_line ? 8 : 0;
                p_new_token->flags |= addflags;
                p_new_token->flags |= addflags;
                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                set_sliced_flag(&stream, p_new_token);
                token_list_add(&list, p_new_token);
                new_line = 0;
                has_space = 0;
                continue;
            }
            if (first_of_string_literal(&stream))
            {
                struct token * p_new_token;

                p_new_token = string_literal(ctx, &stream);
                if (p_new_token == 0U)
                {
                    goto _CKL0;/*throw*/
                }
                p_new_token->flags |= has_space ? 4 : 0;
                p_new_token->flags |= new_line ? 8 : 0;
                p_new_token->flags |= addflags;
                p_new_token->flags |= addflags;
                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                set_sliced_flag(&stream, p_new_token);
                token_list_add(&list, p_new_token);
                new_line = 0;
                has_space = 0;
                continue;
            }
            if (first_of_character_constant(&stream))
            {
                struct token * p_new_token;

                p_new_token = character_constant(ctx, &stream);
                if (p_new_token == 0U)
                {
                    goto _CKL0;/*throw*/
                }
                p_new_token->flags |= has_space ? 4 : 0;
                p_new_token->flags |= new_line ? 8 : 0;
                p_new_token->flags |= addflags;
                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                set_sliced_flag(&stream, p_new_token);
                token_list_add(&list, p_new_token);
                new_line = 0;
                has_space = 0;
                continue;
            }
            if (is_nondigit(&stream))
            {
                struct token * p_new_token;

                p_new_token = identifier(&stream);
                if (p_new_token == 0U)
                {
                    goto _CKL0;/*throw*/
                }
                p_new_token->flags |= has_space ? 4 : 0;
                p_new_token->flags |= new_line ? 8 : 0;
                p_new_token->flags |= addflags;
                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                new_line = 0;
                has_space = 0;
                if (set_sliced_flag(&stream, p_new_token))
                {
                    tokenizer_set_warning(ctx, &stream, "token sliced");
                }
                token_list_add(&list, p_new_token);
                continue;
            }
            if (stream.current[0] == 32 || stream.current[0] == 9 || stream.current[0] == 12)
            {
                char * start;
                struct token * p_new_token;

                start = stream.current;
                while (stream.current[0] == 32 || stream.current[0] == 9 || stream.current[0] == 12)
                {
                    stream_match(&stream);
                }
                p_new_token = new_token(start, stream.current, 143);
                if (p_new_token == 0U)
                {
                    goto _CKL0;/*throw*/
                }
                p_new_token->flags |= has_space ? 4 : 0;
                p_new_token->flags |= new_line ? 8 : 0;
                p_new_token->flags |= addflags;
                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                set_sliced_flag(&stream, p_new_token);
                token_list_add(&list, p_new_token);
                has_space = 1;
                continue;
            }
            if (stream.current[0] == 47 && stream.current[1] == 47)
            {
                char * start;
                struct token * p_new_token;

                start = stream.current;
                stream_match(&stream);
                stream_match(&stream);
                while (stream.current[0] != 10)
                {
                    stream_match(&stream);
                    if (stream.current[0] == 0)
                    {
                        break;
                    }
                }
                p_new_token = new_token(start, stream.current, 132);
                if (p_new_token == 0U)
                {
                    goto _CKL0;/*throw*/
                }
                p_new_token->flags |= has_space ? 4 : 0;
                p_new_token->flags |= new_line ? 8 : 0;
                p_new_token->flags |= addflags;
                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                set_sliced_flag(&stream, p_new_token);
                token_list_add(&list, p_new_token);
                new_line = 1;
                has_space = 0;
                if (stream.current[0] == 0)
                {
                    break;
                }
                continue;
            }
            if (stream.current[0] == 47 && stream.current[1] == 42)
            {
                char * start;
                struct token * p_new_token;

                start = stream.current;
                stream_match(&stream);
                stream_match(&stream);
                for (; ; )
                {
                    if (stream.current[0] == 42 && stream.current[1] == 47)
                    {
                        stream_match(&stream);
                        stream_match(&stream);
                        break;
                    }
                    else
                    {
                        if (stream.current[0] == 0)
                        {
                            tokenizer_set_error(ctx, &stream, "missing end of comment");
                            break;
                        }
                        else
                        {
                            stream_match(&stream);
                        }
                    }
                }
                p_new_token = new_token(start, stream.current, 133);
                if (p_new_token == 0U)
                {
                    goto _CKL0;/*throw*/
                }
                p_new_token->flags |= has_space ? 4 : 0;
                p_new_token->flags |= new_line ? 8 : 0;
                p_new_token->flags |= addflags;
                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                token_list_add(&list, p_new_token);
                new_line = 0;
                has_space = 0;
                continue;
            }
            if (new_line && stream.current[0] == 35)
            {
                char * start;
                struct token * p_new_token;

                start = stream.current;
                stream_match(&stream);
                p_new_token = new_token(start, stream.current, 35);
                if (p_new_token == 0U)
                {
                    goto _CKL0;/*throw*/
                }
                p_new_token->flags |= has_space ? 4 : 0;
                p_new_token->flags |= new_line ? 8 : 0;
                p_new_token->flags |= addflags;
                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                p_new_token->type = 127;
                set_sliced_flag(&stream, p_new_token);
                token_list_add(&list, p_new_token);
                new_line = 0;
                has_space = 0;
                continue;
            }
            if (stream.current[0] == 10 || stream.current[0] == 13)
            {
                char newline[2];
                struct token * p_new_token;

                if (stream.current[0] == 13 && stream.current[1] == 10)
                {
                    stream_match(&stream);
                    stream_match(&stream);
                }
                else
                {
                    stream_match(&stream);
                }
                _cake_memcpy(newline, "\n", 2);
                p_new_token = new_token(newline, newline + 1, 10);
                if (p_new_token == 0U)
                {
                    goto _CKL0;/*throw*/
                }
                p_new_token->flags |= has_space ? 4 : 0;
                p_new_token->flags |= new_line ? 8 : 0;
                p_new_token->flags |= addflags;
                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                set_sliced_flag(&stream, p_new_token);
                token_list_add(&list, p_new_token);
                new_line = 1;
                has_space = 0;
                continue;
            }
            start = stream.current;
            t = is_punctuator(&stream);
            if (t != 0)
            {
                struct token * p_new_token;

                p_new_token = new_token(start, stream.current, t);
                if (p_new_token == 0U)
                {
                    goto _CKL0;/*throw*/
                }
                p_new_token->flags |= has_space ? 4 : 0;
                p_new_token->flags |= new_line ? 8 : 0;
                p_new_token->flags |= addflags;
                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                set_sliced_flag(&stream, p_new_token);
                token_list_add(&list, p_new_token);
                new_line = 0;
                has_space = 0;
                continue;
            }
            else
            {
                struct token * p_new_token;

                stream_match(&stream);
                p_new_token = new_token(start, stream.current, 135);
                if (p_new_token == 0U)
                {
                    goto _CKL0;/*throw*/
                }
                p_new_token->flags |= has_space ? 4 : 0;
                p_new_token->flags |= new_line ? 8 : 0;
                p_new_token->flags |= addflags;
                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                set_sliced_flag(&stream, p_new_token);
                token_list_add(&list, p_new_token);
                new_line = 0;
                has_space = 0;
                continue;
            }
            break;
        }
    }
    else _CKL0: /*catch*/ 
    {
    }
    ;
    return list;
}



static struct token *ppnumber(struct stream * stream)
{
    char * start;
    struct token * p_new_token;

    start = stream->current;
    if (is_digit(stream))
    {
        stream_match(stream);
    }
    else
    {
        if (stream->current[0] == 46)
        {
            stream_match(stream);
            stream_match(stream);
        }
        else
        {
            ;
        }
    }
    for (; ; )
    {
        if (stream->current[0] == 39)
        {
            stream_match(stream);
            if (is_digit(stream))
            {
                stream_match(stream);
            }
            else
            {
                if (is_nondigit(stream))
                {
                    stream_match(stream);
                }
                else
                {
                    ;
                    break;
                }
            }
        }
        else
        {
            if ((stream->current[0] == 101 || stream->current[0] == 69 || stream->current[0] == 112 || stream->current[0] == 80) && (stream->current[1] == 43 || stream->current[1] == 45))
            {
                stream_match(stream);
                stream_match(stream);
            }
            else
            {
                if (stream->current[0] == 46)
                {
                    stream_match(stream);
                }
                else
                {
                    if (is_digit(stream) || is_nondigit(stream))
                    {
                        stream_match(stream);
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }
    p_new_token = new_token(start, stream->current, 134);
    return p_new_token;
}

static unsigned char set_sliced_flag(struct stream * stream, struct token * p_new_token)
{
    if (stream->line_continuation_count > 0)
    {
        p_new_token->flags |= 1024;
        if (stream->line_continuation_count == 1)
        {
            int l;

            l = strlen(p_new_token->lexeme);
            if (p_new_token->lexeme[l - 1] == 10)
            {
            }
            else
            {
                p_new_token->flags |= 512;
            }
        }
        else
        {
            p_new_token->flags |= 512;
        }
    }
    return ((p_new_token->flags & 512) != 0);
}

static unsigned char first_of_string_literal(struct stream * stream)
{
    return stream->current[0] == 34 || (stream->current[0] == 117 && stream->current[1] == 56 && stream->current[2] == 34) || (stream->current[0] == 117 && stream->current[1] == 34) || (stream->current[0] == 85 && stream->current[1] == 34) || (stream->current[0] == 76 && stream->current[1] == 34);
}

static unsigned char first_of_character_constant(struct stream * stream)
{
    return stream->current[0] == 39 || (stream->current[0] == 117 && stream->current[1] == 56 && stream->current[2] == 39) || (stream->current[0] == 117 && stream->current[1] == 39) || (stream->current[0] == 85 && stream->current[1] == 39) || (stream->current[0] == 76 && stream->current[1] == 39);
}
int __cdecl feof(struct _iobuf * _Stream);

unsigned char fread2(void * buffer, unsigned int size, unsigned int count, struct _iobuf * stream, unsigned int * sz)
{
    unsigned char result;
    unsigned int n;

    *sz = 0;
    result = 0;
    n = fread(buffer, size, count, stream);
    if (n == count)
    {
        *sz = n;
        result = 1;
    }
    else
    {
        if (n < count)
        {
            if (feof(stream))
            {
                *sz = n;
                result = 1;
            }
        }
    }
    return result;
}


unsigned char token_list_is_empty(struct token_list * p);
unsigned char preprocessor_token_ahead_is_identifier(struct token * p, char * lexeme);
struct token_list group_part(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char is_active, int level);
void token_list_append_list(struct token_list * dest, struct token_list * source);

struct token_list group_opt(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char is_active, int level)
{
    struct token_list  r;

    _cake_zmem(&r, 8);
    if (1) /*try*/
    {
        if (token_list_is_empty(input_list))
        {
            return r;
        }
        while (!token_list_is_empty(input_list))
        {
            ;
            if (input_list->head->type == 127 && (preprocessor_token_ahead_is_identifier(input_list->head, "endif") || preprocessor_token_ahead_is_identifier(input_list->head, "else") || preprocessor_token_ahead_is_identifier(input_list->head, "elif") || preprocessor_token_ahead_is_identifier(input_list->head, "elifdef") || preprocessor_token_ahead_is_identifier(input_list->head, "elifndef")))
            {
                break;
            }
            else
            {
                struct token_list  r2;

                r2 = group_part(ctx, input_list, is_active, level);
                token_list_append_list(&r, &r2);
                token_list_destroy(&r2);
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
    return r;
}


unsigned char is_parser_token(struct token * p)
{
    return p->type != 133 && p->type != 143 && p->type != 132 && p->type != 10;
}


unsigned char is_never_final(int type)
{
    return type == 8998 || type == 143 || type == 132 || type == 133 || type == 142 || type == 10;
}


struct token *preprocessor_look_ahead_core(struct token * p)
{
    struct token * current;

    current = p->next;
    while (current && (current->type == 143 || current->type == 142 || current->type == 132 || current->type == 133))
    {
        current = current->next;
    }
    return current;
}


unsigned char preprocessor_token_ahead_is(struct token * p, int t)
{
    struct token * p_token;

    p_token = preprocessor_look_ahead_core(p);
    if (p_token != 0U && p_token->type == t)
    {
        return 1;
    }
    return 0;
}


unsigned char preprocessor_token_ahead_is_identifier(struct token * p, char * lexeme)
{
    struct token * p_token;

    ;
    p_token = preprocessor_look_ahead_core(p);
    if (p_token != 0U && p_token->type == 8996)
    {
        return strcmp(p_token->lexeme, lexeme) == 0;
    }
    return 0;
}


struct token *token_list_pop_front_get(struct token_list * list);
void token_list_pop_front(struct token_list * list);

void prematch_level(struct token_list * dest, struct token_list * input_list, int level)
{
    if (1)
    {
        struct token * p;

        p = token_list_pop_front_get(input_list);
        if (p)
        {
            token_list_add(dest, p);
        }
    }
    else
    {
        token_list_pop_front(input_list);
    }
}


void prematch(struct token_list * dest, struct token_list * input_list)
{
    struct token * p;

    p = token_list_pop_front_get(input_list);
    if (p)
    {
        token_list_add(dest, p);
    }
}


static void skip_blanks(struct preprocessor_ctx * ctx, struct token_list * dest, struct token_list * input_list);
char *__cdecl strcat(char * _Destination, char * _Source);

struct token_list process_defined(struct preprocessor_ctx * ctx, struct token_list * input_list)
{
    struct token_list  r;

    _cake_zmem(&r, 8);
    if (1) /*try*/
    {
        while (input_list->head != 0U)
        {
            if (input_list->head->type == 8996 && strcmp(input_list->head->lexeme, "defined") == 0)
            {
                unsigned char has_parentesis;
                struct macro * macro;
                struct token * p_new_token;
                char * temp;

                token_list_pop_front(input_list);
                skip_blanks(ctx, &r, input_list);
                if (input_list->head == 0U)
                {
                    pre_unexpected_end_of_file(r.tail, ctx);
                    goto _CKL0;/*throw*/
                }
                has_parentesis = 0;
                if (input_list->head->type == 40)
                {
                    token_list_pop_front(input_list);
                    has_parentesis = 1;
                }
                skip_blanks(ctx, &r, input_list);
                if (input_list->head == 0U)
                {
                    pre_unexpected_end_of_file(r.tail, ctx);
                    goto _CKL0;/*throw*/
                }
                macro = find_macro(ctx, input_list->head->lexeme);
                p_new_token = token_list_pop_front_get(input_list);
                if (p_new_token == 0U)
                {
                    goto _CKL0;/*throw*/
                }
                p_new_token->type = 134;
                temp = 0U;
                if (macro)
                {
                    temp = strdup("1");
                }
                else
                {
                    temp = strdup("0");
                }
                if (temp == 0U)
                {
                    token_delete(p_new_token);
                    goto _CKL0;/*throw*/
                }
                free(p_new_token->lexeme);
                p_new_token->lexeme = temp;
                token_list_add(&r, p_new_token);
                if (has_parentesis)
                {
                    if (input_list->head == 0U)
                    {
                        goto _CKL0;/*throw*/
                    }
                    if (input_list->head->type != 41)
                    {
                        preprocessor_diagnostic(1160, ctx, input_list->head, "missing )");
                        goto _CKL0;/*throw*/
                    }
                    token_list_pop_front(input_list);
                }
            }
            else
            {
                if (input_list->head->type == 8996 && (strcmp(input_list->head->lexeme, "__has_include") == 0 || strcmp(input_list->head->lexeme, "__has_embed") == 0))
                {
                    char path[100];
                    unsigned char is_angle_bracket_form;
                    char fullpath[300];
                    char full_path_result[200];
                    unsigned char already_included;
                    char * s;
                    unsigned char has_include;
                    struct token * p_new_token;
                    char * temp;

                    token_list_pop_front(input_list);
                    skip_blanks(ctx, &r, input_list);
                    token_list_pop_front(input_list);
                    skip_blanks(ctx, &r, input_list);
                    _cake_zmem(&path, 100);
                    is_angle_bracket_form = 0;
                    if (input_list->head == 0U)
                    {
                        pre_unexpected_end_of_file(r.tail, ctx);
                        goto _CKL0;/*throw*/
                    }
                    if (input_list->head->type == 130)
                    {
                        strcat(path, input_list->head->lexeme);
                        token_list_pop_front(input_list);
                    }
                    else
                    {
                        is_angle_bracket_form = 1;
                        token_list_pop_front(input_list);
                        if (input_list->head == 0U)
                        {
                            pre_unexpected_end_of_file(r.tail, ctx);
                            goto _CKL0;/*throw*/
                        }
                        while (input_list->head->type != 62)
                        {
                            strcat(path, input_list->head->lexeme);
                            token_list_pop_front(input_list);
                            if (input_list->head == 0U)
                            {
                                pre_unexpected_end_of_file(r.tail, ctx);
                                goto _CKL0;/*throw*/
                            }
                        }
                        token_list_pop_front(input_list);
                    }
                    _cake_zmem(&fullpath, 300);
                    _cake_zmem(&full_path_result, 200);
                    already_included = 0;
                    s = find_and_read_include_file(ctx, path, fullpath, is_angle_bracket_form, &already_included, full_path_result, 200U, 0);
                    has_include = s != 0U;
                    free((void *)s);
                    p_new_token = calloc(1, 40U);
                    if (p_new_token == 0U)
                    {
                        goto _CKL0;/*throw*/
                    }
                    p_new_token->type = 134;
                    temp = strdup(has_include ? "1" : "0");
                    if (temp == 0U)
                    {
                        token_delete(p_new_token);
                        goto _CKL0;/*throw*/
                    }
                    p_new_token->lexeme = temp;
                    p_new_token->flags |= 1;
                    token_list_add(&r, p_new_token);
                    token_list_pop_front(input_list);
                }
                else
                {
                    if (input_list->head->type == 8996 && strcmp(input_list->head->lexeme, "__has_c_attribute") == 0)
                    {
                        char path[100];
                        char * has_c_attribute_value;
                        struct token * p_new_token;
                        char * temp;

                        token_list_pop_front(input_list);
                        skip_blanks(ctx, &r, input_list);
                        token_list_pop_front(input_list);
                        skip_blanks(ctx, &r, input_list);
                        if (input_list->head == 0U)
                        {
                            pre_unexpected_end_of_file(r.tail, ctx);
                            goto _CKL0;/*throw*/
                        }
                        _cake_zmem(&path, 100);
                        while (input_list->head->type != 41)
                        {
                            strcat(path, input_list->head->lexeme);
                            token_list_pop_front(input_list);
                            if (input_list->head == 0U)
                            {
                                pre_unexpected_end_of_file(r.tail, ctx);
                                goto _CKL0;/*throw*/
                            }
                        }
                        token_list_pop_front(input_list);
                        has_c_attribute_value = "0";
                        if (strcmp(path, "nodiscard") == 0)
                        {
                            has_c_attribute_value = "202311L";
                        }
                        else
                        {
                            if (strcmp(path, "maybe_unused") == 0)
                            {
                                has_c_attribute_value = "202106L";
                            }
                            else
                            {
                                if (strcmp(path, "deprecated") == 0)
                                {
                                    has_c_attribute_value = "201904L";
                                }
                                else
                                {
                                    if (strcmp(path, "noreturn") == 0)
                                    {
                                        has_c_attribute_value = "202202L";
                                    }
                                    else
                                    {
                                        if (strcmp(path, "reproducible") == 0)
                                        {
                                        }
                                        else
                                        {
                                            if (strcmp(path, "unsequenced") == 0)
                                            {
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        p_new_token = calloc(1, 40U);
                        if (p_new_token == 0U)
                        {
                            goto _CKL0;/*throw*/
                        }
                        p_new_token->type = 134;
                        temp = strdup(has_c_attribute_value);
                        if (temp == 0U)
                        {
                            token_delete(p_new_token);
                            goto _CKL0;/*throw*/
                        }
                        p_new_token->lexeme = temp;
                        p_new_token->flags |= 1;
                        token_list_add(&r, p_new_token);
                        token_list_pop_front(input_list);
                    }
                    else
                    {
                        struct token * tk;

                        tk = token_list_pop_front_get(input_list);
                        if (tk)
                        {
                            token_list_add(&r, tk);
                        }
                    }
                }
            }
        }
    }
    else _CKL0: /*catch*/ 
    {
    }
    return r;
}



static void skip_blanks(struct preprocessor_ctx * ctx, struct token_list * dest, struct token_list * input_list)
{
    while (input_list->head)
    {
        struct token * p;

        if (!token_is_blank(input_list->head))
        {
            break;
        }
        p = token_list_pop_front_get(input_list);
        ;
        token_list_add(dest, p);
    }
}
struct token_list process_identifiers(struct preprocessor_ctx * ctx, struct token_list * list)
{
    struct token_list  list2;

    ;
    _cake_zmem(&list2, 8);
    if (1) /*try*/
    {
        while (list->head != 0U)
        {
            if (list->head->type == 8996 || list->head->type == 8997)
            {
                struct token * p_new_token;

                p_new_token = token_list_pop_front_get(list);
                ;
                p_new_token->type = 134;
                if (strcmp(p_new_token->lexeme, "true") == 0)
                {
                    p_new_token->lexeme[0] = 49;
                    p_new_token->lexeme[1] = 0;
                }
                else
                {
                    if (strcmp(p_new_token->lexeme, "false") == 0)
                    {
                        p_new_token->lexeme[0] = 48;
                        p_new_token->lexeme[1] = 0;
                    }
                    else
                    {
                        char * temp;

                        temp = strdup("0");
                        if (temp == 0U)
                        {
                            token_delete(p_new_token);
                            goto _CKL0;/*throw*/
                        }
                        free(p_new_token->lexeme);
                        p_new_token->lexeme = temp;
                    }
                }
                token_list_add(&list2, p_new_token);
            }
            else
            {
                struct token * ptk;

                ptk = token_list_pop_front_get(list);
                ;
                token_list_add(&list2, ptk);
            }
        }
        ;
    }
    else _CKL0: /*catch*/ 
    {
    }
    return list2;
}


struct token_list ignore_preprocessor_line(struct token_list * input_list)
{
    struct token_list  r;

    _cake_zmem(&r, 8);
    while (input_list->head && input_list->head->type != 10)
    {
        struct token * tk;

        tk = token_list_pop_front_get(input_list);
        ;
        token_list_add(&r, tk);
    }
    return r;
}


struct token_list copy_replacement_list(struct preprocessor_ctx * ctx, struct token_list * list);
void token_list_swap(struct token_list * a, struct token_list * b);
struct token_list preprocessor(struct preprocessor_ctx * ctx, struct token_list * input_list, int level);
int pre_constant_expression(struct preprocessor_ctx * ctx, long long * pvalue);

long long preprocessor_constant_expression(struct preprocessor_ctx * ctx, struct token_list * output_list, struct token_list * input_list, int level)
{
    struct token * first;
    struct token_list  r;
    struct token_list  list1;
    int flags;
    struct token_list  list2;
    long long value;

    ;
    first = input_list->head;
    ctx->conditional_inclusion = 1;
    _cake_zmem(&r, 8);
    while (input_list->head && input_list->head->type != 10)
    {
        struct token * tk;

        tk = token_list_pop_front_get(input_list);
        ;
        token_list_add(&r, tk);
        ;
        r.tail->flags &= -1025;
    }
    list1 = copy_replacement_list(ctx, &r);
    token_list_swap(output_list, &r);
    flags = ctx->flags;
    ctx->flags |= 1;
    list2 = preprocessor(ctx, &list1, 1);
    ctx->flags = flags;
    value = 0;
    if (list2.head == 0U)
    {
        preprocessor_diagnostic(1170, ctx, first, "empty expression");
    }
    else
    {
        struct token_list  list3;
        struct token_list  list4;
        struct preprocessor_ctx  pre_ctx;

        list3 = process_defined(ctx, &list2);
        list4 = process_identifiers(ctx, &list3);
        ;
        _cake_zmem(&pre_ctx, 752);
        pre_ctx.options = ctx->options;
        pre_ctx.input_list = list4;
        pre_ctx.current = pre_ctx.input_list.head;
        if (pre_constant_expression(&pre_ctx, &value) != 0)
        {
            preprocessor_diagnostic(1170, ctx, first, "expression error");
        }
        ctx->conditional_inclusion = 0;
        preprocessor_ctx_destroy(&pre_ctx);
    }
    token_list_destroy(&list1);
    token_list_destroy(&r);
    token_list_destroy(&list2);
    return value;
}


void match_level(struct token_list * dest, struct token_list * input_list, int level)
{
    if (1)
    {
        struct token * tk;

        tk = token_list_pop_front_get(input_list);
        if (tk)
        {
            token_list_add(dest, tk);
        }
    }
    else
    {
        token_list_pop_front(input_list);
    }
}


char *get_token_name(int tk);

int match_token_level(struct token_list * dest, struct token_list * input_list, int type, int level, struct preprocessor_ctx * ctx)
{
    if (1) /*try*/
    {
        if (input_list->head == 0U || input_list->head->type != type)
        {
            if (type == 10 && input_list->head == 0U)
            {
            }
            else
            {
                if (input_list->head)
                {
                    preprocessor_diagnostic(970, ctx, input_list->head, "expected token %s got %s\n", get_token_name(type), get_token_name(input_list->head->type));
                }
                else
                {
                    preprocessor_diagnostic(970, ctx, dest->tail, "expected EOF \n");
                }
                goto _CKL0;/*throw*/
            }
        }
        if (input_list->head != 0U)
        {
            if (1)
            {
                token_list_add(dest, token_list_pop_front_get(input_list));
            }
            else
            {
                token_list_pop_front(input_list);
            }
        }
    }
    else _CKL0: /*catch*/ 
    {
    }
    return ctx->n_errors > 0;
}


static void skip_blanks_level(struct preprocessor_ctx * ctx, struct token_list * dest, struct token_list * input_list, int level);

struct token_list if_group(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char is_active, int level, unsigned char * p_result)
{
    struct token_list  r;

    *p_result = 0;
    ;
    _cake_zmem(&r, 8);
    if (1) /*try*/
    {
        struct token_list  r2;

        match_token_level(&r, input_list, 127, level, ctx);
        skip_blanks_level(ctx, &r, input_list, level);
        if (input_list->head == 0U)
        {
            goto _CKL0;/*throw*/
        }
        ;
        if (strcmp(input_list->head->lexeme, "ifdef") == 0)
        {
            match_token_level(&r, input_list, 8996, level, ctx);
            skip_blanks_level(ctx, &r, input_list, level);
            if (input_list->head == 0U)
            {
                pre_unexpected_end_of_file(r.tail, ctx);
                goto _CKL0;/*throw*/
            }
            if (is_active)
            {
                struct macro * macro;

                macro = find_macro(ctx, input_list->head->lexeme);
                *p_result = (((macro != 0U) ? 1 : 0) != 0);
            }
            match_token_level(&r, input_list, 8996, level, ctx);
            skip_blanks_level(ctx, &r, input_list, level);
            match_token_level(&r, input_list, 10, level, ctx);
        }
        else
        {
            if (strcmp(input_list->head->lexeme, "ifndef") == 0)
            {
                match_token_level(&r, input_list, 8996, level, ctx);
                skip_blanks_level(ctx, &r, input_list, level);
                if (input_list->head == 0U)
                {
                    pre_unexpected_end_of_file(r.tail, ctx);
                    goto _CKL0;/*throw*/
                }
                if (is_active)
                {
                    struct macro * macro;

                    macro = find_macro(ctx, input_list->head->lexeme);
                    *p_result = (((macro == 0U) ? 1 : 0) != 0);
                }
                match_token_level(&r, input_list, 8996, level, ctx);
                skip_blanks_level(ctx, &r, input_list, level);
                match_token_level(&r, input_list, 10, level, ctx);
            }
            else
            {
                if (strcmp(input_list->head->lexeme, "if") == 0)
                {
                    match_token_level(&r, input_list, 8996, level, ctx);
                    skip_blanks_level(ctx, &r, input_list, level);
                    if (is_active)
                    {
                        struct token_list  r0;

                        _cake_zmem(&r0, 8);
                        *p_result = (preprocessor_constant_expression(ctx, &r0, input_list, level) != 0);
                        token_list_append_list(&r, &r0);
                        token_list_destroy(&r0);
                    }
                    else
                    {
                        struct token_list  r0;

                        r0 = ignore_preprocessor_line(input_list);
                        token_list_append_list(&r, &r0);
                        token_list_destroy(&r0);
                    }
                    match_token_level(&r, input_list, 10, level, ctx);
                }
                else
                {
                    preprocessor_diagnostic(650, ctx, input_list->head, "unexpected");
                    goto _CKL0;/*throw*/
                }
            }
        }
        r2 = group_opt(ctx, input_list, is_active && *p_result, level);
        token_list_append_list(&r, &r2);
        ;
        ;
    }
    else _CKL0: /*catch*/ 
    {
    }
    return r;
}



static void skip_blanks_level(struct preprocessor_ctx * ctx, struct token_list * dest, struct token_list * input_list, int level)
{
    while (input_list->head)
    {
        if (!token_is_blank(input_list->head))
        {
            break;
        }
        if (1)
        {
            struct token * p;

            p = token_list_pop_front_get(input_list);
            ;
            token_list_add(dest, p);
        }
        else
        {
            token_list_pop_front(input_list);
        }
    }
}
struct token_list elif_group(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char is_active, int level, unsigned char * p_elif_result)
{
    struct token_list  r;

    *p_elif_result = 0;
    ;
    _cake_zmem(&r, 8);
    if (1) /*try*/
    {
        unsigned long long result;
        struct token_list  r2;

        match_token_level(&r, input_list, 127, level, ctx);
        skip_blanks(ctx, &r, input_list);
        if (input_list->head == 0U)
        {
            goto _CKL0;/*throw*/
        }
        result = 0;
        if (strcmp(input_list->head->lexeme, "elif") == 0)
        {
            match_token_level(&r, input_list, 8996, level, ctx);
            skip_blanks(ctx, &r, input_list);
            if (is_active)
            {
                struct token_list  r0;

                _cake_zmem(&r0, 8);
                result = preprocessor_constant_expression(ctx, &r0, input_list, level);
                token_list_append_list(&r, &r0);
                token_list_destroy(&r0);
            }
            else
            {
                struct token_list  r0;

                r0 = ignore_preprocessor_line(input_list);
                token_list_append_list(&r, &r0);
                token_list_destroy(&r0);
            }
        }
        else
        {
            if (strcmp(input_list->head->lexeme, "elifdef") == 0)
            {
                match_token_level(&r, input_list, 8996, level, ctx);
                skip_blanks(ctx, &r, input_list);
                if (input_list->head == 0U)
                {
                    goto _CKL0;/*throw*/
                }
                if (is_active)
                {
                    result = (hashmap_find(&ctx->macros, input_list->head->lexeme) != 0U) ? 1 : 0;
                }
                match_token_level(&r, input_list, 8996, level, ctx);
            }
            else
            {
                if (strcmp(input_list->head->lexeme, "elifndef") == 0)
                {
                    match_token_level(&r, input_list, 8996, level, ctx);
                    skip_blanks(ctx, &r, input_list);
                    if (input_list->head == 0U)
                    {
                        goto _CKL0;/*throw*/
                    }
                    if (is_active)
                    {
                        result = (hashmap_find(&ctx->macros, input_list->head->lexeme) == 0U) ? 1 : 0;
                    }
                    match_token_level(&r, input_list, 8996, level, ctx);
                }
            }
        }
        *p_elif_result = (result != 0);
        skip_blanks(ctx, &r, input_list);
        match_token_level(&r, input_list, 10, level, ctx);
        r2 = group_opt(ctx, input_list, is_active && *p_elif_result, level);
        token_list_append_list(&r, &r2);
        token_list_destroy(&r2);
    }
    else _CKL0: /*catch*/ 
    {
    }
    return r;
}


struct token_list elif_groups(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char is_active, int level, unsigned char * pelif_result)
{
    struct token_list  r;

    ;
    _cake_zmem(&r, 8);
    if (1) /*try*/
    {
        unsigned char already_found_elif_true;
        unsigned char elif_result;
        struct token_list  r2;

        already_found_elif_true = 0;
        elif_result = 0;
        r2 = elif_group(ctx, input_list, is_active, level, &elif_result);
        if (input_list->head == 0U)
        {
            token_list_destroy(&r2);
            goto _CKL0;/*throw*/
        }
        token_list_append_list(&r, &r2);
        if (elif_result)
        {
            already_found_elif_true = 1;
        }
        if (input_list->head->type == 127 && (preprocessor_token_ahead_is_identifier(input_list->head, "elif") || preprocessor_token_ahead_is_identifier(input_list->head, "elifdef") || preprocessor_token_ahead_is_identifier(input_list->head, "elifndef")))
        {
            struct token_list  r3;

            r3 = elif_groups(ctx, input_list, is_active && !already_found_elif_true, level, &elif_result);
            token_list_append_list(&r, &r3);
            if (elif_result)
            {
                already_found_elif_true = 1;
            }
            token_list_destroy(&r3);
        }
        *pelif_result = already_found_elif_true;
        token_list_destroy(&r2);
    }
    else _CKL0: /*catch*/ 
    {
    }
    return r;
}


struct token_list else_group(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char is_active, int level)
{
    struct token_list  r;

    _cake_zmem(&r, 8);
    if (1) /*try*/
    {
        struct token_list  r2;

        match_token_level(&r, input_list, 127, level, ctx);
        skip_blanks_level(ctx, &r, input_list, level);
        if (ctx->n_errors > 0)
        {
            goto _CKL0;/*throw*/
        }
        match_token_level(&r, input_list, 8996, level, ctx);
        skip_blanks_level(ctx, &r, input_list, level);
        match_token_level(&r, input_list, 10, level, ctx);
        r2 = group_opt(ctx, input_list, is_active, level);
        token_list_append_list(&r, &r2);
        token_list_destroy(&r2);
    }
    else _CKL0: /*catch*/ 
    {
    }
    return r;
}


struct token_list endif_line(struct preprocessor_ctx * ctx, struct token_list * input_list, int level)
{
    struct token_list  r;

    _cake_zmem(&r, 8);
    match_token_level(&r, input_list, 127, level, ctx);
    skip_blanks_level(ctx, &r, input_list, level);
    match_token_level(&r, input_list, 8996, level, ctx);
    skip_blanks_level(ctx, &r, input_list, level);
    match_token_level(&r, input_list, 10, level, ctx);
    return r;
}


static unsigned char is_builtin_macro(char * name);
struct token_list identifier_list(struct preprocessor_ctx * ctx, struct macro * macro, struct token_list * input_list, int level);
void naming_convention_macro(struct preprocessor_ctx * ctx, struct token * token);

struct token_list def_line(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char is_active, int level, struct macro ** pp_macro)
{
    struct token_list  r;

    _cake_zmem(&r, 8);
    if (1) /*try*/
    {
        struct macro * macro;
        struct token * macro_name_token;
        char * temp;
        struct hash_item_set  item;

        macro = calloc(1, 32U);
        if (macro == 0U)
        {
            preprocessor_diagnostic(650, ctx, ctx->current, "out of mem");
            goto _CKL0;/*throw*/
        }
        macro->def_macro = 1;
        match_token_level(&r, input_list, 127, level, ctx);
        match_token_level(&r, input_list, 8996, level, ctx);
        skip_blanks_level(ctx, &r, input_list, level);
        if (input_list->head == 0U)
        {
            macro_delete(macro);
            pre_unexpected_end_of_file(r.tail, ctx);
            goto _CKL0;/*throw*/
        }
        macro_name_token = input_list->head;
        if (is_builtin_macro(macro_name_token->lexeme))
        {
            preprocessor_diagnostic(56, ctx, input_list->head, "redefining builtin macro");
        }
        if (hashmap_find(&ctx->macros, input_list->head->lexeme) != 0U)
        {
        }
        temp = strdup(input_list->head->lexeme);
        if (temp == 0U)
        {
            macro_delete(macro);
            goto _CKL0;/*throw*/
        }
        ;
        macro->name = temp;
        match_token_level(&r, input_list, 8996, level, ctx);
        if (input_list->head == 0U)
        {
            macro_delete(macro);
            pre_unexpected_end_of_file(r.tail, ctx);
            goto _CKL0;/*throw*/
        }
        if (input_list->head->type == 40)
        {
            macro->is_function = 1;
            match_token_level(&r, input_list, 40, level, ctx);
            skip_blanks_level(ctx, &r, input_list, level);
            if (input_list->head == 0U)
            {
                macro_delete(macro);
                pre_unexpected_end_of_file(r.tail, ctx);
                goto _CKL0;/*throw*/
            }
            if (input_list->head->type == 3026478)
            {
                struct macro_parameter * p_macro_parameter;
                char * temp2;

                p_macro_parameter = calloc(1, 20U);
                if (p_macro_parameter == 0U)
                {
                    macro_delete(macro);
                    goto _CKL0;/*throw*/
                }
                temp2 = strdup("__VA_ARGS__");
                if (temp2 == 0U)
                {
                    macro_delete(macro);
                    macro_parameters_delete(p_macro_parameter);
                    goto _CKL0;/*throw*/
                }
                p_macro_parameter->name = temp2;
                macro->parameters = p_macro_parameter;
                match_token_level(&r, input_list, 3026478, level, ctx);
                skip_blanks_level(ctx, &r, input_list, level);
                match_token_level(&r, input_list, 41, level, ctx);
            }
            else
            {
                if (input_list->head->type == 41)
                {
                    match_token_level(&r, input_list, 41, level, ctx);
                    skip_blanks_level(ctx, &r, input_list, level);
                }
                else
                {
                    struct token_list  r3;

                    r3 = identifier_list(ctx, macro, input_list, level);
                    token_list_append_list(&r, &r3);
                    token_list_destroy(&r3);
                    skip_blanks_level(ctx, &r, input_list, level);
                    if (input_list->head == 0U)
                    {
                        macro_delete(macro);
                        pre_unexpected_end_of_file(r.tail, ctx);
                        goto _CKL0;/*throw*/
                    }
                    if (input_list->head->type == 3026478)
                    {
                        struct macro_parameter * p_macro_parameter;
                        char * temp3;
                        struct macro_parameter * p_last;

                        p_macro_parameter = calloc(1, 20U);
                        if (p_macro_parameter == 0U)
                        {
                            macro_delete(macro);
                            goto _CKL0;/*throw*/
                        }
                        temp3 = strdup("__VA_ARGS__");
                        if (temp3 == 0U)
                        {
                            macro_delete(macro);
                            macro_parameters_delete(p_macro_parameter);
                            goto _CKL0;/*throw*/
                        }
                        p_macro_parameter->name = temp3;
                        p_last = macro->parameters;
                        ;
                        while (p_last->next)
                        {
                            p_last = p_last->next;
                        }
                        p_last->next = p_macro_parameter;
                        match_token_level(&r, input_list, 3026478, level, ctx);
                    }
                    skip_blanks_level(ctx, &r, input_list, level);
                    match_token_level(&r, input_list, 41, level, ctx);
                }
            }
        }
        else
        {
            macro->is_function = 0;
        }
        skip_blanks_level(ctx, &r, input_list, level);
        if (input_list->head == 0U)
        {
            macro_delete(macro);
            pre_unexpected_end_of_file(r.tail, ctx);
            goto _CKL0;/*throw*/
        }
        naming_convention_macro(ctx, macro_name_token);
        _cake_zmem(&item, 32);
        item.p_macro = macro;
        hashmap_set(&ctx->macros, macro->name, &item);
        hash_item_set_destroy(&item);
        *pp_macro = macro;
    }
    else _CKL0: /*catch*/ 
    {
    }
    return r;
}



static unsigned char is_builtin_macro(char * name)
{
    if (strcmp(name, "__FILE__") == 0)
    {
        return 1;
    }
    if (strcmp(name, "__CAKE__") == 0)
    {
        return 1;
    }
    return 0;
}
struct token_list replacement_group(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char is_active, int level)
{
    struct token_list  r;

    _cake_zmem(&r, 8);
    if (1) /*try*/
    {
        for (; ; )
        {
            if (input_list->head == 0U)
            {
                preprocessor_diagnostic(650, ctx, r.tail, "missing #enddef");
                goto _CKL0;/*throw*/
            }
            if (input_list->head->type == 127 && (preprocessor_token_ahead_is_identifier(input_list->head, "enddef")))
            {
                break;
            }
            prematch_level(&r, input_list, level);
        }
    }
    else _CKL0: /*catch*/ 
    {
    }
    return r;
}


struct token_list enddef_line(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char is_active, int level)
{
    struct token_list  r;

    _cake_zmem(&r, 8);
    if (1) /*try*/
    {
        if (input_list->head == 0U)
        {
            pre_unexpected_end_of_file(r.tail, ctx);
            goto _CKL0;/*throw*/
        }
        match_token_level(&r, input_list, 127, level, ctx);
        skip_blanks_level(ctx, &r, input_list, level);
        match_token_level(&r, input_list, 8996, level, ctx);
        skip_blanks_level(ctx, &r, input_list, level);
        match_token_level(&r, input_list, 10, level, ctx);
    }
    else _CKL0: /*catch*/ 
    {
    }
    return r;
}


struct token_list def_section(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char is_active, int level)
{
    struct token_list  r;

    _cake_zmem(&r, 8);
    if (1) /*try*/
    {
        struct macro * p_macro;
        struct token_list  r2;
        struct token_list  r3;
        struct token_list  copy;
        struct token_list  r4;

        p_macro = 0U;
        r2 = def_line(ctx, input_list, is_active, level, &p_macro);
        token_list_append_list(&r, &r2);
        if (ctx->n_errors > 0 || p_macro == 0U)
        {
            token_list_destroy(&r2);
            goto _CKL0;/*throw*/
        }
        r3 = replacement_group(ctx, input_list, is_active, level);
        if (ctx->n_errors > 0)
        {
            token_list_destroy(&r2);
            token_list_destroy(&r3);
            goto _CKL0;/*throw*/
        }
        copy = copy_replacement_list(ctx, &r3);
        token_list_append_list(&p_macro->replacement_list, &copy);
        token_list_append_list(&r, &r3);
        r4 = enddef_line(ctx, input_list, is_active, level);
        token_list_append_list(&r, &r4);
        token_list_destroy(&r2);
        token_list_destroy(&r3);
        token_list_destroy(&r4);
        token_list_destroy(&copy);
    }
    else _CKL0: /*catch*/ 
    {
    }
    return r;
}


struct token_list if_section(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char is_active, int level)
{
    struct token_list  r;

    ;
    _cake_zmem(&r, 8);
    if (1) /*try*/
    {
        unsigned char if_result;
        struct token_list  r2;
        unsigned char elif_result;
        struct token_list  r5;

        if_result = 0;
        r2 = if_group(ctx, input_list, is_active, level, &if_result);
        if (ctx->n_errors > 0)
        {
            token_list_destroy(&r2);
            goto _CKL0;/*throw*/
        }
        if (input_list->head == 0U)
        {
            token_list_destroy(&r2);
            goto _CKL0;/*throw*/
        }
        token_list_append_list(&r, &r2);
        elif_result = 0;
        if (input_list->head->type == 127 && (preprocessor_token_ahead_is_identifier(input_list->head, "elif") || preprocessor_token_ahead_is_identifier(input_list->head, "elifdef") || preprocessor_token_ahead_is_identifier(input_list->head, "elifndef")))
        {
            struct token_list  r3;

            r3 = elif_groups(ctx, input_list, is_active && !if_result, level, &elif_result);
            token_list_append_list(&r, &r3);
            token_list_destroy(&r3);
        }
        if (input_list->head == 0U)
        {
            token_list_destroy(&r2);
            pre_unexpected_end_of_file(r.tail, ctx);
            goto _CKL0;/*throw*/
        }
        if (input_list->head->type == 127 && preprocessor_token_ahead_is_identifier(input_list->head, "else"))
        {
            struct token_list  r4;

            r4 = else_group(ctx, input_list, is_active && !if_result && !elif_result, level);
            token_list_append_list(&r, &r4);
            token_list_destroy(&r4);
        }
        if (ctx->n_errors > 0)
        {
            token_list_destroy(&r2);
            goto _CKL0;/*throw*/
        }
        r5 = endif_line(ctx, input_list, level);
        token_list_append_list(&r, &r5);
        token_list_destroy(&r5);
        token_list_destroy(&r2);
    }
    else _CKL0: /*catch*/ 
    {
    }
    return r;
}


struct token_list identifier_list(struct preprocessor_ctx * ctx, struct macro * macro, struct token_list * input_list, int level)
{
    struct token_list  r;

    ;
    _cake_zmem(&r, 8);
    if (1) /*try*/
    {
        struct macro_parameter * p_macro_parameter;
        char * temp;
        struct macro_parameter * p_last_parameter;

        skip_blanks(ctx, &r, input_list);
        if (input_list->head == 0U)
        {
            goto _CKL0;/*throw*/
        }
        p_macro_parameter = calloc(1, 20U);
        if (p_macro_parameter == 0U)
        {
            goto _CKL0;/*throw*/
        }
        temp = strdup(input_list->head->lexeme);
        if (temp == 0U)
        {
            macro_parameters_delete(p_macro_parameter);
            goto _CKL0;/*throw*/
        }
        p_macro_parameter->name = temp;
        ;
        macro->parameters = p_macro_parameter;
        p_last_parameter = macro->parameters;
        match_token_level(&r, input_list, 8996, level, ctx);
        skip_blanks(ctx, &r, input_list);
        if (input_list->head == 0U)
        {
            goto _CKL0;/*throw*/
        }
        while (input_list->head->type == 44)
        {
            struct macro_parameter * p_new_macro_parameter;
            char * temp2;

            match_token_level(&r, input_list, 44, level, ctx);
            skip_blanks(ctx, &r, input_list);
            if (input_list->head == 0U)
            {
                goto _CKL0;/*throw*/
            }
            if (input_list->head->type == 3026478)
            {
                break;
            }
            p_new_macro_parameter = calloc(1, 20U);
            if (p_new_macro_parameter == 0U)
            {
                goto _CKL0;/*throw*/
            }
            temp2 = strdup(input_list->head->lexeme);
            if (temp2 == 0U)
            {
                macro_parameters_delete(p_new_macro_parameter);
                goto _CKL0;/*throw*/
            }
            p_new_macro_parameter->name = temp2;
            ;
            p_last_parameter->next = p_new_macro_parameter;
            p_last_parameter = p_last_parameter->next;
            match_token_level(&r, input_list, 8996, level, ctx);
            skip_blanks(ctx, &r, input_list);
            if (input_list->head == 0U)
            {
                goto _CKL0;/*throw*/
            }
        }
    }
    else _CKL0: /*catch*/ 
    {
    }
    return r;
}


struct token_list replacement_list(struct preprocessor_ctx * ctx, struct macro * macro, struct token_list * input_list, int level)
{
    struct token_list  r;

    _cake_zmem(&r, 8);
    if (1) /*try*/
    {
        struct token_list  copy;

        if (input_list->head == 0U)
        {
            pre_unexpected_end_of_file(0U, ctx);
            goto _CKL0;/*throw*/
        }
        while (input_list->head->type != 10)
        {
            match_level(&r, input_list, level);
            if (input_list->head == 0U)
            {
                break;
            }
        }
        ;
        copy = copy_replacement_list(ctx, &r);
        token_list_append_list(&macro->replacement_list, &copy);
        token_list_destroy(&copy);
    }
    else _CKL0: /*catch*/ 
    {
    }
    return r;
}


struct token_list pp_tokens_opt(struct preprocessor_ctx * ctx, struct token_list * input_list, int level)
{
    struct token_list  r;

    _cake_zmem(&r, 8);
    while (input_list->head && input_list->head->type != 10)
    {
        prematch_level(&r, input_list, level);
    }
    return r;
}


char *dirname(char * path);
static unsigned char is_empty_assert(struct token_list * replacement_list);
void *hashmap_remove(struct hash_map * map, char * key, int * p_type_opt);
static void pragma_once_add(struct preprocessor_ctx * ctx, char * path);
unsigned long long get_warning_bit_mask(char * wname);

struct token_list control_line(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char is_active, int level)
{
    struct token_list  r;

    _cake_zmem(&r, 8);
    if (1) /*try*/
    {
        if (!is_active)
        {
            struct token_list  r7;

            r7 = pp_tokens_opt(ctx, input_list, level);
            token_list_append_list(&r, &r7);
            match_token_level(&r, input_list, 10, level, ctx);
            token_list_destroy(&r7);
            return r;
        }
        if (input_list->head == 0U)
        {
            pre_unexpected_end_of_file(r.tail, ctx);
            goto _CKL0;/*throw*/
        }
        match_token_level(&r, input_list, 127, level, ctx);
        skip_blanks_level(ctx, &r, input_list, level);
        if (input_list->head == 0U)
        {
            pre_unexpected_end_of_file(r.tail, ctx);
            goto _CKL0;/*throw*/
        }
        if (strcmp(input_list->head->lexeme, "include") == 0 || strcmp(input_list->head->lexeme, "include_next") == 0)
        {
            unsigned char include_next;
            char path[100];
            unsigned char is_angle_bracket_form;
            char current_file_dir[300];
            char full_path_result[200];
            unsigned char already_included;
            char * content;

            include_next = strcmp(input_list->head->lexeme, "include_next") == 0;
            match_token_level(&r, input_list, 8996, level, ctx);
            skip_blanks_level(ctx, &r, input_list, level);
            if (input_list->head == 0U)
            {
                pre_unexpected_end_of_file(r.tail, ctx);
                goto _CKL0;/*throw*/
            }
            _cake_zmem(&path, 100);
            is_angle_bracket_form = 0;
            if (input_list->head->type == 130)
            {
                strcat(path, input_list->head->lexeme);
                prematch_level(&r, input_list, level);
            }
            else
            {
                is_angle_bracket_form = 1;
                while (input_list->head->type != 62)
                {
                    strcat(path, input_list->head->lexeme);
                    prematch_level(&r, input_list, level);
                    if (input_list->head == 0U)
                    {
                        pre_unexpected_end_of_file(r.tail, ctx);
                        goto _CKL0;/*throw*/
                    }
                }
                strcat(path, input_list->head->lexeme);
                prematch_level(&r, input_list, level);
            }
            while (input_list->head->type != 10)
            {
                prematch_level(&r, input_list, level);
                if (input_list->head == 0U)
                {
                    pre_unexpected_end_of_file(r.tail, ctx);
                    goto _CKL0;/*throw*/
                }
            }
            match_token_level(&r, input_list, 10, level, ctx);
            path[strlen(path) - 1] = 0;
            _cake_zmem(&current_file_dir, 300);
            snprintf(current_file_dir, 300U, "%s", r.tail->token_origin->lexeme);
            dirname(current_file_dir);
            _cake_zmem(&full_path_result, 200);
            already_included = 0;
            content = find_and_read_include_file(ctx, path + 1, current_file_dir, is_angle_bracket_form, &already_included, full_path_result, 200U, include_next);
            if (content != 0U)
            {
                struct tokenizer_ctx  tctx;
                struct token_list  list;
                struct token_list  list2;

                if (ctx->options.show_includes)
                {
                    {
                        int i;

                        i = 0;
                        for (; i < (level + 1); i++)
                        printf(".");
                    }
                    printf("%s\n", full_path_result);
                }
                _cake_zmem(&tctx, 696);
                list = tokenizer(&tctx, content, full_path_result, level + 1, 0);
                free((void *)content);
                list2 = preprocessor(ctx, &list, level + 1);
                token_list_append_list(&r, &list2);
                token_list_destroy(&list2);
                token_list_destroy(&list);
            }
            else
            {
                if (!already_included)
                {
                    preprocessor_diagnostic(1150, ctx, r.tail, "file %s not found", path + 1);
                    {
                        struct include_dir * p;

                        p = ctx->include_dir.head;
                        for (; p; p = p->next)
                        {
                            preprocessor_diagnostic(63, ctx, r.tail, "dir = '%s'", p->path);
                        }
                    }
                }
                else
                {
                }
            }
        }
        else
        {
            if (strcmp(input_list->head->lexeme, "embed") == 0)
            {
                struct token_list  discard0;
                struct token_list * p_list;
                struct token * p_embed_token;
                char path[100];
                char fullpath[300];
                int nlevel;
                int f;
                struct token_list  list;

                _cake_zmem(&discard0, 8);
                p_list = &r;
                p_embed_token = input_list->head;
                match_token_level(p_list, input_list, 8996, level, ctx);
                skip_blanks_level(ctx, p_list, input_list, level);
                if (input_list->head == 0U)
                {
                    goto _CKL0;/*throw*/
                }
                _cake_zmem(&path, 100);
                if (input_list->head->type == 130)
                {
                    strcat(path, input_list->head->lexeme);
                    prematch_level(p_list, input_list, level);
                }
                else
                {
                    while (input_list->head->type != 62)
                    {
                        strcat(path, input_list->head->lexeme);
                        prematch_level(p_list, input_list, level);
                        if (input_list->head == 0U)
                        {
                            goto _CKL0;/*throw*/
                        }
                    }
                    strcat(path, input_list->head->lexeme);
                    prematch_level(p_list, input_list, level);
                }
                if (input_list->head)
                {
                    while (input_list->head->type != 10)
                    {
                        prematch_level(p_list, input_list, level);
                        if (input_list->head == 0U)
                        {
                            pre_unexpected_end_of_file(p_list->tail, ctx);
                            goto _CKL0;/*throw*/
                        }
                    }
                }
                match_token_level(p_list, input_list, 10, level, ctx);
                _cake_zmem(&fullpath, 300);
                path[strlen(path) - 1] = 0;
                snprintf(fullpath, 300U, "%s", path + 1);
                nlevel = level;
                f = 0;
                f = 1;
                nlevel = nlevel + 1;
                list = embed_tokenizer(ctx, p_embed_token, fullpath, nlevel, f);
                if (ctx->n_errors > 0)
                {
                    token_list_destroy(&list);
                    goto _CKL0;/*throw*/
                }
                token_list_append_list(&r, &list);
                token_list_destroy(&list);
                token_list_destroy(&discard0);
            }
            else
            {
                if (strcmp(input_list->head->lexeme, "define") == 0)
                {
                    struct macro * macro;
                    struct token * macro_name_token;
                    char * temp;
                    struct token_list  r4;
                    struct macro * existing_macro;

                    macro = calloc(1, 32U);
                    if (macro == 0U)
                    {
                        preprocessor_diagnostic(650, ctx, ctx->current, "out of mem");
                        goto _CKL0;/*throw*/
                    }
                    match_token_level(&r, input_list, 8996, level, ctx);
                    skip_blanks_level(ctx, &r, input_list, level);
                    if (input_list->head == 0U)
                    {
                        macro_delete(macro);
                        pre_unexpected_end_of_file(r.tail, ctx);
                        goto _CKL0;/*throw*/
                    }
                    macro_name_token = input_list->head;
                    if (is_builtin_macro(macro_name_token->lexeme))
                    {
                        preprocessor_diagnostic(56, ctx, input_list->head, "redefining builtin macro");
                    }
                    macro->p_name_token = macro_name_token;
                    temp = strdup(input_list->head->lexeme);
                    if (temp == 0U)
                    {
                        macro_delete(macro);
                        goto _CKL0;/*throw*/
                    }
                    ;
                    macro->name = temp;
                    match_token_level(&r, input_list, 8996, level, ctx);
                    if (input_list->head == 0U)
                    {
                        macro_delete(macro);
                        pre_unexpected_end_of_file(r.tail, ctx);
                        goto _CKL0;/*throw*/
                    }
                    if (input_list->head->type == 40)
                    {
                        macro->is_function = 1;
                        match_token_level(&r, input_list, 40, level, ctx);
                        skip_blanks_level(ctx, &r, input_list, level);
                        if (input_list->head == 0U)
                        {
                            macro_delete(macro);
                            pre_unexpected_end_of_file(r.tail, ctx);
                            goto _CKL0;/*throw*/
                        }
                        if (input_list->head->type == 3026478)
                        {
                            struct macro_parameter * p_macro_parameter;
                            char * temp2;

                            p_macro_parameter = calloc(1, 20U);
                            if (p_macro_parameter == 0U)
                            {
                                macro_delete(macro);
                                goto _CKL0;/*throw*/
                            }
                            temp2 = strdup("__VA_ARGS__");
                            if (temp2 == 0U)
                            {
                                macro_delete(macro);
                                macro_parameters_delete(p_macro_parameter);
                                goto _CKL0;/*throw*/
                            }
                            p_macro_parameter->name = temp2;
                            macro->parameters = p_macro_parameter;
                            match_token_level(&r, input_list, 3026478, level, ctx);
                            skip_blanks_level(ctx, &r, input_list, level);
                            match_token_level(&r, input_list, 41, level, ctx);
                        }
                        else
                        {
                            if (input_list->head->type == 41)
                            {
                                match_token_level(&r, input_list, 41, level, ctx);
                                skip_blanks_level(ctx, &r, input_list, level);
                            }
                            else
                            {
                                struct token_list  r3;

                                r3 = identifier_list(ctx, macro, input_list, level);
                                token_list_append_list(&r, &r3);
                                token_list_destroy(&r3);
                                skip_blanks_level(ctx, &r, input_list, level);
                                if (input_list->head == 0U)
                                {
                                    macro_delete(macro);
                                    pre_unexpected_end_of_file(r.tail, ctx);
                                    goto _CKL0;/*throw*/
                                }
                                if (input_list->head->type == 3026478)
                                {
                                    struct macro_parameter * p_macro_parameter;
                                    char * temp3;
                                    struct macro_parameter * p_last;

                                    p_macro_parameter = calloc(1, 20U);
                                    if (p_macro_parameter == 0U)
                                    {
                                        macro_delete(macro);
                                        goto _CKL0;/*throw*/
                                    }
                                    temp3 = strdup("__VA_ARGS__");
                                    if (temp3 == 0U)
                                    {
                                        macro_delete(macro);
                                        macro_parameters_delete(p_macro_parameter);
                                        goto _CKL0;/*throw*/
                                    }
                                    p_macro_parameter->name = temp3;
                                    p_last = macro->parameters;
                                    ;
                                    while (p_last->next)
                                    {
                                        p_last = p_last->next;
                                    }
                                    p_last->next = p_macro_parameter;
                                    match_token_level(&r, input_list, 3026478, level, ctx);
                                }
                                skip_blanks_level(ctx, &r, input_list, level);
                                match_token_level(&r, input_list, 41, level, ctx);
                            }
                        }
                    }
                    else
                    {
                        macro->is_function = 0;
                    }
                    skip_blanks_level(ctx, &r, input_list, level);
                    if (input_list->head == 0U)
                    {
                        macro_delete(macro);
                        pre_unexpected_end_of_file(r.tail, ctx);
                        goto _CKL0;/*throw*/
                    }
                    r4 = replacement_list(ctx, macro, input_list, level);
                    token_list_append_list(&r, &r4);
                    token_list_destroy(&r4);
                    match_token_level(&r, input_list, 10, level, ctx);
                    if (!ctx->options.disable_assert && strcmp(macro->name, "assert") == 0)
                    {
                        if (!is_empty_assert(&macro->replacement_list))
                        {
                            struct macro_parameter * p_macro_parameter;
                            char * temp2;
                            struct tokenizer_ctx  tctx;

                            macro_parameters_delete(macro->parameters);
                            p_macro_parameter = calloc(1, 20U);
                            if (p_macro_parameter == 0U)
                            {
                                macro_delete(macro);
                                goto _CKL0;/*throw*/
                            }
                            temp2 = strdup("__VA_ARGS__");
                            if (temp2 == 0U)
                            {
                                macro_delete(macro);
                                macro_parameters_delete(p_macro_parameter);
                                goto _CKL0;/*throw*/
                            }
                            p_macro_parameter->name = temp2;
                            macro->parameters = p_macro_parameter;
                            token_list_destroy(&macro->replacement_list);
                            _cake_zmem(&tctx, 696);
                            macro->replacement_list = tokenizer(&tctx, "assert(__VA_ARGS__)", 0U, level, 0);
                        }
                    }
                    naming_convention_macro(ctx, macro_name_token);
                    existing_macro = find_macro(ctx, macro->name);
                    if (existing_macro)
                    {
                        if (!macro_is_same(macro, existing_macro))
                        {
                            preprocessor_diagnostic(1820, ctx, macro->p_name_token, "macro redefinition");
                            preprocessor_diagnostic(63, ctx, existing_macro->p_name_token, "previous definition");
                            macro_delete(macro);
                            goto _CKL0;/*throw*/
                        }
                        macro_delete(macro);
                    }
                    else
                    {
                        struct hash_item_set  item;

                        _cake_zmem(&item, 32);
                        item.p_macro = macro;
                        hashmap_set(&ctx->macros, macro->name, &item);
                        hash_item_set_destroy(&item);
                    }
                }
                else
                {
                    if (strcmp(input_list->head->lexeme, "undef") == 0)
                    {
                        struct macro * macro;

                        match_token_level(&r, input_list, 8996, level, ctx);
                        skip_blanks_level(ctx, &r, input_list, level);
                        if (input_list->head == 0U)
                        {
                            pre_unexpected_end_of_file(r.tail, ctx);
                            goto _CKL0;/*throw*/
                        }
                        macro = (struct macro *)hashmap_remove(&ctx->macros, input_list->head->lexeme, 0U);
                        ;
                        if (macro)
                        {
                            macro_delete(macro);
                            match_token_level(&r, input_list, 8996, level, ctx);
                        }
                        else
                        {
                            match_token_level(&r, input_list, 8996, level, ctx);
                        }
                        skip_blanks_level(ctx, &r, input_list, level);
                        match_token_level(&r, input_list, 10, level, ctx);
                    }
                    else
                    {
                        if (strcmp(input_list->head->lexeme, "line") == 0)
                        {
                            struct token_list  r5;

                            match_token_level(&r, input_list, 8996, level, ctx);
                            r5 = pp_tokens_opt(ctx, input_list, level);
                            token_list_append_list(&r, &r5);
                            token_list_destroy(&r5);
                            match_token_level(&r, input_list, 10, level, ctx);
                        }
                        else
                        {
                            if (strcmp(input_list->head->lexeme, "error") == 0)
                            {
                                struct token_list  r6;

                                match_token_level(&r, input_list, 8996, level, ctx);
                                r6 = pp_tokens_opt(ctx, input_list, level);
                                token_list_append_list(&r, &r6);
                                token_list_destroy(&r6);
                                match_token_level(&r, input_list, 10, level, ctx);
                                preprocessor_diagnostic(1180, ctx, r.head, "#error");
                            }
                            else
                            {
                                if (strcmp(input_list->head->lexeme, "warning") == 0)
                                {
                                    struct token_list  r6;

                                    match_token_level(&r, input_list, 8996, level, ctx);
                                    r6 = pp_tokens_opt(ctx, input_list, level);
                                    token_list_append_list(&r, &r6);
                                    match_token_level(&r, input_list, 10, level, ctx);
                                    preprocessor_diagnostic(1, ctx, r.head, "#warning");
                                    token_list_destroy(&r6);
                                }
                                else
                                {
                                    if (strcmp(input_list->head->lexeme, "pragma") == 0)
                                    {
                                        struct token_list  r7;

                                        match_token_level(&r, input_list, 8996, level, ctx);
                                        if (r.tail)
                                        {
                                            r.tail->type = 128;
                                            r.tail->flags |= 1;
                                        }
                                        skip_blanks_level(ctx, &r, input_list, level);
                                        if (input_list->head == 0U)
                                        {
                                            pre_unexpected_end_of_file(r.tail, ctx);
                                            goto _CKL0;/*throw*/
                                        }
                                        if (input_list->head->type == 8996)
                                        {
                                            if (strcmp(input_list->head->lexeme, "CAKE") == 0)
                                            {
                                                match_token_level(&r, input_list, 8996, level, ctx);
                                                if (r.tail)
                                                {
                                                    r.tail->flags |= 1;
                                                }
                                                skip_blanks_level(ctx, &r, input_list, level);
                                            }
                                            if (input_list->head == 0U)
                                            {
                                                pre_unexpected_end_of_file(r.tail, ctx);
                                                goto _CKL0;/*throw*/
                                            }
                                            if (strcmp(input_list->head->lexeme, "once") == 0)
                                            {
                                                pragma_once_add(ctx, input_list->head->token_origin->lexeme);
                                                match_token_level(&r, input_list, 8996, level, ctx);
                                                if (r.tail)
                                                {
                                                    r.tail->flags |= 1;
                                                }
                                            }
                                            else
                                            {
                                                if (strcmp(input_list->head->lexeme, "dir") == 0)
                                                {
                                                    char path[200];

                                                    match_token_level(&r, input_list, 8996, level, ctx);
                                                    skip_blanks_level(ctx, &r, input_list, level);
                                                    if (input_list->head == 0U)
                                                    {
                                                        pre_unexpected_end_of_file(r.tail, ctx);
                                                        goto _CKL0;/*throw*/
                                                    }
                                                    if (input_list->head->type != 130)
                                                    {
                                                        preprocessor_diagnostic(650, ctx, input_list->head, "expected string");
                                                        goto _CKL0;/*throw*/
                                                    }
                                                    _cake_zmem(&path, 200);
                                                    strncpy(path, input_list->head->lexeme + 1, strlen(input_list->head->lexeme) - 2);
                                                    include_dir_add(&ctx->include_dir, path);
                                                    match_token_level(&r, input_list, 130, level, ctx);
                                                    if (r.tail)
                                                    {
                                                        r.tail->flags |= 1;
                                                    }
                                                }
                                                else
                                                {
                                                    if (strcmp(input_list->head->lexeme, "nullchecks") == 0)
                                                    {
                                                        ;
                                                        match_token_level(&r, input_list, 8996, level, ctx);
                                                        ;
                                                        r.tail->flags |= 1;
                                                        skip_blanks_level(ctx, &r, input_list, level);
                                                        ctx->options.null_checks_enabled = 1;
                                                    }
                                                }
                                            }
                                            if (input_list->head == 0U)
                                            {
                                                pre_unexpected_end_of_file(r.tail, ctx);
                                                goto _CKL0;/*throw*/
                                            }
                                            if (strcmp(input_list->head->lexeme, "diagnostic") == 0)
                                            {
                                                match_token_level(&r, input_list, 8996, level, ctx);
                                                ;
                                                r.tail->flags |= 1;
                                                skip_blanks_level(ctx, &r, input_list, level);
                                                if (input_list->head == 0U)
                                                {
                                                    pre_unexpected_end_of_file(r.tail, ctx);
                                                    goto _CKL0;/*throw*/
                                                }
                                                if (strcmp(input_list->head->lexeme, "push") == 0)
                                                {
                                                    match_token_level(&r, input_list, 8996, level, ctx);
                                                    ;
                                                    r.tail->flags |= 1;
                                                    if (ctx->options.diagnostic_stack.top_index < 10U)
                                                    {
                                                        ctx->options.diagnostic_stack.top_index++;
                                                        ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index] = ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index - 1];
                                                    }
                                                }
                                                else
                                                {
                                                    if (strcmp(input_list->head->lexeme, "pop") == 0)
                                                    {
                                                        match_token_level(&r, input_list, 8996, level, ctx);
                                                        ;
                                                        r.tail->flags |= 1;
                                                        if (ctx->options.diagnostic_stack.top_index > 0)
                                                        {
                                                            ctx->options.diagnostic_stack.top_index--;
                                                        }
                                                    }
                                                    else
                                                    {
                                                        if (strcmp(input_list->head->lexeme, "warning") == 0)
                                                        {
                                                            match_token_level(&r, input_list, 8996, level, ctx);
                                                            ;
                                                            r.tail->flags |= 1;
                                                            skip_blanks_level(ctx, &r, input_list, level);
                                                            if (input_list->head && input_list->head->type == 130)
                                                            {
                                                                unsigned long long w;

                                                                w = get_warning_bit_mask(input_list->head->lexeme + 1);
                                                                match_token_level(&r, input_list, 130, level, ctx);
                                                                ;
                                                                r.tail->flags |= 1;
                                                                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings |= w;
                                                            }
                                                        }
                                                        else
                                                        {
                                                            if (strcmp(input_list->head->lexeme, "ignored") == 0)
                                                            {
                                                                match_token_level(&r, input_list, 8996, level, ctx);
                                                                ;
                                                                r.tail->flags |= 1;
                                                                skip_blanks_level(ctx, &r, input_list, level);
                                                                if (input_list->head && input_list->head->type == 130)
                                                                {
                                                                    unsigned long long w;

                                                                    w = get_warning_bit_mask(input_list->head->lexeme + 1);
                                                                    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings &= ~w;
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        r7 = pp_tokens_opt(ctx, input_list, level);
                                        token_list_append_list(&r, &r7);
                                        match_token_level(&r, input_list, 10, level, ctx);
                                        ;
                                        r.tail->type = 129;
                                        r.tail->flags |= 1;
                                        token_list_destroy(&r7);
                                    }
                                    else
                                    {
                                        if (input_list->head->type == 10)
                                        {
                                            skip_blanks_level(ctx, &r, input_list, level);
                                            match_token_level(&r, input_list, 10, level, ctx);
                                        }
                                        else
                                        {
                                            preprocessor_diagnostic(970, ctx, input_list->head, "unexpected\n");
                                            goto _CKL0;/*throw*/
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else _CKL0: /*catch*/ 
    {
    }
    return r;
}



static unsigned char is_empty_assert(struct token_list * replacement_list)
{
    struct token * token;

    token = replacement_list->head;
    if (token == 0U)
    {
        return 0;
    }
    if (strcmp(token->lexeme, "("))
    {
        return 0;
    }
    token = token->next;
    if (token == 0U)
    {
        return 0;
    }
    if (strcmp(token->lexeme, "("))
    {
        return 0;
    }
    token = token->next;
    if (token == 0U)
    {
        return 0;
    }
    if (strcmp(token->lexeme, "void"))
    {
        return 0;
    }
    token = token->next;
    if (token == 0U)
    {
        return 0;
    }
    if (strcmp(token->lexeme, ")"))
    {
        return 0;
    }
    token = token->next;
    if (token == 0U)
    {
        return 0;
    }
    if (strcmp(token->lexeme, "0"))
    {
        return 0;
    }
    token = token->next;
    if (token == 0U)
    {
        return 0;
    }
    if (strcmp(token->lexeme, ")"))
    {
        return 0;
    }
    token = token->next;
    if (token != 0U)
    {
        return 0;
    }
    return 1;
}

static void pragma_once_add(struct preprocessor_ctx * ctx, char * path)
{
    struct hash_item_set  item;

    _cake_zmem(&item, 32);
    item.number = 1;
    hashmap_set(&ctx->pragma_once_map, path, &item);
    hash_item_set_destroy(&item);
}
static struct token_list concatenate(struct preprocessor_ctx * ctx, struct token_list * input_list);
int ss_fprintf(struct osstream * stream, char * fmt, ...);
void ss_close(struct osstream * stream);
void token_list_pop_back(struct token_list * list);
static struct token_list operator_pragma(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char is_active, int level);
static char *decode_pragma_string(char * literal);
void *malloc(unsigned int size);
void token_list_clear(struct token_list * list);
static unsigned char macro_already_expanded(struct macro_expanded * p_list, char * name);
static struct macro_argument_list collect_macro_arguments(struct preprocessor_ctx * ctx, struct macro * macro, struct token_list * input_list, int level);
struct token_list expand_macro(struct preprocessor_ctx * ctx, struct macro_expanded * p_list, struct macro * macro, struct macro_argument_list * arguments, int level, struct token * origin);
void token_list_append_list_at_beginning(struct token_list * dest, struct token_list * source);

struct token_list replacement_list_reexamination(struct preprocessor_ctx * ctx, struct macro_expanded * p_list, struct token_list * oldlist, int level, struct token * origin)
{
    struct token_list  r;

    _cake_zmem(&r, 8);
    if (1) /*try*/
    {
        struct token_list  new_list;

        new_list = concatenate(ctx, oldlist);
        while (new_list.head != 0U)
        {
            struct macro * macro;

            if (new_list.head->type == 8996 && strcmp(new_list.head->lexeme, "_Pragma") == 0)
            {
                struct token_list  list;

                list = operator_pragma(ctx, &new_list, 1, level);
                token_list_append_list(&new_list, &list);
                continue;
            }
            macro = 0U;
            if (new_list.head->type == 8996)
            {
                macro = find_macro(ctx, new_list.head->lexeme);
                if (macro && macro->is_function && !preprocessor_token_ahead_is(new_list.head, 40))
                {
                    macro = 0U;
                }
                if (macro && macro_already_expanded(p_list, new_list.head->lexeme))
                {
                    new_list.head->type = 8997;
                    macro = 0U;
                }
                if (ctx->conditional_inclusion)
                {
                    if (r.tail && r.tail->type == 8996 && strcmp(r.tail->lexeme, "defined") == 0)
                    {
                        macro = 0U;
                    }
                    else
                    {
                        if (r.tail && r.tail->type == 40)
                        {
                            struct token * previous;

                            previous = r.tail->prev;
                            if (previous != 0U && previous->type == 8996 && strcmp(previous->lexeme, "defined") == 0)
                            {
                                macro = 0U;
                            }
                        }
                    }
                }
            }
            if (macro)
            {
                int flags;
                struct macro_argument_list  arguments;
                struct token_list  r3;

                flags = new_list.head->flags;
                arguments = collect_macro_arguments(ctx, macro, &new_list, level);
                if (ctx->n_errors > 0)
                {
                    macro_argument_list_destroy(&arguments);
                    token_list_destroy(&new_list);
                    goto _CKL0;/*throw*/
                }
                r3 = expand_macro(ctx, p_list, macro, &arguments, level, origin);
                if (ctx->n_errors > 0)
                {
                    token_list_destroy(&new_list);
                    token_list_destroy(&r3);
                    macro_argument_list_destroy(&arguments);
                    goto _CKL0;/*throw*/
                }
                if (r3.head)
                {
                    r3.head->flags = flags;
                }
                token_list_append_list_at_beginning(&new_list, &r3);
                macro_argument_list_destroy(&arguments);
                token_list_destroy(&r3);
            }
            else
            {
                new_list.head->level = level;
                new_list.head->flags |= 2;
                prematch(&r, &new_list);
            }
        }
    }
    else _CKL0: /*catch*/ 
    {
    }
    return r;
}



static struct token_list concatenate(struct preprocessor_ctx * ctx, struct token_list * input_list)
{
    struct token_list  r;

    _cake_zmem(&r, 8);
    if (1) /*try*/
    {
        while (input_list->head)
        {
            if (input_list->head->type == 8995)
            {
                struct osstream  ss;
                int level;
                struct tokenizer_ctx  tctx;
                struct token_list  newlist;

                if (r.tail == 0U)
                {
                    preprocessor_diagnostic(1210, ctx, input_list->head, "missing macro argument (should be checked before)");
                    break;
                }
                token_list_pop_front(input_list);
                _cake_zmem(&ss, 12);
                if (r.tail->lexeme[0] != 0)
                {
                    ss_fprintf(&ss, "%s", r.tail->lexeme);
                }
                if (input_list->head && input_list->head->lexeme[0] != 0)
                {
                    ss_fprintf(&ss, "%s", input_list->head->lexeme);
                }
                level = input_list->head ? input_list->head->level : 0;
                token_list_pop_front(input_list);
                _cake_zmem(&tctx, 696);
                _cake_zmem(&newlist, 8);
                if (ss.c_str != 0U)
                {
                    newlist = tokenizer(&tctx, ss.c_str, 0U, level, 0);
                }
                if (newlist.head)
                {
                    newlist.head->flags = r.tail->flags;
                }
                else
                {
                    struct token * p_new_token;
                    char * temp;

                    p_new_token = calloc(1, 40U);
                    if (p_new_token == 0U)
                    {
                        ss_close(&ss);
                        goto _CKL0;/*throw*/
                    }
                    temp = strdup("");
                    if (temp == 0U)
                    {
                        ss_close(&ss);
                        token_delete(p_new_token);
                        goto _CKL0;/*throw*/
                    }
                    p_new_token->lexeme = temp;
                    p_new_token->type = 142;
                    token_list_add(&newlist, p_new_token);
                    ;
                    newlist.head->flags = r.tail->flags;
                }
                token_list_pop_back(&r);
                token_list_append_list(&r, &newlist);
                ss_close(&ss);
                token_list_destroy(&newlist);
                if (input_list->head == 0U)
                {
                    break;
                }
            }
            else
            {
                prematch(&r, input_list);
            }
        }
    }
    else _CKL0: /*catch*/ 
    {
    }
    return r;
}

static char *decode_pragma_string(char * literal)
{
    unsigned int len;
    char * p;
    char * result;
    char * out;

    while (*literal != 34)
    literal++;
    literal++;
    len = 0;
    p = literal;
    p = literal;
    while (*p && *p != 34)
    {
        if (*p == 92 && *(p + 1))
        {
            p++;
            /*switch*/
            {
                register char __Ck2_temp = *p;
                if (__Ck2_temp == 34) goto _CKL2; /*case 34*/
                if (__Ck2_temp == 92) goto _CKL3; /*case 92*/
                goto _CKL4;/*default*/

                {
                    _CKL2: /*case 34*/ 
                    _CKL3: /*case 92*/ 
                    goto _CKL1; /*break*/

                    _CKL4: /*default*/ 
                    len++;
                }
                _CKL1:;
            }
        }
        len++;
        p++;
    }
    result = (char *)malloc(len + 1);
    if (!result)
    {
        return 0U;
    }
    out = result;
    p = literal;
    while (*p && *p != 34)
    {
        if (*p == 92 && *(p + 1))
        {
            p++;
            /*switch*/
            {
                register char __Ck3_temp = *p;
                if (__Ck3_temp == 34) goto _CKL8; /*case 34*/
                if (__Ck3_temp == 92) goto _CKL9; /*case 92*/
                goto _CKL10;/*default*/

                {
                    _CKL8: /*case 34*/ 
                    _CKL9: /*case 92*/ 
                    goto _CKL7; /*break*/

                    _CKL10: /*default*/ 
                    *out++ = 92;
                    goto _CKL7; /*break*/

                }
                _CKL7:;
            }
        }
        *out++ = *p;
        p++;
    }
    *out = 0;
    return result;
}

static struct token_list operator_pragma(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char is_active, int level)
{
    struct token_list  r;

    _cake_zmem(&r, 8);
    if (1) /*try*/
    {
        char * line;
        struct tokenizer_ctx  tctx;
        struct token_list  r0;

        if (input_list->head->type != 8996)
        {
            goto _CKL0;/*throw*/
        }
        prematch(&r, input_list);
        r.tail->type = 128;
        r.tail->flags |= 1;
        skip_blanks_level(ctx, &r, input_list, level);
        if (input_list->head->type != 40)
        {
            preprocessor_diagnostic(970, ctx, input_list->head, "expected (");
            goto _CKL0;/*throw*/
        }
        token_list_pop_front(input_list);
        skip_blanks_level(ctx, &r, input_list, level);
        if (input_list->head->type != 130)
        {
            preprocessor_diagnostic(970, ctx, input_list->head, "expected string");
            goto _CKL0;/*throw*/
        }
        line = decode_pragma_string(input_list->head->lexeme);
        if (line == 0U)
        {
            goto _CKL0;/*throw*/
        }
        token_list_pop_front(input_list);
        _cake_zmem(&tctx, 696);
        r0 = tokenizer(&tctx, line, "", 0, 0);
        free(line);
        token_list_pop_front(&r0);
        token_list_append_list(&r, &r0);
        skip_blanks_level(ctx, &r, input_list, level);
        if (input_list->head->type != 41)
        {
            preprocessor_diagnostic(970, ctx, input_list->head, "expected (");
            goto _CKL0;/*throw*/
        }
        prematch(&r, input_list);
        r.tail->type = 129;
        r.tail->flags |= 1;
    }
    else _CKL0: /*catch*/ 
    {
        token_list_clear(&r);
    }
    return r;
}

static unsigned char macro_already_expanded(struct macro_expanded * p_list, char * name)
{
    struct macro_expanded * p_item;

    p_item = p_list;
    while (p_item)
    {
        if (strcmp(name, p_item->name) == 0)
        {
            return 1;
        }
        p_item = p_item->p_previous;
    }
    return 0;
}

static struct macro_argument_list collect_macro_arguments(struct preprocessor_ctx * ctx, struct macro * macro, struct token_list * input_list, int level)
{
    struct macro_argument_list  macro_argument_list;

    _cake_zmem(&macro_argument_list, 16);
    if (1) /*try*/
    {
        struct token * macro_name_token;
        int count;
        struct macro_parameter * p_current_parameter;
        struct macro_argument * p_argument;

        if (input_list->head == 0U)
        {
            goto _CKL0;/*throw*/
        }
        ;
        macro_name_token = input_list->head;
        match_token_level(&macro_argument_list.tokens, input_list, 8996, level, ctx);
        if (!macro->is_function)
        {
            return macro_argument_list;
        }
        count = 1;
        skip_blanks(ctx, &macro_argument_list.tokens, input_list);
        match_token_level(&macro_argument_list.tokens, input_list, 40, level, ctx);
        skip_blanks(ctx, &macro_argument_list.tokens, input_list);
        if (input_list->head == 0U)
        {
            pre_unexpected_end_of_file(macro_argument_list.tokens.tail, ctx);
            goto _CKL0;/*throw*/
        }
        if (input_list->head->type == 41)
        {
            if (macro->parameters != 0U)
            {
                struct macro_argument * p_argument;
                struct macro_parameter * p_current_parameter;

                p_argument = calloc(1, 16U);
                if (p_argument == 0U)
                {
                    goto _CKL0;/*throw*/
                }
                p_current_parameter = macro->parameters;
                p_argument->macro_parameter = p_current_parameter;
                argument_list_add(&macro_argument_list, p_argument);
            }
            match_token_level(&macro_argument_list.tokens, input_list, 41, level, ctx);
            return macro_argument_list;
        }
        if (macro->parameters == 0U)
        {
            preprocessor_diagnostic(1191, ctx, macro_name_token, "too many arguments provided to function-like macro invocation\n");
            goto _CKL0;/*throw*/
        }
        p_current_parameter = macro->parameters;
        p_argument = calloc(1, 16U);
        if (p_argument == 0U)
        {
            goto _CKL0;/*throw*/
        }
        p_argument->macro_parameter = p_current_parameter;
        while (input_list->head != 0U)
        {
            if (input_list->head->type == 40)
            {
                count++;
                token_list_clone_and_add(&p_argument->tokens, input_list->head);
                match_token_level(&macro_argument_list.tokens, input_list, 40, level, ctx);
            }
            else
            {
                if (input_list->head->type == 41)
                {
                    count--;
                    if (count == 0)
                    {
                        match_token_level(&macro_argument_list.tokens, input_list, 41, level, ctx);
                        argument_list_add(&macro_argument_list, p_argument);
                        p_argument = 0U;
                        if (p_current_parameter->next != 0U)
                        {
                            p_current_parameter = p_current_parameter->next;
                            if (strcmp(p_current_parameter->name, "__VA_ARGS__") == 0)
                            {
                                p_argument = calloc(1, 16U);
                                if (p_argument == 0U)
                                {
                                    goto _CKL0;/*throw*/
                                }
                                p_argument->macro_parameter = p_current_parameter;
                                argument_list_add(&macro_argument_list, p_argument);
                                p_argument = 0U;
                            }
                            else
                            {
                                preprocessor_diagnostic(1190, ctx, macro_name_token, "too few arguments provided to function-like macro invocation\n");
                                goto _CKL0;/*throw*/
                            }
                        }
                        break;
                    }
                    else
                    {
                        token_list_clone_and_add(&p_argument->tokens, input_list->head);
                        match_token_level(&macro_argument_list.tokens, input_list, 41, level, ctx);
                    }
                }
                else
                {
                    if (count == 1 && input_list->head->type == 44)
                    {
                        if (strcmp(p_current_parameter->name, "__VA_ARGS__") == 0)
                        {
                            token_list_clone_and_add(&p_argument->tokens, input_list->head);
                            match_token_level(&macro_argument_list.tokens, input_list, 44, level, ctx);
                        }
                        else
                        {
                            match_token_level(&macro_argument_list.tokens, input_list, 44, level, ctx);
                            argument_list_add(&macro_argument_list, p_argument);
                            p_argument = 0U;
                            p_argument = calloc(1, 16U);
                            if (p_argument == 0U)
                            {
                                goto _CKL0;/*throw*/
                            }
                            if (p_current_parameter->next == 0U)
                            {
                                preprocessor_diagnostic(1191, ctx, macro_argument_list.tokens.tail, "too many arguments provided to function-like macro invocation\n");
                                macro_argument_delete(p_argument);
                                p_argument = 0U;
                                goto _CKL0;/*throw*/
                            }
                            p_current_parameter = p_current_parameter->next;
                            p_argument->macro_parameter = p_current_parameter;
                        }
                    }
                    else
                    {
                        token_list_clone_and_add(&p_argument->tokens, input_list->head);
                        prematch_level(&macro_argument_list.tokens, input_list, level);
                    }
                }
            }
        }
        ;
    }
    else _CKL0: /*catch*/ 
    {
    }
    return macro_argument_list;
}
int lexeme_cmp(char * s1, char * s2)
{
    while (*s1 && *s2)
    {
        while ((s1[0] == 92 && s1[1] == 10))
        {
            s1++;
            s1++;
        }
        while (s2[0] == 92 && s2[1] == 10)
        {
            s2++;
            s2++;
        }
        if (*s1 != *s2)
        {
            break;
        }
        s1++;
        s2++;
    }
    while ((s1[0] == 92 && s1[1] == 10))
    {
        s1++;
        s1++;
    }
    while (s2[0] == 92 && s2[1] == 10)
    {
        s2++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}


void remove_line_continuation(char * s)
{
    char * pread;
    char * pwrite;

    pread = s;
    pwrite = s;
    while (*pread)
    {
        if (pread[0] == 92 && (pread[1] == 10 || (pread[1] == 13 && pread[2] == 10)))
        {
            if (pread[1] == 13 && pread[2] == 10)
            {
                pread++;
                pread++;
                pread++;
            }
            else
            {
                pread++;
                pread++;
            }
        }
        else
        {
            *pwrite = *pread;
            pread++;
            pwrite++;
        }
    }
    *pwrite = *pread;
}


struct token_list copy_replacement_list_core(struct preprocessor_ctx * ctx, struct token_list * list, unsigned char new_line_is_space)
{
    struct token_list  r;
    struct token * current;
    unsigned char is_first;

    _cake_zmem(&r, 8);
    current = list->head;
    if (!new_line_is_space)
    {
        while (current && token_is_blank(current))
        {
            current = current->next;
        }
    }
    else
    {
        while (current && (token_is_blank(current) || current->type == 10))
        {
            current = current->next;
        }
    }
    is_first = 1;
    for (; current; )
    {
        struct token * token_added;

        if (!new_line_is_space)
        {
            if (current && token_is_blank(current))
            {
                if (current == list->tail)
                {
                    break;
                }
                current = current->next;
                continue;
            }
        }
        else
        {
            if (current && (token_is_blank(current) || current->type == 10))
            {
                if (current == list->tail)
                {
                    break;
                }
                current = current->next;
                continue;
            }
        }
        token_added = token_list_clone_and_add(&r, current);
        if (!ctx->options.preprocess_def_macro && token_added->type == 127)
        {
            token_added->type = 35;
            free(token_added->lexeme);
            token_added->lexeme = strdup("#");
        }
        if (token_added->flags & 8)
        {
            token_added->flags = token_added->flags & -9;
            token_added->flags |= 4;
        }
        if (is_first)
        {
            token_added->flags = token_added->flags & -5;
            token_added->flags = token_added->flags & -9;
            is_first = 0;
        }
        remove_line_continuation(token_added->lexeme);
        if (current == list->tail)
        {
            break;
        }
        current = current->next;
    }
    return r;
}


struct token_list copy_replacement_list(struct preprocessor_ctx * ctx, struct token_list * list)
{
    return copy_replacement_list_core(ctx, list, !ctx->options.preprocess_def_macro);
}


int stringify(char * input, int n, char output[]);

struct token_list macro_copy_replacement_list(struct preprocessor_ctx * ctx, struct macro * macro, struct token * origin)
{
    if (strcmp(macro->name, "__LINE__") == 0)
    {
        struct tokenizer_ctx  tctx;
        char line[50];
        struct token_list  r;

        _cake_zmem(&tctx, 696);
        _cake_zmem(&line, 50);
        ;
        snprintf(line, 50U, "%d", origin->line);
        r = tokenizer(&tctx, line, "", 0, 0);
        token_list_pop_front(&r);
        if (r.head != 0U)
        {
            r.head->flags = 0;
        }
        return r;
    }
    else
    {
        if (strcmp(macro->name, "__FILE__") == 0)
        {
            char buffer[300];
            struct tokenizer_ctx  tctx;
            struct token_list  r;

            _cake_zmem(&buffer, 300);
            if (stringify(origin->token_origin->lexeme, 300U, buffer) < 0)
            {
            }
            _cake_zmem(&tctx, 696);
            r = tokenizer(&tctx, buffer, "", 0, 0);
            token_list_pop_front(&r);
            if (r.head)
            {
                r.head->flags = 0;
            }
            return r;
        }
        else
        {
            if (strcmp(macro->name, "__COUNTER__") == 0)
            {
                char line[50];
                struct tokenizer_ctx  tctx;
                struct token_list  r;

                _cake_zmem(&line, 50);
                snprintf(line, 50U, "%d", ctx->count_macro_value);
                ctx->count_macro_value++;
                _cake_zmem(&tctx, 696);
                r = tokenizer(&tctx, line, "", 0, 0);
                token_list_pop_front(&r);
                if (r.head != 0U)
                {
                    r.head->flags = 0;
                }
                return r;
            }
        }
    }
    return copy_replacement_list(ctx, &macro->replacement_list);
}


static struct token_list replace_macro_arguments(struct preprocessor_ctx * ctx, struct macro_expanded * p_list, struct token_list * input_list, struct macro_argument_list * arguments, struct token * origin);
static unsigned char has_argument_list_empty_substitution(struct preprocessor_ctx * ctx, struct macro_expanded * p_list, struct macro_argument_list * p_macro_argument_list, struct token * origin);
void token_list_remove(struct token_list * list, struct token * first, struct token * last);
char *token_list_join_tokens(struct token_list * list, unsigned char bliteral);
char *print_preprocessed_to_string2(struct token * p_token);

struct token_list expand_macro(struct preprocessor_ctx * ctx, struct macro_expanded * p_list_of_macro_expanded_opt, struct macro * macro, struct macro_argument_list * arguments, int level, struct token * origin)
{
    struct token_list  r;

    macro->usage++;
    _cake_zmem(&r, 8);
    if (1) /*try*/
    {
        struct macro_expanded  macro_expanded;

        ;
        _cake_zmem(&macro_expanded, 8);
        macro_expanded.name = macro->name;
        macro_expanded.p_previous = p_list_of_macro_expanded_opt;
        if (macro->is_function)
        {
            struct token_list  copy;
            struct token_list  copy2;
            struct token_list  r2;

            copy = macro_copy_replacement_list(ctx, macro, origin);
            copy2 = replace_macro_arguments(ctx, &macro_expanded, &copy, arguments, origin);
            r2 = replacement_list_reexamination(ctx, &macro_expanded, &copy2, level, origin);
            token_list_append_list(&r, &r2);
            token_list_destroy(&copy);
            token_list_destroy(&copy2);
            token_list_destroy(&r2);
            if (ctx->n_errors > 0)
            {
                goto _CKL0;/*throw*/
            }
        }
        else
        {
            struct token_list  copy;
            struct token_list  r3;

            copy = macro_copy_replacement_list(ctx, macro, origin);
            r3 = replacement_list_reexamination(ctx, &macro_expanded, &copy, level, origin);
            if (ctx->n_errors > 0)
            {
                token_list_destroy(&copy);
                token_list_destroy(&r3);
                goto _CKL0;/*throw*/
            }
            token_list_append_list(&r, &r3);
            token_list_destroy(&copy);
            token_list_destroy(&r3);
        }
        if (ctx->n_errors > 0)
        {
            goto _CKL0;/*throw*/
        }
        if (ctx->options.preprocess_def_macro && macro->def_macro)
        {
            struct token_list  r0;
            struct token_list  list2;
            struct tokenizer_ctx  tctx;
            char * result;
            struct token_list  list3;

            _cake_zmem(&r0, 8);
            token_list_append_list(&r0, &r);
            list2 = preprocessor(ctx, &r0, level + 1);
            _cake_zmem(&tctx, 696);
            result = print_preprocessed_to_string2(list2.head);
            token_list_clear(&r);
            r = tokenizer(&tctx, result, "", 0, 2);
            list3 = copy_replacement_list_core(ctx, &r, 1);
            token_list_swap(&list3, &r);
            free((void *)result);
            token_list_destroy(&list2);
            token_list_destroy(&list3);
        }
    }
    else _CKL0: /*catch*/ 
    {
    }
    return r;
}



static unsigned char has_argument_list_empty_substitution(struct preprocessor_ctx * ctx, struct macro_expanded * p_list, struct macro_argument_list * p_macro_argument_list, struct token * origin)
{
    struct macro_argument * p_va_args_argument;

    if (p_macro_argument_list->head == 0U)
    {
        return 1;
    }
    p_va_args_argument = find_macro_argument_by_name(p_macro_argument_list, "__VA_ARGS__");
    if (p_va_args_argument)
    {
        struct token_list  argumentlist;
        struct token_list  r4;
        unsigned char results_in_empty_substituition;

        if (p_va_args_argument->tokens.head == 0U)
        {
            return 1;
        }
        argumentlist = copy_argument_list(p_va_args_argument);
        r4 = replacement_list_reexamination(ctx, p_list, &argumentlist, 0, origin);
        results_in_empty_substituition = (r4.head == 0U || r4.head->type == 142);
        token_list_destroy(&r4);
        token_list_destroy(&argumentlist);
        return results_in_empty_substituition;
    }
    return 0;
}

static struct token_list replace_macro_arguments(struct preprocessor_ctx * ctx, struct macro_expanded * p_list, struct token_list * input_list, struct macro_argument_list * arguments, struct token * origin)
{
    struct token_list  r;

    _cake_zmem(&r, 8);
    if (1) /*try*/
    {
        struct macro_argument * p;

        p = arguments->head;
        while (p)
        {
            struct macro_argument * next;

            next = p->next;
            if (p->macro_parameter)
            {
                p->macro_parameter->already_expanded = 0;
                token_list_clear(&p->macro_parameter->expanded_list);
            }
            p = next;
        }
        while (input_list->head)
        {
            struct macro_argument * p_argument;

            ;
            ;
            ;
            p_argument = 0U;
            if (input_list->head->type == 8996)
            {
                if (strcmp(input_list->head->lexeme, "__VA_OPT__") == 0)
                {
                    int parenteses_count;
                    unsigned char discard_va_opt;

                    token_list_pop_front(input_list);
                    token_list_pop_front(input_list);
                    parenteses_count = 1;
                    discard_va_opt = has_argument_list_empty_substitution(ctx, p_list, arguments, origin);
                    if (discard_va_opt)
                    {
                        while (input_list->head)
                        {
                            if (input_list->head->type == 40)
                            {
                                parenteses_count++;
                            }
                            else
                            {
                                if (input_list->head->type == 41)
                                {
                                    parenteses_count--;
                                }
                            }
                            token_list_pop_front(input_list);
                            if (parenteses_count == 0)
                            {
                                break;
                            }
                        }
                    }
                    else
                    {
                        struct token * p_token;

                        p_token = input_list->head;
                        for (; p_token; p_token = p_token->next)
                        {
                            if (p_token->type == 40)
                            {
                                parenteses_count++;
                            }
                            else
                            {
                                if (p_token->type == 41)
                                {
                                    parenteses_count--;
                                }
                            }
                            if (parenteses_count == 0)
                            {
                                break;
                            }
                        }
                        token_list_remove(input_list, p_token, p_token);
                    }
                    continue;
                }
                p_argument = find_macro_argument_by_name(arguments, input_list->head->lexeme);
            }
            if (p_argument)
            {
                if (r.tail != 0U && r.tail->type == 35)
                {
                    int flags;
                    struct token_list  argumentlist;
                    char * s;
                    struct token * p_new_token;

                    flags = r.tail->flags;
                    token_list_pop_front(input_list);
                    while (token_is_blank(r.tail))
                    {
                        token_list_pop_back(&r);
                    }
                    token_list_pop_back(&r);
                    argumentlist = copy_argument_list(p_argument);
                    s = token_list_join_tokens(&argumentlist, 1);
                    if (s == 0U)
                    {
                        token_list_destroy(&argumentlist);
                        preprocessor_diagnostic(650, ctx, input_list->head, "unexpected");
                        goto _CKL0;/*throw*/
                    }
                    p_new_token = calloc(1, 40U);
                    if (p_new_token == 0U)
                    {
                        free(s);
                        token_list_destroy(&argumentlist);
                        goto _CKL0;/*throw*/
                    }
                    p_new_token->lexeme = s;
                    p_new_token->type = 130;
                    p_new_token->flags = flags;
                    token_list_add(&r, p_new_token);
                    token_list_destroy(&argumentlist);
                    continue;
                }
                else
                {
                    if (r.tail != 0U && r.tail->type == 8995)
                    {
                        struct token_list  argumentlist;

                        token_list_pop_front(input_list);
                        argumentlist = copy_argument_list(p_argument);
                        token_list_append_list(&r, &argumentlist);
                        token_list_destroy(&argumentlist);
                    }
                    else
                    {
                        if (input_list->head->next && input_list->head->next->type == 8995)
                        {
                            int flags;
                            struct token_list  argumentlist;

                            flags = input_list->head->flags;
                            token_list_pop_front(input_list);
                            argumentlist = copy_argument_list(p_argument);
                            if (argumentlist.head != 0U)
                            {
                                argumentlist.head->flags = flags;
                            }
                            token_list_append_list(&r, &argumentlist);
                            prematch(&r, input_list);
                            token_list_destroy(&argumentlist);
                        }
                        else
                        {
                            int flags;
                            struct token_list  copy_list;

                            flags = input_list->head->flags;
                            token_list_pop_front(input_list);
                            if (p_argument->macro_parameter == 0U)
                            {
                                goto _CKL0;/*throw*/
                            }
                            if (!p_argument->macro_parameter->already_expanded)
                            {
                                struct token_list  copy_list;
                                struct token_list  r4;

                                copy_list = copy_argument_list(p_argument);
                                r4 = replacement_list_reexamination(ctx, p_list, &copy_list, 0, origin);
                                token_list_swap(&p_argument->macro_parameter->expanded_list, &r4);
                                token_list_destroy(&r4);
                                p_argument->macro_parameter->already_expanded = 1;
                            }
                            copy_list = copy_argument_list_tokens(&p_argument->macro_parameter->expanded_list);
                            if (copy_list.head)
                            {
                                copy_list.head->flags = flags;
                            }
                            token_list_append_list(&r, &copy_list);
                            token_list_destroy(&copy_list);
                            if (ctx->n_errors > 0)
                            {
                                goto _CKL0;/*throw*/
                            }
                        }
                    }
                }
            }
            else
            {
                prematch(&r, input_list);
            }
        }
    }
    else _CKL0: /*catch*/ 
    {
    }
    return r;
}
static struct token_list non_directive(struct preprocessor_ctx * ctx, struct token_list * input_list, int level);
static struct token_list text_line(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char is_active, int level);
void token_list_set_file(struct token_list * list, struct token * filetoken, int line, int col);

struct token_list group_part(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char is_active, int level)
{
    ;
    if (input_list->head->type == 127)
    {
        if (preprocessor_token_ahead_is_identifier(input_list->head, "if") || preprocessor_token_ahead_is_identifier(input_list->head, "ifdef") || preprocessor_token_ahead_is_identifier(input_list->head, "ifndef"))
        {
            return if_section(ctx, input_list, is_active, level);
        }
        else
        {
            if (preprocessor_token_ahead_is_identifier(input_list->head, "def"))
            {
                return def_section(ctx, input_list, is_active, level);
            }
            else
            {
                if (preprocessor_token_ahead_is_identifier(input_list->head, "include") || preprocessor_token_ahead_is_identifier(input_list->head, "include_next") || preprocessor_token_ahead_is_identifier(input_list->head, "embed") || preprocessor_token_ahead_is_identifier(input_list->head, "define") || preprocessor_token_ahead_is_identifier(input_list->head, "undef") || preprocessor_token_ahead_is_identifier(input_list->head, "warning") || preprocessor_token_ahead_is_identifier(input_list->head, "line") || preprocessor_token_ahead_is_identifier(input_list->head, "error") || preprocessor_token_ahead_is_identifier(input_list->head, "pragma") || preprocessor_token_ahead_is(input_list->head, 10))
                {
                    return control_line(ctx, input_list, is_active, level);
                }
                else
                {
                    if (is_active)
                    {
                        struct token * p_token;
                        char * directive_name;

                        p_token = preprocessor_look_ahead_core(input_list->head);
                        directive_name = p_token ? p_token->lexeme : "";
                        preprocessor_diagnostic(1830, ctx, input_list->head, "invalid preprocessor directive '#%s'\n", directive_name);
                    }
                    return non_directive(ctx, input_list, level);
                }
            }
        }
    }
    return text_line(ctx, input_list, is_active, level);
}



static struct token_list non_directive(struct preprocessor_ctx * ctx, struct token_list * input_list, int level)
{
    struct token_list  r;

    r = pp_tokens_opt(ctx, input_list, level);
    skip_blanks_level(ctx, &r, input_list, level);
    match_token_level(&r, input_list, 10, level, ctx);
    return r;
}

static struct token_list text_line(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char is_active, int level)
{
    struct token_list  r;

    _cake_zmem(&r, 8);
    if (1) /*try*/
    {
        while (input_list->head && input_list->head->type != 127)
        {
            struct macro * macro;
            struct token * start_token;
            struct token * origin;

            macro = 0U;
            start_token = input_list->head;
            origin = 0U;
            if (input_list->head->type == 8996 && strcmp(input_list->head->lexeme, "_Pragma") == 0)
            {
                struct token_list  r0;

                r0 = operator_pragma(ctx, input_list, is_active, level);
                token_list_append_list(&r, &r0);
                continue;
            }
            if (is_active && input_list->head->type == 8996)
            {
                origin = input_list->head;
                macro = find_macro(ctx, input_list->head->lexeme);
                if (macro && macro->is_function && !preprocessor_token_ahead_is(input_list->head, 40))
                {
                    macro = 0U;
                }
                if (ctx->conditional_inclusion)
                {
                    if (r.tail && r.tail->type == 8996 && strcmp(r.tail->lexeme, "defined") == 0)
                    {
                        macro = 0U;
                    }
                    else
                    {
                        if (r.tail && r.tail->type == 40)
                        {
                            struct token * previous;

                            previous = r.tail->prev;
                            if (previous != 0U && previous->type == 8996 && strcmp(previous->lexeme, "defined") == 0)
                            {
                                macro = 0U;
                            }
                        }
                    }
                }
            }
            if (macro)
            {
                int flags;
                struct macro_argument_list  arguments;
                struct token_list  start_macro;

                flags = input_list->head->flags;
                arguments = collect_macro_arguments(ctx, macro, input_list, level);
                if (ctx->n_errors > 0)
                {
                    macro_argument_list_destroy(&arguments);
                    goto _CKL0;/*throw*/
                }
                start_macro = expand_macro(ctx, 0U, macro, &arguments, level, origin);
                if (start_macro.head)
                {
                    start_macro.head->flags |= flags;
                }
                token_list_set_file(&start_macro, start_token->token_origin, start_token->line, start_token->col);
                token_list_append_list_at_beginning(input_list, &start_macro);
                if (ctx->flags & 1)
                {
                }
                else
                {
                    if (level == 0 || 1)
                    {
                        token_list_append_list(&r, &arguments.tokens);
                    }
                }
                while (macro)
                {
                    macro = 0U;
                    if (input_list->head && input_list->head->type == 8996)
                    {
                        macro = find_macro(ctx, input_list->head->lexeme);
                        if (macro && macro->is_function && !preprocessor_token_ahead_is(input_list->head, 40))
                        {
                            macro = 0U;
                        }
                        if (macro)
                        {
                            int flags2;
                            struct macro_argument_list  arguments2;
                            struct token_list  r3;

                            flags2 = input_list->head->flags;
                            arguments2 = collect_macro_arguments(ctx, macro, input_list, level);
                            if (ctx->n_errors > 0)
                            {
                                macro_argument_list_destroy(&arguments2);
                                macro_argument_list_destroy(&arguments);
                                token_list_destroy(&start_macro);
                                goto _CKL0;/*throw*/
                            }
                            if (ctx->flags & 1)
                            {
                            }
                            else
                            {
                                if (level == 0 || 1)
                                {
                                    token_list_append_list(&r, &arguments2.tokens);
                                }
                            }
                            r3 = expand_macro(ctx, 0U, macro, &arguments2, level, origin);
                            if (ctx->n_errors > 0)
                            {
                                macro_argument_list_destroy(&arguments2);
                                token_list_destroy(&r3);
                                macro_argument_list_destroy(&arguments);
                                token_list_destroy(&start_macro);
                                goto _CKL0;/*throw*/
                            }
                            token_list_set_file(&r3, start_token->token_origin, start_token->line, start_token->col);
                            if (r3.head)
                            {
                                r3.head->flags = flags2;
                            }
                            token_list_append_list_at_beginning(input_list, &r3);
                            macro_argument_list_destroy(&arguments2);
                            token_list_destroy(&r3);
                        }
                    }
                }
                macro_argument_list_destroy(&arguments);
                token_list_destroy(&start_macro);
                continue;
            }
            else
            {
                unsigned char blanks;
                unsigned char is_final;

                if (input_list->head->flags & 1024 && !(input_list->head->flags & 2))
                {
                    if (input_list->head->type == 130)
                    {
                        preprocessor_diagnostic(63, ctx, input_list->head, "you can use \"adjacent\" \"strings\"");
                    }
                    else
                    {
                        if (input_list->head->type == 132)
                        {
                            preprocessor_diagnostic(12, ctx, input_list->head, "multi-line //comment");
                        }
                        else
                        {
                            preprocessor_diagnostic(13, ctx, input_list->head, "unnecessary line-slicing");
                        }
                    }
                }
                blanks = token_is_blank(input_list->head) || input_list->head->type == 10;
                is_final = is_active && !is_never_final(input_list->head->type);
                if (ctx->flags & 1)
                {
                    if (is_final)
                    {
                        prematch(&r, input_list);
                        ;
                        r.tail->flags |= 1;
                    }
                    else
                    {
                        token_list_pop_front(input_list);
                    }
                }
                else
                {
                    if (blanks)
                    {
                        if (level == 0 || 1)
                        {
                            prematch(&r, input_list);
                        }
                        else
                        {
                            token_list_pop_front(input_list);
                        }
                    }
                    else
                    {
                        if (level == 0 || 1)
                        {
                            prematch(&r, input_list);
                            if (is_final)
                            {
                                ;
                                r.tail->flags |= 1;
                            }
                        }
                        else
                        {
                            if (is_final)
                            {
                                prematch(&r, input_list);
                                ;
                                r.tail->flags |= 1;
                            }
                            else
                            {
                                token_list_pop_front(input_list);
                            }
                        }
                    }
                }
            }
        }
    }
    else _CKL0: /*catch*/ 
    {
    }
    return r;
}
struct token_list preprocessor(struct preprocessor_ctx * ctx, struct token_list * input_list, int level)
{
    struct token_list  r;
    struct token_list  g;

    _cake_zmem(&r, 8);
    if (input_list->head == 0U)
    {
        return r;
    }
    if (input_list->head->type == 8998)
    {
        prematch_level(&r, input_list, 1);
    }
    g = group_opt(ctx, input_list, 1, level);
    token_list_append_list(&r, &g);
    token_list_destroy(&g);
    return r;
}


void check_unused_macros(struct hash_map * map)
{
    if (map->table != 0U)
    {
        {
            int i;

            i = 0;
            for (; i < map->capacity; i++)
            {
                struct map_entry * pentry;

                pentry = map->table[i];
                while (pentry != 0U)
                {
                    struct macro * macro;

                    ;
                    macro = pentry->data.p_macro;
                    if (macro->usage == 0)
                    {
                        printf("%s not used\n", macro->name);
                    }
                    pentry = pentry->next;
                }
            }
        }
    }
}


int get_self_path(char * buffer, int maxsize);
static void mark_macros_as_used(struct hash_map * map);

int include_config_header(struct preprocessor_ctx * ctx, char * file_name)
{
    char local_cakeconfig_path[260];
    char * str;
    int w;
    struct tokenizer_ctx  tctx;
    struct token_list  l;
    struct token_list  l10;

    _cake_zmem(&local_cakeconfig_path, 260);
    snprintf(local_cakeconfig_path, 260U, "%s", file_name);
    dirname(local_cakeconfig_path);
    snprintf(local_cakeconfig_path, 260U, "%s/cakeconfig.h", local_cakeconfig_path);
    str = read_file(local_cakeconfig_path, 1);
    if (str && ctx->options.show_includes)
    {
        printf(".%s\n", local_cakeconfig_path);
    }
    while (str == 0U)
    {
        dirname(local_cakeconfig_path);
        dirname(local_cakeconfig_path);
        if (local_cakeconfig_path[0] == 0)
        {
            break;
        }
        str = read_file(local_cakeconfig_path, 1);
        if (str && ctx->options.show_includes)
        {
            printf(".%s\n", local_cakeconfig_path);
        }
    }
    if (str == 0U)
    {
        char executable_path[246];
        char root_cakeconfig_path[260];

        _cake_zmem(&executable_path, 246);
        get_self_path(executable_path, 246U);
        dirname(executable_path);
        _cake_zmem(&root_cakeconfig_path, 260);
        snprintf(root_cakeconfig_path, 260U, "%s/cakeconfig.h", executable_path);
        str = read_file(root_cakeconfig_path, 1);
        if (str && ctx->options.show_includes)
        {
            printf(".%s\n", root_cakeconfig_path);
        }
    }
    if (str == 0U)
    {
        if (ctx->options.show_includes)
        {
            printf(".(cakeconfig.h not found)\n");
        }
        return 2;
    }
    w = ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings;
    _cake_zmem(&tctx, 696);
    l = tokenizer(&tctx, str, "standard macros inclusion", 0, 0);
    l10 = preprocessor(ctx, &l, 0);
    mark_macros_as_used(&ctx->macros);
    token_list_destroy(&l);
    free(str);
    token_list_destroy(&l10);
    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings = w;
    return 0;
}



static void mark_macros_as_used(struct hash_map * map)
{
    if (map->table != 0U)
    {
        {
            int i;

            i = 0;
            for (; i < map->capacity; i++)
            {
                struct map_entry * pentry;

                pentry = map->table[i];
                while (pentry != 0U)
                {
                    struct macro * macro;

                    ;
                    macro = pentry->data.p_macro;
                    macro->usage = 1;
                    pentry = pentry->next;
                }
            }
        }
    }
}
static long long __cdecl time(long long * _Time);
long long __cdecl _time64(long long * _Time);
static struct tm *__cdecl localtime(long long * _Time);
struct tm *__cdecl _localtime64(long long * _Time);
static char __Ck5_mon[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

extern char * TARGET_X86_X64_GCC_PREDEFINED_MACROS;
extern char * TARGET_X86_MSVC_PREDEFINED_MACROS;
extern char * TARGET_X64_MSVC_PREDEFINED_MACROS;
extern char * TARGET_CCU8_PREDEFINED_MACROS;
extern char * TARGET_CATALINA_PREDEFINED_MACROS;

void add_standard_macros(struct preprocessor_ctx * ctx, int target)
{
    struct diagnostic  w;
    struct diagnostic  __Ck4_compound_lit;
    long long now;
    struct tm * tm;
    struct tokenizer_ctx  tctx;
    char datastr[100];
    struct token_list  l1;
    struct token_list  tl1;
    char timestr[100];
    struct token_list  l2;
    struct token_list  tl2;
    char * pre_defined_macros_text;
    struct token_list  l;
    struct token_list  l10;

    w = ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index];
    __Ck4_compound_lit.errors = 0;
    __Ck4_compound_lit.warnings = 0;
    __Ck4_compound_lit.notes = 0;
    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index] = __Ck4_compound_lit;
    now = time(0U);
    tm = localtime(&now);
    _cake_zmem(&tctx, 696);
    _cake_zmem(&datastr, 100);
    snprintf(datastr, 100U, "#define __DATE__ \"%s %2d %d\"\n", __Ck5_mon[tm->tm_mon], tm->tm_mday, tm->tm_year + 1900);
    l1 = tokenizer(&tctx, datastr, "__DATE__ macro inclusion", 0, 0);
    tl1 = preprocessor(ctx, &l1, 0);
    token_list_destroy(&tl1);
    token_list_destroy(&l1);
    _cake_zmem(&timestr, 100);
    snprintf(timestr, 100U, "#define __TIME__ \"%02d:%02d:%02d\"\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
    l2 = tokenizer(&tctx, timestr, "__TIME__ macro inclusion", 0, 0);
    tl2 = preprocessor(ctx, &l2, 0);
    token_list_destroy(&tl2);
    token_list_destroy(&l2);
    pre_defined_macros_text = 0U;
    /*switch*/
    {
        register int __Ck6_temp = target;
        if (__Ck6_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck6_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck6_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck6_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck6_temp == 4) goto _CKL5; /*case 4*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            pre_defined_macros_text = TARGET_X86_X64_GCC_PREDEFINED_MACROS;
            goto _CKL0; /*break*/

            _CKL2: /*case 1*/ 
            pre_defined_macros_text = TARGET_X86_MSVC_PREDEFINED_MACROS;
            goto _CKL0; /*break*/

            _CKL3: /*case 2*/ 
            pre_defined_macros_text = TARGET_X64_MSVC_PREDEFINED_MACROS;
            goto _CKL0; /*break*/

            _CKL4: /*case 3*/ 
            pre_defined_macros_text = TARGET_CCU8_PREDEFINED_MACROS;
            goto _CKL0; /*break*/

            _CKL5: /*case 4*/ 
            pre_defined_macros_text = TARGET_CATALINA_PREDEFINED_MACROS;
            goto _CKL0; /*break*/

        }
        _CKL0:;
    }
    l = tokenizer(&tctx, pre_defined_macros_text, "standard macros inclusion", 0, 0);
    l10 = preprocessor(ctx, &l, 0);
    mark_macros_as_used(&ctx->macros);
    token_list_destroy(&l);
    token_list_destroy(&l10);
    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index] = w;
}



static long long __cdecl time(long long * _Time)
{
    return _time64(_Time);
}

static struct tm *__cdecl localtime(long long * _Time)
{
    return _localtime64(_Time);
}
char *get_token_name(int tk)
{
    /*switch*/
    {
        register int __Ck7_temp = tk;
        if (__Ck7_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck7_temp == 10) goto _CKL2; /*case 10*/
        if (__Ck7_temp == 32) goto _CKL3; /*case 32*/
        if (__Ck7_temp == 33) goto _CKL4; /*case 33*/
        if (__Ck7_temp == 34) goto _CKL5; /*case 34*/
        if (__Ck7_temp == 35) goto _CKL6; /*case 35*/
        if (__Ck7_temp == 36) goto _CKL7; /*case 36*/
        if (__Ck7_temp == 37) goto _CKL8; /*case 37*/
        if (__Ck7_temp == 38) goto _CKL9; /*case 38*/
        if (__Ck7_temp == 39) goto _CKL10; /*case 39*/
        if (__Ck7_temp == 40) goto _CKL11; /*case 40*/
        if (__Ck7_temp == 41) goto _CKL12; /*case 41*/
        if (__Ck7_temp == 42) goto _CKL13; /*case 42*/
        if (__Ck7_temp == 43) goto _CKL14; /*case 43*/
        if (__Ck7_temp == 44) goto _CKL15; /*case 44*/
        if (__Ck7_temp == 45) goto _CKL16; /*case 45*/
        if (__Ck7_temp == 46) goto _CKL17; /*case 46*/
        if (__Ck7_temp == 47) goto _CKL18; /*case 47*/
        if (__Ck7_temp == 58) goto _CKL19; /*case 58*/
        if (__Ck7_temp == 59) goto _CKL20; /*case 59*/
        if (__Ck7_temp == 60) goto _CKL21; /*case 60*/
        if (__Ck7_temp == 61) goto _CKL22; /*case 61*/
        if (__Ck7_temp == 62) goto _CKL23; /*case 62*/
        if (__Ck7_temp == 63) goto _CKL24; /*case 63*/
        if (__Ck7_temp == 64) goto _CKL25; /*case 64*/
        if (__Ck7_temp == 91) goto _CKL26; /*case 91*/
        if (__Ck7_temp == 12079) goto _CKL27; /*case 12079*/
        if (__Ck7_temp == 93) goto _CKL28; /*case 93*/
        if (__Ck7_temp == 94) goto _CKL29; /*case 94*/
        if (__Ck7_temp == 95) goto _CKL30; /*case 95*/
        if (__Ck7_temp == 96) goto _CKL31; /*case 96*/
        if (__Ck7_temp == 123) goto _CKL32; /*case 123*/
        if (__Ck7_temp == 124) goto _CKL33; /*case 124*/
        if (__Ck7_temp == 125) goto _CKL34; /*case 125*/
        if (__Ck7_temp == 126) goto _CKL35; /*case 126*/
        if (__Ck7_temp == 127) goto _CKL36; /*case 127*/
        if (__Ck7_temp == 128) goto _CKL37; /*case 128*/
        if (__Ck7_temp == 130) goto _CKL38; /*case 130*/
        if (__Ck7_temp == 131) goto _CKL39; /*case 131*/
        if (__Ck7_temp == 132) goto _CKL40; /*case 132*/
        if (__Ck7_temp == 133) goto _CKL41; /*case 133*/
        if (__Ck7_temp == 134) goto _CKL42; /*case 134*/
        if (__Ck7_temp == 9047) goto _CKL43; /*case 9047*/
        if (__Ck7_temp == 9048) goto _CKL44; /*case 9048*/
        if (__Ck7_temp == 9056) goto _CKL45; /*case 9056*/
        if (__Ck7_temp == 9057) goto _CKL46; /*case 9057*/
        if (__Ck7_temp == 135) goto _CKL47; /*case 135*/
        if (__Ck7_temp == 136) goto _CKL48; /*case 136*/
        if (__Ck7_temp == 137) goto _CKL49; /*case 137*/
        if (__Ck7_temp == 138) goto _CKL50; /*case 138*/
        if (__Ck7_temp == 139) goto _CKL51; /*case 139*/
        if (__Ck7_temp == 140) goto _CKL52; /*case 140*/
        if (__Ck7_temp == 141) goto _CKL53; /*case 141*/
        if (__Ck7_temp == 142) goto _CKL54; /*case 142*/
        if (__Ck7_temp == 143) goto _CKL55; /*case 143*/
        if (__Ck7_temp == 11051) goto _CKL56; /*case 11051*/
        if (__Ck7_temp == 11565) goto _CKL57; /*case 11565*/
        if (__Ck7_temp == 11582) goto _CKL58; /*case 11582*/
        if (__Ck7_temp == 15420) goto _CKL59; /*case 15420*/
        if (__Ck7_temp == 15934) goto _CKL60; /*case 15934*/
        if (__Ck7_temp == 31868) goto _CKL61; /*case 31868*/
        if (__Ck7_temp == 9766) goto _CKL62; /*case 9766*/
        if (__Ck7_temp == 8995) goto _CKL63; /*case 8995*/
        if (__Ck7_temp == 8996) goto _CKL64; /*case 8996*/
        if (__Ck7_temp == 8997) goto _CKL65; /*case 8997*/
        if (__Ck7_temp == 8998) goto _CKL66; /*case 8998*/
        if (__Ck7_temp == 8999) goto _CKL67; /*case 8999*/
        if (__Ck7_temp == 9001) goto _CKL68; /*case 9001*/
        if (__Ck7_temp == 9002) goto _CKL69; /*case 9002*/
        if (__Ck7_temp == 9003) goto _CKL70; /*case 9003*/
        if (__Ck7_temp == 9004) goto _CKL71; /*case 9004*/
        if (__Ck7_temp == 9005) goto _CKL72; /*case 9005*/
        if (__Ck7_temp == 9006) goto _CKL73; /*case 9006*/
        if (__Ck7_temp == 9007) goto _CKL74; /*case 9007*/
        if (__Ck7_temp == 9008) goto _CKL75; /*case 9008*/
        if (__Ck7_temp == 9009) goto _CKL76; /*case 9009*/
        if (__Ck7_temp == 9010) goto _CKL77; /*case 9010*/
        if (__Ck7_temp == 9011) goto _CKL78; /*case 9011*/
        if (__Ck7_temp == 9012) goto _CKL79; /*case 9012*/
        if (__Ck7_temp == 9013) goto _CKL80; /*case 9013*/
        if (__Ck7_temp == 9014) goto _CKL81; /*case 9014*/
        if (__Ck7_temp == 9015) goto _CKL82; /*case 9015*/
        if (__Ck7_temp == 9016) goto _CKL83; /*case 9016*/
        if (__Ck7_temp == 9017) goto _CKL84; /*case 9017*/
        if (__Ck7_temp == 9018) goto _CKL85; /*case 9018*/
        if (__Ck7_temp == 9019) goto _CKL86; /*case 9019*/
        if (__Ck7_temp == 9020) goto _CKL87; /*case 9020*/
        if (__Ck7_temp == 9021) goto _CKL88; /*case 9021*/
        if (__Ck7_temp == 9022) goto _CKL89; /*case 9022*/
        if (__Ck7_temp == 9023) goto _CKL90; /*case 9023*/
        if (__Ck7_temp == 9024) goto _CKL91; /*case 9024*/
        if (__Ck7_temp == 9025) goto _CKL92; /*case 9025*/
        if (__Ck7_temp == 9026) goto _CKL93; /*case 9026*/
        if (__Ck7_temp == 9027) goto _CKL94; /*case 9027*/
        if (__Ck7_temp == 9028) goto _CKL95; /*case 9028*/
        if (__Ck7_temp == 9029) goto _CKL96; /*case 9029*/
        if (__Ck7_temp == 9030) goto _CKL97; /*case 9030*/
        if (__Ck7_temp == 9031) goto _CKL98; /*case 9031*/
        if (__Ck7_temp == 9033) goto _CKL99; /*case 9033*/
        if (__Ck7_temp == 9034) goto _CKL100; /*case 9034*/
        if (__Ck7_temp == 9035) goto _CKL101; /*case 9035*/
        if (__Ck7_temp == 9036) goto _CKL102; /*case 9036*/
        if (__Ck7_temp == 9037) goto _CKL103; /*case 9037*/
        if (__Ck7_temp == 9038) goto _CKL104; /*case 9038*/
        if (__Ck7_temp == 9039) goto _CKL105; /*case 9039*/
        if (__Ck7_temp == 9040) goto _CKL106; /*case 9040*/
        if (__Ck7_temp == 9041) goto _CKL107; /*case 9041*/
        if (__Ck7_temp == 9042) goto _CKL108; /*case 9042*/
        if (__Ck7_temp == 9043) goto _CKL109; /*case 9043*/
        if (__Ck7_temp == 9044) goto _CKL110; /*case 9044*/
        if (__Ck7_temp == 9045) goto _CKL111; /*case 9045*/
        if (__Ck7_temp == 9046) goto _CKL112; /*case 9046*/
        if (__Ck7_temp == 9058) goto _CKL113; /*case 9058*/
        if (__Ck7_temp == 9059) goto _CKL114; /*case 9059*/
        if (__Ck7_temp == 9060) goto _CKL115; /*case 9060*/
        if (__Ck7_temp == 9061) goto _CKL116; /*case 9061*/
        if (__Ck7_temp == 9062) goto _CKL117; /*case 9062*/
        if (__Ck7_temp == 9063) goto _CKL118; /*case 9063*/
        if (__Ck7_temp == 9064) goto _CKL119; /*case 9064*/
        if (__Ck7_temp == 9065) goto _CKL120; /*case 9065*/
        if (__Ck7_temp == 9066) goto _CKL121; /*case 9066*/
        if (__Ck7_temp == 9067) goto _CKL122; /*case 9067*/
        if (__Ck7_temp == 9068) goto _CKL123; /*case 9068*/
        if (__Ck7_temp == 9069) goto _CKL124; /*case 9069*/
        if (__Ck7_temp == 9070) goto _CKL125; /*case 9070*/
        if (__Ck7_temp == 9071) goto _CKL126; /*case 9071*/
        if (__Ck7_temp == 9072) goto _CKL127; /*case 9072*/
        if (__Ck7_temp == 9073) goto _CKL128; /*case 9073*/
        if (__Ck7_temp == 9074) goto _CKL129; /*case 9074*/
        if (__Ck7_temp == 9075) goto _CKL130; /*case 9075*/
        if (__Ck7_temp == 9076) goto _CKL131; /*case 9076*/
        if (__Ck7_temp == 9077) goto _CKL132; /*case 9077*/
        if (__Ck7_temp == 9078) goto _CKL133; /*case 9078*/
        if (__Ck7_temp == 9079) goto _CKL134; /*case 9079*/
        if (__Ck7_temp == 9080) goto _CKL135; /*case 9080*/
        if (__Ck7_temp == 9081) goto _CKL136; /*case 9081*/
        if (__Ck7_temp == 9082) goto _CKL137; /*case 9082*/
        if (__Ck7_temp == 9083) goto _CKL138; /*case 9083*/
        if (__Ck7_temp == 9084) goto _CKL139; /*case 9084*/
        if (__Ck7_temp == 9085) goto _CKL140; /*case 9085*/
        if (__Ck7_temp == 9086) goto _CKL141; /*case 9086*/
        if (__Ck7_temp == 9087) goto _CKL142; /*case 9087*/
        if (__Ck7_temp == 9088) goto _CKL143; /*case 9088*/
        if (__Ck7_temp == 9089) goto _CKL144; /*case 9089*/
        if (__Ck7_temp == 9090) goto _CKL145; /*case 9090*/
        if (__Ck7_temp == 9091) goto _CKL146; /*case 9091*/
        if (__Ck7_temp == 9092) goto _CKL147; /*case 9092*/
        if (__Ck7_temp == 9093) goto _CKL148; /*case 9093*/
        if (__Ck7_temp == 9094) goto _CKL149; /*case 9094*/
        if (__Ck7_temp == 9095) goto _CKL150; /*case 9095*/
        if (__Ck7_temp == 9096) goto _CKL151; /*case 9096*/
        if (__Ck7_temp == 9097) goto _CKL152; /*case 9097*/
        if (__Ck7_temp == 9098) goto _CKL153; /*case 9098*/
        if (__Ck7_temp == 129) goto _CKL154; /*case 129*/
        if (__Ck7_temp == 9032) goto _CKL155; /*case 9032*/
        if (__Ck7_temp == 11069) goto _CKL156; /*case 11069*/
        if (__Ck7_temp == 11581) goto _CKL157; /*case 11581*/
        if (__Ck7_temp == 10813) goto _CKL158; /*case 10813*/
        if (__Ck7_temp == 12093) goto _CKL159; /*case 12093*/
        if (__Ck7_temp == 9533) goto _CKL160; /*case 9533*/
        if (__Ck7_temp == 3947581) goto _CKL161; /*case 3947581*/
        if (__Ck7_temp == 4079165) goto _CKL162; /*case 4079165*/
        if (__Ck7_temp == 9789) goto _CKL163; /*case 9789*/
        if (__Ck7_temp == 31805) goto _CKL164; /*case 31805*/
        if (__Ck7_temp == 24125) goto _CKL165; /*case 24125*/
        if (__Ck7_temp == 9049) goto _CKL166; /*case 9049*/
        if (__Ck7_temp == 9050) goto _CKL167; /*case 9050*/
        if (__Ck7_temp == 9051) goto _CKL168; /*case 9051*/
        if (__Ck7_temp == 9052) goto _CKL169; /*case 9052*/
        if (__Ck7_temp == 9053) goto _CKL170; /*case 9053*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            return "TK_NONE";
            _CKL2: /*case 10*/ 
            return "TK_NEWLINE";
            _CKL3: /*case 32*/ 
            return "TK_WHITE_SPACE";
            _CKL4: /*case 33*/ 
            return "TK_EXCLAMATION_MARK";
            _CKL5: /*case 34*/ 
            return "TK_QUOTATION_MARK";
            _CKL6: /*case 35*/ 
            return "TK_NUMBER_SIGN";
            _CKL7: /*case 36*/ 
            return "TK_DOLLAR_SIGN";
            _CKL8: /*case 37*/ 
            return "TK_PERCENT_SIGN";
            _CKL9: /*case 38*/ 
            return "TK_AMPERSAND";
            _CKL10: /*case 39*/ 
            return "TK_APOSTROPHE";
            _CKL11: /*case 40*/ 
            return "TK_LEFT_PARENTHESIS";
            _CKL12: /*case 41*/ 
            return "TK_RIGHT_PARENTHESIS";
            _CKL13: /*case 42*/ 
            return "TK_ASTERISK";
            _CKL14: /*case 43*/ 
            return "TK_PLUS_SIGN";
            _CKL15: /*case 44*/ 
            return "TK_COMMA";
            _CKL16: /*case 45*/ 
            return "TK_HYPHEN_MINUS";
            _CKL17: /*case 46*/ 
            return "TK_FULL_STOP";
            _CKL18: /*case 47*/ 
            return "TK_SOLIDUS";
            _CKL19: /*case 58*/ 
            return "TK_COLON";
            _CKL20: /*case 59*/ 
            return "TK_SEMICOLON";
            _CKL21: /*case 60*/ 
            return "TK_LESS_THAN_SIGN";
            _CKL22: /*case 61*/ 
            return "TK_EQUALS_SIGN";
            _CKL23: /*case 62*/ 
            return "TK_GREATER_THAN_SIGN";
            _CKL24: /*case 63*/ 
            return "TK_QUESTION_MARK";
            _CKL25: /*case 64*/ 
            return "TK_COMMERCIAL_AT";
            _CKL26: /*case 91*/ 
            return "TK_LEFT_SQUARE_BRACKET";
            _CKL27: /*case 12079*/ 
            return "TK_REVERSE_SOLIDUS";
            _CKL28: /*case 93*/ 
            return "TK_RIGHT_SQUARE_BRACKET";
            _CKL29: /*case 94*/ 
            return "TK_CIRCUMFLEX_ACCENT";
            _CKL30: /*case 95*/ 
            return "TK_FLOW_LINE";
            _CKL31: /*case 96*/ 
            return "TK_GRAVE_ACCENT";
            _CKL32: /*case 123*/ 
            return "TK_LEFT_CURLY_BRACKET";
            _CKL33: /*case 124*/ 
            return "TK_VERTICAL_LINE";
            _CKL34: /*case 125*/ 
            return "TK_RIGHT_CURLY_BRACKET";
            _CKL35: /*case 126*/ 
            return "TK_TILDE";
            _CKL36: /*case 127*/ 
            return "TK_PREPROCESSOR_LINE";
            _CKL37: /*case 128*/ 
            return "TK_PRAGMA";
            _CKL38: /*case 130*/ 
            return "TK_STRING_LITERAL";
            _CKL39: /*case 131*/ 
            return "TK_CHAR_CONSTANT";
            _CKL40: /*case 132*/ 
            return "TK_LINE_COMMENT";
            _CKL41: /*case 133*/ 
            return "TK_COMMENT";
            _CKL42: /*case 134*/ 
            return "TK_PPNUMBER";
            _CKL43: /*case 9047*/ 
            return "TK_KEYWORD_GCC__ATTRIBUTE";
            _CKL44: /*case 9048*/ 
            return "TK_KEYWORD_GCC__BUILTIN_VA_LIST";
            _CKL45: /*case 9056*/ 
            return "TK_KEYWORD_MSVC__PTR32";
            _CKL46: /*case 9057*/ 
            return "TK_KEYWORD_MSVC__PTR64";
            _CKL47: /*case 135*/ 
            return "ANY_OTHER_PP_TOKEN";
            _CKL48: /*case 136*/ 
            return "TK_COMPILER_DECIMAL_CONSTANT";
            _CKL49: /*case 137*/ 
            return "TK_COMPILER_OCTAL_CONSTANT";
            _CKL50: /*case 138*/ 
            return "TK_COMPILER_HEXADECIMAL_CONSTANT";
            _CKL51: /*case 139*/ 
            return "TK_COMPILER_BINARY_CONSTANT";
            _CKL52: /*case 140*/ 
            return "TK_COMPILER_DECIMAL_FLOATING_CONSTANT";
            _CKL53: /*case 141*/ 
            return "TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT";
            _CKL54: /*case 142*/ 
            return "TK_PLACEMARKER";
            _CKL55: /*case 143*/ 
            return "TK_BLANKS";
            _CKL56: /*case 11051*/ 
            return "TK_PLUSPLUS";
            _CKL57: /*case 11565*/ 
            return "TK_MINUSMINUS";
            _CKL58: /*case 11582*/ 
            return "TK_ARROW";
            _CKL59: /*case 15420*/ 
            return "TK_SHIFTLEFT";
            _CKL60: /*case 15934*/ 
            return "TK_SHIFTRIGHT";
            _CKL61: /*case 31868*/ 
            return "TK_LOGICAL_OPERATOR_OR";
            _CKL62: /*case 9766*/ 
            return "TK_LOGICAL_OPERATOR_AND";
            _CKL63: /*case 8995*/ 
            return "TK_MACRO_CONCATENATE_OPERATOR";
            _CKL64: /*case 8996*/ 
            return "TK_IDENTIFIER";
            _CKL65: /*case 8997*/ 
            return "TK_IDENTIFIER_RECURSIVE_MACRO";
            _CKL66: /*case 8998*/ 
            return "TK_BEGIN_OF_FILE";
            _CKL67: /*case 8999*/ 
            return "TK_KEYWORD_AUTO";
            _CKL68: /*case 9001*/ 
            return "TK_KEYWORD_BREAK";
            _CKL69: /*case 9002*/ 
            return "TK_KEYWORD_CASE";
            _CKL70: /*case 9003*/ 
            return "TK_KEYWORD_CONSTEXPR";
            _CKL71: /*case 9004*/ 
            return "TK_KEYWORD_CHAR";
            _CKL72: /*case 9005*/ 
            return "TK_KEYWORD_CONST";
            _CKL73: /*case 9006*/ 
            return "TK_KEYWORD_CONTINUE";
            _CKL74: /*case 9007*/ 
            return "TK_KEYWORD_CAKE_CATCH";
            _CKL75: /*case 9008*/ 
            return "TK_KEYWORD_DEFAULT";
            _CKL76: /*case 9009*/ 
            return "TK_KEYWORD_DO";
            _CKL77: /*case 9010*/ 
            return "TK_KEYWORD_DEFER";
            _CKL78: /*case 9011*/ 
            return "TK_KEYWORD_DOUBLE";
            _CKL79: /*case 9012*/ 
            return "TK_KEYWORD_ELSE";
            _CKL80: /*case 9013*/ 
            return "TK_KEYWORD_ENUM";
            _CKL81: /*case 9014*/ 
            return "TK_KEYWORD_EXTERN";
            _CKL82: /*case 9015*/ 
            return "TK_KEYWORD_FLOAT";
            _CKL83: /*case 9016*/ 
            return "TK_KEYWORD_FOR";
            _CKL84: /*case 9017*/ 
            return "TK_KEYWORD_GOTO";
            _CKL85: /*case 9018*/ 
            return "TK_KEYWORD_IF";
            _CKL86: /*case 9019*/ 
            return "TK_KEYWORD_INLINE";
            _CKL87: /*case 9020*/ 
            return "TK_KEYWORD_INT";
            _CKL88: /*case 9021*/ 
            return "TK_KEYWORD_LONG";
            _CKL89: /*case 9022*/ 
            return "TK_KEYWORD_MSVC__INT8";
            _CKL90: /*case 9023*/ 
            return "TK_KEYWORD_MSVC__INT16";
            _CKL91: /*case 9024*/ 
            return "TK_KEYWORD_MSVC__INT32";
            _CKL92: /*case 9025*/ 
            return "TK_KEYWORD_MSVC__INT64";
            _CKL93: /*case 9026*/ 
            return "TK_KEYWORD_REGISTER";
            _CKL94: /*case 9027*/ 
            return "TK_KEYWORD_RESTRICT";
            _CKL95: /*case 9028*/ 
            return "TK_KEYWORD_RETURN";
            _CKL96: /*case 9029*/ 
            return "TK_KEYWORD_SHORT";
            _CKL97: /*case 9030*/ 
            return "TK_KEYWORD_SIGNED";
            _CKL98: /*case 9031*/ 
            return "TK_KEYWORD_SIZEOF";
            _CKL99: /*case 9033*/ 
            return "TK_KEYWORD_STATIC";
            _CKL100: /*case 9034*/ 
            return "TK_KEYWORD_STRUCT";
            _CKL101: /*case 9035*/ 
            return "TK_KEYWORD_SWITCH";
            _CKL102: /*case 9036*/ 
            return "TK_KEYWORD_TYPEDEF";
            _CKL103: /*case 9037*/ 
            return "TK_KEYWORD_CAKE_TRY";
            _CKL104: /*case 9038*/ 
            return "TK_KEYWORD_CAKE_THROW";
            _CKL105: /*case 9039*/ 
            return "TK_KEYWORD_UNION";
            _CKL106: /*case 9040*/ 
            return "TK_KEYWORD_UNSIGNED";
            _CKL107: /*case 9041*/ 
            return "TK_KEYWORD_VOID";
            _CKL108: /*case 9042*/ 
            return "TK_KEYWORD_VOLATILE";
            _CKL109: /*case 9043*/ 
            return "TK_KEYWORD_WHILE";
            _CKL110: /*case 9044*/ 
            return "TK_KEYWORD__ALIGNAS";
            _CKL111: /*case 9045*/ 
            return "TK_KEYWORD__ALIGNOF";
            _CKL112: /*case 9046*/ 
            return "TK_KEYWORD__ATOMIC";
            _CKL113: /*case 9058*/ 
            return "TK_KEYWORD_MSVC__FASTCALL";
            _CKL114: /*case 9059*/ 
            return "TK_KEYWORD_MSVC__STDCALL";
            _CKL115: /*case 9060*/ 
            return "TK_KEYWORD_MSVC__CDECL";
            _CKL116: /*case 9061*/ 
            return "TK_KEYWORD_MSVC__DECLSPEC";
            _CKL117: /*case 9062*/ 
            return "TK_KEYWORD_MSVC__ASM";
            _CKL118: /*case 9063*/ 
            return "TK_KEYWORD__BOOL";
            _CKL119: /*case 9064*/ 
            return "TK_KEYWORD__COMPLEX";
            _CKL120: /*case 9065*/ 
            return "TK_KEYWORD__DECIMAL128";
            _CKL121: /*case 9066*/ 
            return "TK_KEYWORD__DECIMAL32";
            _CKL122: /*case 9067*/ 
            return "TK_KEYWORD__DECIMAL64";
            _CKL123: /*case 9068*/ 
            return "TK_KEYWORD__GENERIC";
            _CKL124: /*case 9069*/ 
            return "TK_KEYWORD__IMAGINARY";
            _CKL125: /*case 9070*/ 
            return "TK_KEYWORD__NORETURN";
            _CKL126: /*case 9071*/ 
            return "TK_KEYWORD__STATIC_ASSERT";
            _CKL127: /*case 9072*/ 
            return "TK_KEYWORD_ASSERT";
            _CKL128: /*case 9073*/ 
            return "TK_KEYWORD__THREAD_LOCAL";
            _CKL129: /*case 9074*/ 
            return "TK_KEYWORD_TYPEOF";
            _CKL130: /*case 9075*/ 
            return "TK_KEYWORD_TRUE";
            _CKL131: /*case 9076*/ 
            return "TK_KEYWORD_FALSE";
            _CKL132: /*case 9077*/ 
            return "TK_KEYWORD_NULLPTR";
            _CKL133: /*case 9078*/ 
            return "TK_KEYWORD_TYPEOF_UNQUAL";
            _CKL134: /*case 9079*/ 
            return "TK_KEYWORD__BITINT";
            _CKL135: /*case 9080*/ 
            return "TK_KEYWORD_CAKE_OWNER";
            _CKL136: /*case 9081*/ 
            return "TK_KEYWORD__OUT";
            _CKL137: /*case 9082*/ 
            return "TK_KEYWORD__OBJ_OWNER";
            _CKL138: /*case 9083*/ 
            return "TK_KEYWORD_CAKE_VIEW";
            _CKL139: /*case 9084*/ 
            return "TK_KEYWORD_CAKE_OPT";
            _CKL140: /*case 9085*/ 
            return "TK_KEYWORD_CAKE_STATIC_DEBUG";
            _CKL141: /*case 9086*/ 
            return "TK_KEYWORD_CAKE_STATIC_DEBUG_EX";
            _CKL142: /*case 9087*/ 
            return "TK_KEYWORD_STATIC_STATE";
            _CKL143: /*case 9088*/ 
            return "TK_KEYWORD_STATIC_SET";
            _CKL144: /*case 9089*/ 
            return "TK_KEYWORD_IS_POINTER";
            _CKL145: /*case 9090*/ 
            return "TK_KEYWORD_IS_LVALUE";
            _CKL146: /*case 9091*/ 
            return "TK_KEYWORD_IS_CONST";
            _CKL147: /*case 9092*/ 
            return "TK_KEYWORD_IS_OWNER";
            _CKL148: /*case 9093*/ 
            return "TK_KEYWORD_IS_ARRAY";
            _CKL149: /*case 9094*/ 
            return "TK_KEYWORD_IS_FUNCTION";
            _CKL150: /*case 9095*/ 
            return "TK_KEYWORD_IS_SCALAR";
            _CKL151: /*case 9096*/ 
            return "TK_KEYWORD_IS_ARITHMETIC";
            _CKL152: /*case 9097*/ 
            return "TK_KEYWORD_IS_FLOATING_POINT";
            _CKL153: /*case 9098*/ 
            return "TK_KEYWORD_IS_INTEGRAL";
            _CKL154: /*case 129*/ 
            return "TK_PRAGMA_END";
            _CKL155: /*case 9032*/ 
            return "TK_KEYWORD__COUNTOF";
            _CKL156: /*case 11069*/ 
            return "TK_PLUS_ASSIGN";
            _CKL157: /*case 11581*/ 
            return "TK_MINUS_ASSIGN";
            _CKL158: /*case 10813*/ 
            return "TK_MULTI_ASSIGN";
            _CKL159: /*case 12093*/ 
            return "TK_DIV_ASSIGN";
            _CKL160: /*case 9533*/ 
            return "TK_MOD_ASSIGN";
            _CKL161: /*case 3947581*/ 
            return "TK_SHIFT_LEFT_ASSIGN";
            _CKL162: /*case 4079165*/ 
            return "TK_SHIFT_RIGHT_ASSIGN";
            _CKL163: /*case 9789*/ 
            return "TK_AND_ASSIGN";
            _CKL164: /*case 31805*/ 
            return "TK_OR_ASSIGN";
            _CKL165: /*case 24125*/ 
            return "TK_NOT_ASSIGN";
            _CKL166: /*case 9049*/ 
            return "TK_KEYWORD_GCC__BUILTIN_VA_END";
            _CKL167: /*case 9050*/ 
            return "TK_KEYWORD_GCC__BUILTIN_VA_ARG";
            _CKL168: /*case 9051*/ 
            return "TK_KEYWORD_GCC__BUILTIN_C23_VA_START";
            _CKL169: /*case 9052*/ 
            return "TK_KEYWORD_GCC__BUILTIN_VA_COPY";
            _CKL170: /*case 9053*/ 
            return "TK_KEYWORD_GCC__BUILTIN_OFFSETOF";
        }
        _CKL0:;
    }
    return "TK_X_MISSING_NAME";
}


int stringify(char * input, int n, char output[])
{
    int count;
    char * p;

    count = 0;
    if (count < n)
    {
        output[count++] = 34;
    }
    p = input;
    while (*p)
    {
        if (*p == 34 || *p == 92)
        {
            if (count < n)
            {
                output[count++] = 92;
            }
            if (count < n)
            {
                output[count++] = *p;
            }
            p++;
        }
        else
        {
            if (count < n)
            {
                output[count++] = *p;
            }
            p++;
        }
    }
    if (count < n)
    {
        output[count++] = 34;
    }
    if (count < n)
    {
        output[count++] = 0;
    }
    if (count >= n)
    {
        return -count;
    }
    return count;
}


void print_literal(char * s)
{
    if (s == 0U)
    {
        printf("\"");
        printf("\"");
        return;
    }
    printf("\"");
    while (*s)
    {
        /*switch*/
        {
            register char __Ck8_temp = *s;
            if (__Ck8_temp == 10) goto _CKL2; /*case 10*/
            goto _CKL3;/*default*/

            {
                _CKL2: /*case 10*/ 
                printf("\\n");
                goto _CKL1; /*break*/

                _CKL3: /*default*/ 
                printf("%c", *s);
            }
            _CKL1:;
        }
        s++;
    }
    printf("\"");
}


char *get_code_as_we_see_plus_macros(struct token_list * list)
{
    struct osstream  ss;
    struct token * current;
    char * cstr;

    _cake_zmem(&ss, 12);
    current = list->head;
    while (current)
    {
        if (current->level == 0 && current->type != 8998)
        {
            if (current->flags & 2)
            {
                ss_fprintf(&ss, "\x1b[36;1m");
            }
            else
            {
                ss_fprintf(&ss, "\x1b[97m");
            }
            ss_fprintf(&ss, "%s", current->lexeme);
            ss_fprintf(&ss, "\x1b[0m");
        }
        current = current->next;
    }
    cstr = ss.c_str;
    ss.c_str = 0U;
    ss_close(&ss);
    return cstr;
}


void print_code_as_we_see(struct token_list * list, unsigned char remove_comments)
{
    struct token * current;

    if (list->head == 0U || list->tail == 0U)
    {
        return;
    }
    current = list->head;
    while (current && current != list->tail->next)
    {
        if (current->level == 0 && !(current->flags & 2) && !(current->flags & 64) && current->type != 8998)
        {
            if ((current->flags & 4) && (current->prev != 0U && current->prev->type != 143))
            {
                printf(" ");
            }
            if (remove_comments)
            {
                if (current->type == 132)
                {
                    printf("\n");
                }
                else
                {
                    if (current->type == 133)
                    {
                        printf(" ");
                    }
                    else
                    {
                        printf("%s", current->lexeme);
                    }
                }
            }
            else
            {
                printf("%s", current->lexeme);
            }
        }
        current = current->next;
    }
}


char *get_code_as_we_see(struct token_list * list, unsigned char remove_comments)
{
    struct osstream  ss;
    struct token * current;
    char * cstr;

    if (list->head == 0U || list->tail == 0U)
    {
        return 0U;
    }
    _cake_zmem(&ss, 12);
    current = list->head;
    while (current && current != list->tail->next)
    {
        if (current->level == 0 && !(current->flags & 2) && !(current->flags & 64) && current->type != 8998)
        {
            if ((current->flags & 4) && (current->prev != 0U && current->prev->type != 143))
            {
                ss_fprintf(&ss, " ");
            }
            if (remove_comments)
            {
                if (current->type == 132)
                {
                    ss_fprintf(&ss, "\n");
                }
                else
                {
                    if (current->type == 133)
                    {
                        ss_fprintf(&ss, " ");
                    }
                    else
                    {
                        ss_fprintf(&ss, "%s", current->lexeme);
                    }
                }
            }
            else
            {
                ss_fprintf(&ss, "%s", current->lexeme);
            }
        }
        current = current->next;
    }
    cstr = ss.c_str;
    ss.c_str = 0U;
    ss_close(&ss);
    return cstr;
}


char *get_code_as_compiler_see(struct token_list * list)
{
    struct osstream  ss;
    struct token * current;

    if (list->head == 0U || list->tail == 0U)
    {
        return 0U;
    }
    _cake_zmem(&ss, 12);
    current = list->head;
    while (current && current != list->tail->next)
    {
        if (!(current->flags & 64) && current->type != 8998 && (current->flags & 1))
        {
            if (current->flags & 4)
            {
                ss_fprintf(&ss, " ");
            }
            if (current->flags & 8)
            {
                ss_fprintf(&ss, "\n");
            }
            if (current->type == 132)
            {
                ss_fprintf(&ss, "\n");
            }
            else
            {
                if (current->type == 133)
                {
                    ss_fprintf(&ss, " ");
                }
                else
                {
                    ss_fprintf(&ss, "%s", current->lexeme);
                }
            }
        }
        current = current->next;
    }
    return ss.c_str;
}


char *print_preprocessed_to_string2(struct token * p_token)
{
    struct osstream  ss;
    struct token * current;

    if (p_token == 0U)
    {
        return strdup("(null)");
    }
    _cake_zmem(&ss, 12);
    current = p_token;
    while (current)
    {
        remove_line_continuation(current->lexeme);
        if (current->flags & 1)
        {
            if (current->level > 0)
            {
                if ((current->flags & 8))
                {
                    ss_fprintf(&ss, "\n");
                }
                else
                {
                    if ((current->flags & 4))
                    {
                        ss_fprintf(&ss, " ");
                    }
                }
            }
            else
            {
                if (current->flags & 2)
                {
                    if ((current->flags & 4))
                    {
                        ss_fprintf(&ss, " ");
                    }
                }
            }
            if (current->type == 128)
            {
                if (strcmp(current->lexeme, "_Pragma") == 0)
                {
                    ss_fprintf(&ss, "\n");
                }
                ss_fprintf(&ss, "#pragma ");
                current = current->next;
                while (current->type != 129)
                {
                    ss_fprintf(&ss, "%s", current->lexeme);
                    current = current->next;
                }
                ss_fprintf(&ss, "\n");
                current = current->next;
                continue;
            }
            if (current->lexeme[0] != 0)
            {
                ss_fprintf(&ss, "%s", current->lexeme);
            }
            current = current->next;
        }
        else
        {
            if (current->level == 0)
            {
                if (current->type == 143 || current->type == 10)
                {
                    ss_fprintf(&ss, "%s", current->lexeme);
                }
            }
            current = current->next;
        }
    }
    return ss.c_str;
}


char *print_preprocessed_to_string(struct token * p_token)
{
    struct osstream  ss;
    struct token * current;
    unsigned char first;

    _cake_zmem(&ss, 12);
    current = p_token;
    while (!(current->flags & 1) || current->type == 143 || current->type == 133 || current->type == 132 || current->type == 10 || current->type == 127)
    {
        current = current->next;
        if (current == 0U)
        {
            return ss.c_str;
        }
    }
    first = 1;
    while (current)
    {
        ;
        if (current->flags & 1)
        {
            if (!first && current->flags & 8)
            {
                ss_fprintf(&ss, "\n");
            }
            else
            {
                if (!first && current->flags & 4)
                {
                    ss_fprintf(&ss, " ");
                }
            }
            if (current->lexeme[0] != 0)
            {
                ss_fprintf(&ss, "%s", current->lexeme);
            }
            first = 0;
            current = current->next;
        }
        else
        {
            current = current->next;
        }
    }
    return ss.c_str;
}


void print_preprocessed(struct token * p_token)
{
    char * s;

    s = print_preprocessed_to_string(p_token);
    if (s)
    {
        printf("%s", s);
        free((void *)s);
    }
}


void print_all_macros(struct preprocessor_ctx * prectx)
{
    {
        int i;

        i = 0;
        for (; i < prectx->macros.capacity; i++)
        {
            struct map_entry * entry;
            struct macro * macro;
            struct token * token;

            entry = prectx->macros.table[i];
            if (entry == 0U)
            {
                continue;
            }
            ;
            macro = entry->data.p_macro;
            printf("#define %s", macro->name);
            if (macro->is_function)
            {
                struct macro_parameter * parameter;

                printf("(");
                parameter = macro->parameters;
                while (parameter)
                {
                    printf("%s", parameter->name);
                    if (parameter->next)
                    {
                        printf(",");
                    }
                    parameter = parameter->next;
                }
                printf(")");
            }
            printf(" ");
            token = macro->replacement_list.head;
            while (token)
            {
                printf("%s", token->lexeme);
                if (token == macro->replacement_list.tail)
                {
                    break;
                }
                token = token->next;
            }
            printf("\n");
        }
    }
}


static unsigned char is_screaming_case(char * text);

void naming_convention_macro(struct preprocessor_ctx * ctx, struct token * token)
{
    if (!is_screaming_case(token->lexeme))
    {
        preprocessor_diagnostic(63, ctx, token, "use SCREAMING_CASE for macros");
    }
}



static unsigned char is_screaming_case(char * text)
{
    unsigned char screaming_case;

    screaming_case = 0;
    while (*text)
    {
        if ((*text >= 65 && *text <= 90) || (*text >= 48 && *text <= 57) || (*text == 95))
        {
            screaming_case = 1;
        }
        else
        {
            return 0;
        }
        text++;
    }
    return screaming_case;
}
