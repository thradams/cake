struct diagnostic {
    unsigned long long errors;
    unsigned long long warnings;
    unsigned long long notes;
};

struct token_list {
    struct token * head;
    struct token * tail;
};

struct struct_entry;
struct include_dir {
    char * path;
    struct include_dir * next;
};

struct macro_parameter {
    char * name;
    struct macro_parameter * next;
};

struct struct_or_union_specifier;
struct struct_entry5;
struct enumerator;
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

struct hash_map {
    struct map_entry ** table;
    int capacity;
    int size;
};

struct include_dir_list {
    struct include_dir * head;
    struct include_dir * tail;
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

struct declarator3;
struct _iobuf {
    void * _Placeholder;
};

struct tokenizer_ctx {
    struct options  options;
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

struct osstream {
    char * c_str;
    int size;
    int capacity;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct enum_specifier0;
struct marker {
    char * file;
    int line;
    int start_col;
    int end_col;
    struct token * p_token_caret;
    struct token * p_token_begin;
    struct token * p_token_end;
};

struct macro_argument_list {
    struct token_list  tokens;
    struct macro_argument * head;
    struct macro_argument * tail;
};

struct stream {
    char * source;
    char * current;
    int line;
    int col;
    int line_continuation_count;
    char * path;
};

struct enum_specifier;
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

struct hash_item_set {
    unsigned int number;
    struct enum_specifier0 * p_enum_specifier;
    struct enumerator1 * p_enumerator;
    struct struct_or_union_specifier2 * p_struct_or_union_specifier;
    struct declarator3 * p_declarator;
    struct init_declarator4 * p_init_declarator;
    struct macro * p_macro;
    struct struct_entry5 * p_struct_entry;
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

struct macro_expanded {
    char * name;
    struct macro_expanded * p_previous;
};

struct init_declarator4;
struct enumerator1;
struct __crt_multibyte_data;
struct init_declarator;
struct declarator;
struct struct_or_union_specifier2;
struct __crt_locale_data;
struct macro {
    char * name;
    struct token_list  replacement_list;
    struct macro_parameter * parameters;
    unsigned char   is_function;
    int usage;
    unsigned char   expand;
};

struct macro_argument {
    char * name;
    struct token_list  tokens;
    struct macro_argument * next;
};


void free(void * ptr);

void include_dir_list_destroy(struct include_dir_list * list)
{
    struct include_dir * p = list->head;
    while (p)
    {
        struct include_dir * next = p->next;
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


int __stdio_common_vsprintf(unsigned __int64 _Options, char * _Buffer, unsigned int _BufferCount, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);

inline unsigned __int64 *__local_stdio_printf_options(void)
{
    static unsigned __int64 _OptionsStorage;
    return &_OptionsStorage;
}
inline int vsnprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, char * _ArgList)
{
    int _Result = __stdio_common_vsprintf(( *__local_stdio_printf_options()) | (1ULL << 1), _Buffer, _BufferCount, _Format, ((void *)0), _ArgList);
    return _Result < 0 ?  -1 : _Result;
}
void print_position(char * path, int line, int col, unsigned char   msvc_format);


int __stdio_common_vfprintf(unsigned __int64 _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
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

static void tokenizer_set_error(struct tokenizer_ctx * ctx, struct stream * stream, char * fmt, ...)
{
    ctx->n_errors++;
    char buffer[200] = {0};
    char * args = {0};
    ((void)(args = (char *)(&(fmt)) + ((sizeof (fmt) + sizeof (int) - 1) &  ~(sizeof (int) - 1))));
    vsnprintf(buffer, sizeof (buffer), fmt, args);
    ((void)(args = (char *)0));
    print_position(stream->path, stream->line, stream->col, ctx->options.visual_studio_ouput_format);
    if (ctx->options.visual_studio_ouput_format)
    {
        printf("error: ", buffer);
    }
    else
    {
        printf("\x1b[91m", buffer);
    }
}

static void tokenizer_set_warning(struct tokenizer_ctx * ctx, struct stream * stream, char * fmt, ...)
{
    ctx->n_warnings++;
    char buffer[200] = {0};
    char * args = {0};
    ((void)(args = (char *)(&(fmt)) + ((sizeof (fmt) + sizeof (int) - 1) &  ~(sizeof (int) - 1))));
    vsnprintf(buffer, sizeof (buffer), fmt, args);
    ((void)(args = (char *)0));
    print_position(stream->path, stream->line, stream->col, ctx->options.visual_studio_ouput_format);
    if (ctx->options.visual_studio_ouput_format)
    {
        printf("warning: ", buffer);
    }
    else
    {
        printf("\x1b[95m", buffer);
    }
}

unsigned char  preprocessor_diagnostic_message(int   w, struct preprocessor_ctx * ctx, struct token * p_token, char * fmt, ...);

void pre_unexpected_end_of_file(struct token * p_token, struct preprocessor_ctx * ctx)
{
    preprocessor_diagnostic_message(970, ctx, p_token, "unexpected end of file");
}

void print_line_and_token(struct marker * p_marker, unsigned char   visual_studio_ouput_format);

unsigned char  preprocessor_diagnostic_message(int   w, struct preprocessor_ctx * ctx, struct token * p_token_opt, char * fmt, ...)
{
    struct marker  marker = {0};
    if (p_token_opt == ((void *)0))
    {
        return 0;
    }
    marker.file = p_token_opt->token_origin->lexeme;
    marker.line = p_token_opt->line;
    marker.start_col = p_token_opt->col;
    marker.end_col = p_token_opt->col;
    marker.p_token_caret = p_token_opt;
    unsigned char   included_file_location = p_token_opt->level > 0;
    unsigned char   is_error = 0;
    unsigned char   is_warning = 0;
    unsigned char   is_note = 0;
    if (included_file_location)
    {
        return 0;
    }
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
    print_position(marker.file, marker.line, marker.start_col, ctx->options.visual_studio_ouput_format);
    char buffer[200] = {0};
    char * args = {0};
    ((void)(args = (char *)(&(fmt)) + ((sizeof (fmt) + sizeof (int) - 1) &  ~(sizeof (int) - 1))));
    vsnprintf(buffer, sizeof (buffer), fmt, args);
    ((void)(args = (char *)0));
    if (ctx->options.visual_studio_ouput_format)
    {
        if (is_warning)
        {
            printf("warning: ", buffer);
        }
        else
        {
            if (is_error)
            {
                printf("warning: ", buffer);
            }
            else
            {
                if (is_note)
                {
                    printf("note: ", buffer);
                }
            }
        }
        print_line_and_token(&marker, ctx->options.visual_studio_ouput_format);
    }
    else
    {
        if (is_error)
        {
            printf("\x1b[91m", buffer);
        }
        else
        {
            if (is_warning)
            {
                printf("\x1b[95m", buffer);
            }
            else
            {
                if (is_note)
                {
                    printf("\x1b[36;1m", buffer);
                }
            }
        }
        print_line_and_token(&marker, ctx->options.visual_studio_ouput_format);
    }
    return 1;
}

void *calloc(int nmemb, unsigned int size);
unsigned int strlen(char * _Str);
char *strdup(char * src);
int snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...);

struct include_dir *include_dir_add(struct include_dir_list * list, char * path)
{
    if (1)
    {
        struct include_dir * p_new_include_dir = calloc(1, sizeof  *p_new_include_dir);
        if (p_new_include_dir == ((void *)0))
        {
            goto _catch_label_1;
        }
        int len = strlen(path);
        if (path[len - 1] == 92)
        {
            char * temp = strdup(path);
            if (temp == ((void *)0))
            {
                free(p_new_include_dir);
                goto _catch_label_1;
            }
            p_new_include_dir->path = temp;
        }
        else
        {
            if (path[len - 1] != 47)
            {
                char * temp = calloc(len + 2, sizeof (char));
                if (temp == ((void *)0))
                {
                    free(p_new_include_dir);
                    goto _catch_label_1;
                }
                p_new_include_dir->path = temp;
                snprintf((char *)p_new_include_dir->path, len + 2, "%s/", path);
            }
            else
            {
                char * temp = strdup(path);
                if (temp == ((void *)0))
                {
                    free(p_new_include_dir);
                    goto _catch_label_1;
                }
                p_new_include_dir->path = temp;
            }
        }
        if (list->head == ((void *)0))
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
    else _catch_label_1:
    {
    }
    return ((void *)0);
}

int hashmap_set(struct hash_map * map, char * key, struct hash_item_set * item);
void hash_item_set_destroy(struct hash_item_set * p);

static void pragma_once_add(struct preprocessor_ctx * ctx, char * path)
{
    struct hash_item_set  item = {0};
    item.number = 1;
    hashmap_set(&ctx->pragma_once_map, path, &item);
    hash_item_set_destroy(&item);
}

struct map_entry *hashmap_find(struct hash_map * map, char * key);

static unsigned char  pragma_once_already_included(struct preprocessor_ctx * ctx, char * path)
{
    return hashmap_find(&ctx->pragma_once_map, path) != ((void *)0);
}

unsigned char  path_is_absolute(char * path);
void path_normalize(char * path);
char *read_file(char * path, unsigned char   append_newline);
char *realpath(char * path, char * resolved_path);

char *find_and_read_include_file(struct preprocessor_ctx * ctx, char * path, char * current_file_dir, unsigned char   is_angle_bracket_form, unsigned char  * p_already_included, char full_path_out[], int full_path_out_size)
{
    char newpath[200] = {0};
    full_path_out[0] = 0;
    if (path_is_absolute(path))
    {
        snprintf(newpath, sizeof newpath, "%s", path);
        path_normalize(newpath);
        if (pragma_once_already_included(ctx, newpath))
        {
             *p_already_included = 1;
            return ((void *)0);
        }
        char * content = read_file(newpath, 1);
        if (content != ((void *)0))
        {
            snprintf(full_path_out, full_path_out_size, "%s", path);
            return content;
        }
        return ((void *)0);
    }
    char * content = ((void *)0);
    if ( !is_angle_bracket_form)
    {
        snprintf(newpath, sizeof newpath, "%s/%s", current_file_dir, path);
        if ( !realpath(newpath, full_path_out))
        {
            full_path_out[0] = 0;
        }
        path_normalize(full_path_out);
        if (pragma_once_already_included(ctx, full_path_out))
        {
             *p_already_included = 1;
            return ((void *)0);
        }
        if (full_path_out[0] != 0)
        {
            content = read_file(full_path_out, 1);
        }
        if (content != ((void *)0))
        {
            return content;
        }
    }
    struct include_dir * current = ctx->include_dir.head;
    while (current)
    {
        int len = strlen(current->path);
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
            return ((void *)0);
        }
        content = read_file(full_path_out, 1);
        if (content != ((void *)0))
        {
            return content;
        }
        current = current->next;
    }
    full_path_out[0] = 0;
    return ((void *)0);
}

void add_macro(struct preprocessor_ctx * ctx, char * name)
{
    if (1)
    {
        char * name_local = strdup(name);
        if (name_local == ((void *)0))
        {
            goto _catch_label_1;
        }
        struct macro * macro = calloc(1, sizeof  *macro);
        if (macro == ((void *)0))
        {
            free(name_local);
            goto _catch_label_1;
        }
        macro->name = name_local;
        struct hash_item_set  item = {0, 0, 0, 0, 0, 0, 0, 0};
        item.p_macro = macro;
        hashmap_set(&ctx->macros, name, &item);
        hash_item_set_destroy(&item);
    }
    else _catch_label_1:
    {
    }
}

unsigned char  token_is_blank(struct token * p);
struct token *token_list_clone_and_add(struct token_list * list, struct token * pnew);
void remove_line_continuation(char * s);

struct token_list copy_argument_list_tokens(struct token_list * list)
{
    struct token_list  r = {0};
    struct token * current = list->head;
    while (current && (token_is_blank(current) || current->type == 10))
    {
        current = current->next;
    }
    unsigned char   is_first = 1;
    for (; current; )
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
        struct token * token = token_list_clone_and_add(&r, current);
        if (token->flags & 8)
        {
            token->flags = token->flags &  ~8;
            token->flags = 4;
        }
        if (is_first)
        {
            token->flags = token->flags &  ~4;
            token->flags = token->flags &  ~8;
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
        free((void *)p->name);
        free(p);
    }
}

void token_delete(struct token * p);
struct token *token_list_add(struct token_list * list, struct token * pnew);

struct token_list copy_argument_list(struct macro_argument * p_macro_argument)
{
    if (1)
    {
        struct token_list  list = copy_argument_list_tokens(&p_macro_argument->tokens);
        if (list.head == ((void *)0))
        {
            struct token * p_new_token = calloc(1, sizeof  *p_new_token);
            if (p_new_token == ((void *)0))
            {
                goto _catch_label_1;
            }
            char * temp = strdup("");
            if (temp == ((void *)0))
            {
                token_delete(p_new_token);
                goto _catch_label_1;
            }
            p_new_token->lexeme = temp;
            p_new_token->type = 142;
            token_list_add(&list, p_new_token);
        }
        return list;
    }
    else _catch_label_1:
    {
    }
    struct token_list  empty = {0};
    return empty;
}

void macro_argument_list_destroy(struct macro_argument_list * list)
{
    token_list_destroy(&list->tokens);
    struct macro_argument * p = list->head;
    while (p)
    {
        struct macro_argument * next = p->next;
        p->next = ((void *)0);
        macro_argument_delete(p);
        p = next;
    }
}

void print_list(struct token_list * list);

void print_macro_arguments(struct macro_argument_list * arguments)
{
    struct macro_argument * p_argument = arguments->head;
    while (p_argument)
    {
        printf("%s:", p_argument->name);
        print_list(&p_argument->tokens);
        p_argument = p_argument->next;
    }
}

int strcmp(char * _Str1, char * _Str2);

struct macro_argument *find_macro_argument_by_name(struct macro_argument_list * parameters, char * name)
{
    struct macro_argument * p = parameters->head;
    while (p)
    {
        if (strcmp(p->name, name) == 0)
        {
            return p;
        }
        p = p->next;
    }
    return ((void *)0);
}

void argument_list_add(struct macro_argument_list * list, struct macro_argument * pnew)
{
    ;
    if (list->head == ((void *)0))
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
    printf("%s", macro->name);
    if (macro->is_function)
    {
        printf("(");
    }
    struct macro_parameter * parameter = macro->parameters;
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
    struct macro_parameter * p = parameters;
    while (p)
    {
        struct macro_parameter * p_next = p->next;
        free((void *)p->name);
        free(p);
        p = p_next;
    }
}

void macro_delete(struct macro * macro)
{
    if (macro)
    {
        token_list_destroy(&macro->replacement_list);
        struct macro_parameter * p_macro_parameter = macro->parameters;
        while (p_macro_parameter)
        {
            struct macro_parameter * p_next = p_macro_parameter->next;
            free((void *)p_macro_parameter->name);
            free(p_macro_parameter);
            p_macro_parameter = p_next;
        }
        free((void *)macro->name);
        free(macro);
    }
}

struct macro *find_macro(struct preprocessor_ctx * ctx, char * name)
{
    struct map_entry * p_entry = hashmap_find(&ctx->macros, name);
    if (p_entry == ((void *)0))
    {
        return ((void *)0);
    }
    return p_entry->data.p_macro;
}

void stream_print_line(struct stream * stream)
{
    char * p = stream->current;
    while ((p - 1) >= stream->source &&  *(p - 1) != 10)
    {
        p--;
    }
    while ( *p &&  *(p + 1) != 10)
    {
        printf("%c",  *p);
        p++;
    }
    printf("\n");
    {
        int i = 0;
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
    while (stream->current[0] == 92 && stream->current[1] == 10)
    {
        stream->current++;
        stream->current++;
        stream->line++;
        stream->col = 1;
        stream->line_continuation_count++;
    }
}

void print_line(struct token * p)
{
    printf("%s\n", p->token_origin->lexeme);
    struct token * prev = p;
    while (prev->prev && prev->prev->type != 10)
    {
        prev = prev->prev;
    }
    struct token * next = prev;
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

int  is_punctuator(struct stream * stream)
{
    int   type = 0;
    switch (stream->current[0])
    {
        case 91 :
        type = 91;
        stream_match(stream);
        break;
        case 93 :
        type = 93;
        stream_match(stream);
        break;
        case 40 :
        type = 40;
        stream_match(stream);
        break;
        case 41 :
        type = 41;
        stream_match(stream);
        break;
        case 123 :
        type = 123;
        stream_match(stream);
        break;
        case 125 :
        type = 125;
        stream_match(stream);
        break;
        case 59 :
        type = 59;
        stream_match(stream);
        break;
        case 44 :
        type = 44;
        stream_match(stream);
        break;
        case 33 :
        type = 33;
        stream_match(stream);
        if (stream->current[0] == 61)
        {
            type = 8509;
            stream_match(stream);
        }
        break;
        case 58 :
        type = 58;
        stream_match(stream);
        if (stream->current[0] == 58)
        {
            type = 14906;
            stream_match(stream);
        }
        break;
        case 126 :
        type = 126;
        stream_match(stream);
        break;
        case 63 :
        type = 63;
        stream_match(stream);
        break;
        case 47 :
        type = 47;
        stream_match(stream);
        if (stream->current[0] == 61)
        {
            type = 12093;
            stream_match(stream);
        }
        break;
        case 42 :
        type = 42;
        stream_match(stream);
        if (stream->current[0] == 61)
        {
            type = 10813;
            stream_match(stream);
        }
        break;
        case 37 :
        type = 37;
        stream_match(stream);
        if (stream->current[0] == 61)
        {
            type = 9533;
            stream_match(stream);
        }
        break;
        case 45 :
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
        break;
        case 124 :
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
        break;
        case 43 :
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
        break;
        case 61 :
        type = 61;
        stream_match(stream);
        if (stream->current[0] == 61)
        {
            type = 15677;
            stream_match(stream);
        }
        break;
        case 94 :
        type = 94;
        stream_match(stream);
        if (stream->current[0] == 61)
        {
            type = 24125;
            stream_match(stream);
        }
        break;
        case 38 :
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
        break;
        case 62 :
        type = 62;
        stream_match(stream);
        if (stream->current[0] == 62)
        {
            type = 15934;
            stream_match(stream);
            if (stream->current[0] == 61)
            {
                type = 15933;
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
        break;
        case 60 :
        type = 60;
        stream_match(stream);
        if (stream->current[0] == 60)
        {
            type = 15420;
            stream_match(stream);
            if (stream->current[0] == 61)
            {
                type = 15421;
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
        break;
        case 35 :
        type = 35;
        stream_match(stream);
        if (stream->current[0] == 35)
        {
            type = 8995;
            stream_match(stream);
        }
        break;
        case 46 :
        type = 46;
        stream_match(stream);
        if (stream->current[0] == 46 && stream->current[1] == 46)
        {
            type = 11822;
            stream_match(stream);
            stream_match(stream);
        }
        break;
    }
    return type;
}

char *strncpy(char * _Destination, char * _Source, unsigned int _Count);

struct token *new_token(char * lexeme_head, char * lexeme_tail, int   type)
{
    struct token * p_new_token = ((void *)0);
    if (1)
    {
        p_new_token = calloc(1, sizeof  *p_new_token);
        if (p_new_token == ((void *)0))
        {
            goto _catch_label_1;
        }
        unsigned int sz = lexeme_tail - lexeme_head;
        char * temp = calloc(sz + 1, sizeof (char));
        if (temp == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_new_token->lexeme = temp;
        p_new_token->type = type;
        strncpy(p_new_token->lexeme, lexeme_head, sz);
    }
    else _catch_label_1:
    {
        token_delete(p_new_token);
        p_new_token = ((void *)0);
    }
    return p_new_token;
}

int is_digit(struct stream * p);

struct token *identifier(struct stream * stream)
{
    char * start = stream->current;
    stream_match(stream);
    while (is_nondigit(stream) || is_digit(stream))
    {
        stream_match(stream);
    }
    struct token * p_new_token = new_token(start, stream->current, 8996);
    return p_new_token;
}

static unsigned char  first_of_character_constant(struct stream * stream)
{
    return stream->current[0] == 39 || (stream->current[0] == 117 && stream->current[1] == 56 && stream->current[2] == 39) || (stream->current[0] == 117 && stream->current[1] == 39) || (stream->current[0] == 85 && stream->current[1] == 39) || (stream->current[0] == 76 && stream->current[1] == 39);
}

struct token *character_constant(struct tokenizer_ctx * ctx, struct stream * stream)
{
    char * start = stream->current;
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
    struct token * p_new_token = new_token(start, stream->current, 131);
    return p_new_token;
}

static unsigned char  first_of_string_literal(struct stream * stream)
{
    return stream->current[0] == 34 || (stream->current[0] == 117 && stream->current[1] == 56 && stream->current[2] == 34) || (stream->current[0] == 117 && stream->current[1] == 34) || (stream->current[0] == 85 && stream->current[1] == 34) || (stream->current[0] == 76 && stream->current[1] == 34);
}

struct token *string_literal(struct tokenizer_ctx * ctx, struct stream * stream)
{
    struct token * p_new_token = ((void *)0);
    char * start = stream->current;
    if (1)
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
                goto _catch_label_1;
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
    else _catch_label_1:
    {
    }
    return p_new_token;
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
            return (int)sizeof (unsigned short);
        }
    }
    return 1;
}

int string_literal_byte_size_not_zero_included(char * s)
{
    struct stream  stream = {0, 0, 0, 0, 0, 0};
    stream.source = s;
    stream.current = s;
    stream.line = 1;
    stream.col = 1;
    stream.path = "";
    int size = 0;
    int charsize = string_literal_char_byte_size(s);
    if (1)
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
                goto _catch_label_1;
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
    else _catch_label_1:
    {
    }
    return size * charsize;
}

static struct token *ppnumber(struct stream * stream)
{
    char * start = stream->current;
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
    struct token * p_new_token = new_token(start, stream->current, 134);
    return p_new_token;
}

struct _iobuf *fopen(char * _FileName, char * _Mode);
unsigned int fread(void * _Buffer, unsigned int _ElementSize, unsigned int _ElementCount, struct _iobuf * _Stream);
int fclose(struct _iobuf * _Stream);

struct token_list embed_tokenizer(struct preprocessor_ctx * ctx, struct token * position, char * filename_opt, int level, int   addflags)
{
    struct token_list  list = {0};
    struct _iobuf * file = ((void *)0);
    unsigned char   b_first = 1;
    int line = 1;
    int col = 1;
    int count = 0;
    if (1)
    {
        file = (struct _iobuf *)fopen(filename_opt, "rb");
        if (file == ((void *)0))
        {
            preprocessor_diagnostic_message(1150, ctx, position, "file '%s' not found", filename_opt);
            goto _catch_label_1;
        }
        unsigned char ch = 0;
        while (fread(&ch, 1, 1, file))
        {
            if (b_first)
            {
                b_first = 0;
            }
            else
            {
                char b[2] = ",";
                struct token * p_new_token = new_token(b, &b[1], 44);
                if (p_new_token == ((void *)0))
                {
                    goto _catch_label_1;
                }
                p_new_token->flags = addflags;
                p_new_token->level = level;
                p_new_token->token_origin = ((void *)0);
                p_new_token->line = line;
                p_new_token->col = col;
                token_list_add(&list, p_new_token);
                if (count > 0 && count % 25 == 0)
                {
                    char newline[2] = "\n";
                    struct token * p_new3 = new_token(newline, &newline[1], 10);
                    if (p_new3 == ((void *)0))
                    {
                        goto _catch_label_1;
                    }
                    p_new3->level = level;
                    p_new3->token_origin = ((void *)0);
                    p_new3->line = line;
                    p_new3->col = col;
                    token_list_add(&list, p_new3);
                }
            }
            char buffer[30] = {0};
            int c = snprintf(buffer, sizeof buffer, "%d", (int)ch);
            struct token * p_new_token = new_token(buffer, &buffer[c], 134);
            if (p_new_token == ((void *)0))
            {
                goto _catch_label_1;
            }
            p_new_token->flags = addflags;
            p_new_token->level = level;
            p_new_token->token_origin = ((void *)0);
            p_new_token->line = line;
            p_new_token->col = col;
            token_list_add(&list, p_new_token);
            count++;
        }
        char newline[2] = "\n";
        struct token * p_new_token = new_token(newline, &newline[1], 10);
        if (p_new_token == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_new_token->level = level;
        p_new_token->token_origin = ((void *)0);
        p_new_token->line = line;
        p_new_token->col = col;
        token_list_add(&list, p_new_token);
        ;
    }
    else _catch_label_1:
    {
    }
    if (file)
    {
        fclose(file);
    }
    return list;
}

static unsigned char  set_sliced_flag(struct stream * stream, struct token * p_new_token)
{
    if (stream->line_continuation_count > 0)
    {
        p_new_token->flags = 1024;
        if (stream->line_continuation_count == 1)
        {
            int l = strlen(p_new_token->lexeme);
            if (p_new_token->lexeme[l - 1] == 10)
            {
            }
            else
            {
                p_new_token->flags = 512;
            }
        }
        else
        {
            p_new_token->flags = 512;
        }
    }
    return p_new_token->flags & 512;
}

int isdigit(int _C);

struct token_list tokenizer(struct tokenizer_ctx * ctx, char * text, char * filename_opt, int level, int   addflags)
{
    struct token_list  list = {0};
    struct stream  stream = {0, 0, 1, 1, 0, 0};
    stream.source = text;
    stream.current = text;
    stream.path = filename_opt ? filename_opt : "";
    if (1)
    {
        struct token * p_first = ((void *)0);
        if (filename_opt != ((void *)0))
        {
            char * begin = filename_opt;
            char * end = filename_opt + strlen(filename_opt);
            struct token * p_new = new_token(begin, end, 8998);
            if (p_new == ((void *)0))
            {
                goto _catch_label_1;
            }
            path_normalize(p_new->lexeme);
            p_new->level = level;
            p_first = token_list_add(&list, p_new);
        }
        unsigned char   new_line = 1;
        unsigned char   has_space = 0;
        while (1)
        {
            int line = stream.line;
            int col = stream.col;
            stream.line_continuation_count = 0;
            if (stream.current[0] == 0)
            {
                stream_match(&stream);
                break;
            }
            if (is_digit(&stream) || (stream.current[0] == 46 && isdigit(stream.current[0])))
            {
                struct token * p_new_token = ppnumber(&stream);
                if (p_new_token == ((void *)0))
                {
                    goto _catch_label_1;
                }
                p_new_token->flags = has_space ? 4 : 0;
                p_new_token->flags = new_line ? 8 : 0;
                p_new_token->flags = addflags;
                p_new_token->flags = addflags;
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
                struct token * p_new_token = string_literal(ctx, &stream);
                if (p_new_token == ((void *)0))
                {
                    goto _catch_label_1;
                }
                p_new_token->flags = has_space ? 4 : 0;
                p_new_token->flags = new_line ? 8 : 0;
                p_new_token->flags = addflags;
                p_new_token->flags = addflags;
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
                struct token * p_new_token = character_constant(ctx, &stream);
                if (p_new_token == ((void *)0))
                {
                    goto _catch_label_1;
                }
                p_new_token->flags = has_space ? 4 : 0;
                p_new_token->flags = new_line ? 8 : 0;
                p_new_token->flags = addflags;
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
                struct token * p_new_token = identifier(&stream);
                if (p_new_token == ((void *)0))
                {
                    goto _catch_label_1;
                }
                p_new_token->flags = has_space ? 4 : 0;
                p_new_token->flags = new_line ? 8 : 0;
                p_new_token->flags = addflags;
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
                char * start = stream.current;
                while (stream.current[0] == 32 || stream.current[0] == 9 || stream.current[0] == 12)
                {
                    stream_match(&stream);
                }
                struct token * p_new_token = new_token(start, stream.current, 143);
                if (p_new_token == ((void *)0))
                {
                    goto _catch_label_1;
                }
                p_new_token->flags = has_space ? 4 : 0;
                p_new_token->flags = new_line ? 8 : 0;
                p_new_token->flags = addflags;
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
                char * start = stream.current;
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
                struct token * p_new_token = new_token(start, stream.current, 132);
                if (p_new_token == ((void *)0))
                {
                    goto _catch_label_1;
                }
                p_new_token->flags = has_space ? 4 : 0;
                p_new_token->flags = new_line ? 8 : 0;
                p_new_token->flags = addflags;
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
                char * start = stream.current;
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
                struct token * p_new_token = new_token(start, stream.current, 133);
                if (p_new_token == ((void *)0))
                {
                    goto _catch_label_1;
                }
                p_new_token->flags = has_space ? 4 : 0;
                p_new_token->flags = new_line ? 8 : 0;
                p_new_token->flags = addflags;
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
                char * start = stream.current;
                stream_match(&stream);
                struct token * p_new_token = new_token(start, stream.current, 35);
                if (p_new_token == ((void *)0))
                {
                    goto _catch_label_1;
                }
                p_new_token->flags = has_space ? 4 : 0;
                p_new_token->flags = new_line ? 8 : 0;
                p_new_token->flags = addflags;
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
                if (stream.current[0] == 13 && stream.current[1] == 10)
                {
                    stream_match(&stream);
                    stream_match(&stream);
                }
                else
                {
                    stream_match(&stream);
                }
                char newline[2] = "\n";
                struct token * p_new_token = new_token(newline, newline + 1, 10);
                if (p_new_token == ((void *)0))
                {
                    goto _catch_label_1;
                }
                p_new_token->flags = has_space ? 4 : 0;
                p_new_token->flags = new_line ? 8 : 0;
                p_new_token->flags = addflags;
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
            char * start = stream.current;
            int   t = is_punctuator(&stream);
            if (t != 0)
            {
                struct token * p_new_token = new_token(start, stream.current, t);
                if (p_new_token == ((void *)0))
                {
                    goto _catch_label_1;
                }
                p_new_token->flags = has_space ? 4 : 0;
                p_new_token->flags = new_line ? 8 : 0;
                p_new_token->flags = addflags;
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
                stream_match(&stream);
                struct token * p_new_token = new_token(start, stream.current, 135);
                if (p_new_token == ((void *)0))
                {
                    goto _catch_label_1;
                }
                p_new_token->flags = has_space ? 4 : 0;
                p_new_token->flags = new_line ? 8 : 0;
                p_new_token->flags = addflags;
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
    else _catch_label_1:
    {
    }
    ;
    return list;
}

int feof(struct _iobuf * _Stream);

unsigned char  fread2(void * buffer, unsigned int size, unsigned int count, struct _iobuf * stream, unsigned int * sz)
{
     *sz = 0;
    unsigned char   result = 0;
    unsigned int n = fread(buffer, size, count, stream);
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

unsigned char  token_list_is_empty(struct token_list * p);
unsigned char  preprocessor_token_ahead_is_identifier(struct token * p, char * lexeme);
struct token_list group_part(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char   is_active, int level);
void token_list_append_list(struct token_list * dest, struct token_list * source);

struct token_list group_opt(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char   is_active, int level)
{
    struct token_list  r = {0};
    if (1)
    {
        if (token_list_is_empty(input_list))
        {
            return r;
        }
        while ( !token_list_is_empty(input_list))
        {
            ;
            if (input_list->head->type == 127 && (preprocessor_token_ahead_is_identifier(input_list->head, "endif") || preprocessor_token_ahead_is_identifier(input_list->head, "else") || preprocessor_token_ahead_is_identifier(input_list->head, "elif") || preprocessor_token_ahead_is_identifier(input_list->head, "elifdef") || preprocessor_token_ahead_is_identifier(input_list->head, "elifndef")))
            {
                break;
            }
            else
            {
                struct token_list  r2 = group_part(ctx, input_list, is_active, level);
                token_list_append_list(&r, &r2);
                token_list_destroy(&r2);
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
    return r;
}

unsigned char  is_parser_token(struct token * p)
{
    return p->type != 133 && p->type != 143 && p->type != 132 && p->type != 10;
}

unsigned char  is_never_final(int   type)
{
    return type == 8998 || type == 143 || type == 132 || type == 133 || type == 142 || type == 10;
}

struct token *preprocessor_look_ahead_core(struct token * p)
{
    struct token * current = p->next;
    while (current && (current->type == 143 || current->type == 142 || current->type == 132 || current->type == 133))
    {
        current = current->next;
    }
    return current;
}

unsigned char  preprocessor_token_ahead_is(struct token * p, int   t)
{
    struct token * p_token = preprocessor_look_ahead_core(p);
    if (p_token != ((void *)0) && p_token->type == t)
    {
        return 1;
    }
    return 0;
}

unsigned char  preprocessor_token_ahead_is_identifier(struct token * p, char * lexeme)
{
    ;
    struct token * p_token = preprocessor_look_ahead_core(p);
    if (p_token != ((void *)0) && p_token->type == 8996)
    {
        return strcmp(p_token->lexeme, lexeme) == 0;
    }
    return 0;
}

struct token *token_list_pop_front_get(struct token_list * list);
void token_list_pop_front(struct token_list * list);

static void skip_blanks_level(struct preprocessor_ctx * ctx, struct token_list * dest, struct token_list * input_list, int level)
{
    while (input_list->head)
    {
        if ( !token_is_blank(input_list->head))
        {
            break;
        }
        if (1 || level == 0)
        {
            struct token * p = token_list_pop_front_get(input_list);
            ;
            token_list_add(dest, p);
        }
        else
        {
            token_list_pop_front(input_list);
        }
    }
}

static void skip_blanks(struct preprocessor_ctx * ctx, struct token_list * dest, struct token_list * input_list)
{
    while (input_list->head)
    {
        if ( !token_is_blank(input_list->head))
        {
            break;
        }
        struct token * p = token_list_pop_front_get(input_list);
        ;
        token_list_add(dest, p);
    }
}

void prematch_level(struct token_list * dest, struct token_list * input_list, int level)
{
    if (1 || level == 0)
    {
        struct token * p = token_list_pop_front_get(input_list);
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
    struct token * p = token_list_pop_front_get(input_list);
    if (p)
    {
        token_list_add(dest, p);
    }
}

char *strcat(char * _Destination, char * _Source);

struct token_list process_defined(struct preprocessor_ctx * ctx, struct token_list * input_list)
{
    struct token_list  r = {0};
    if (1)
    {
        while (input_list->head != ((void *)0))
        {
            if (input_list->head->type == 8996 && strcmp(input_list->head->lexeme, "defined") == 0)
            {
                token_list_pop_front(input_list);
                skip_blanks(ctx, &r, input_list);
                if (input_list->head == ((void *)0))
                {
                    pre_unexpected_end_of_file(r.tail, ctx);
                    goto _catch_label_1;
                }
                unsigned char   has_parentesis = 0;
                if (input_list->head->type == 40)
                {
                    token_list_pop_front(input_list);
                    has_parentesis = 1;
                }
                skip_blanks(ctx, &r, input_list);
                if (input_list->head == ((void *)0))
                {
                    pre_unexpected_end_of_file(r.tail, ctx);
                    goto _catch_label_1;
                }
                struct macro * macro = find_macro(ctx, input_list->head->lexeme);
                struct token * p_new_token = token_list_pop_front_get(input_list);
                if (p_new_token == ((void *)0))
                {
                    goto _catch_label_1;
                }
                p_new_token->type = 134;
                char * temp = ((void *)0);
                if (macro)
                {
                    temp = strdup("1");
                }
                else
                {
                    temp = strdup("0");
                }
                if (temp == ((void *)0))
                {
                    token_delete(p_new_token);
                    goto _catch_label_1;
                }
                free(p_new_token->lexeme);
                p_new_token->lexeme = temp;
                token_list_add(&r, p_new_token);
                if (has_parentesis)
                {
                    if (input_list->head == ((void *)0))
                    {
                        goto _catch_label_1;
                    }
                    if (input_list->head->type != 41)
                    {
                        preprocessor_diagnostic_message(1160, ctx, input_list->head, "missing )");
                        goto _catch_label_1;
                    }
                    token_list_pop_front(input_list);
                }
            }
            else
            {
                if (input_list->head->type == 8996 && (strcmp(input_list->head->lexeme, "__has_include") == 0 || strcmp(input_list->head->lexeme, "__has_embed") == 0))
                {
                    token_list_pop_front(input_list);
                    skip_blanks(ctx, &r, input_list);
                    token_list_pop_front(input_list);
                    skip_blanks(ctx, &r, input_list);
                    char path[100] = {0};
                    unsigned char   is_angle_bracket_form = 0;
                    if (input_list->head == ((void *)0))
                    {
                        pre_unexpected_end_of_file(r.tail, ctx);
                        goto _catch_label_1;
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
                        if (input_list->head == ((void *)0))
                        {
                            pre_unexpected_end_of_file(r.tail, ctx);
                            goto _catch_label_1;
                        }
                        while (input_list->head->type != 62)
                        {
                            strcat(path, input_list->head->lexeme);
                            token_list_pop_front(input_list);
                            if (input_list->head == ((void *)0))
                            {
                                pre_unexpected_end_of_file(r.tail, ctx);
                                goto _catch_label_1;
                            }
                        }
                        token_list_pop_front(input_list);
                    }
                    char fullpath[300] = {0};
                    char full_path_result[200] = {0};
                    unsigned char   already_included = 0;
                    char * s = find_and_read_include_file(ctx, path, fullpath, is_angle_bracket_form, &already_included, full_path_result, sizeof full_path_result);
                    unsigned char   has_include = s != ((void *)0);
                    free((void *)s);
                    struct token * p_new_token = calloc(1, sizeof  *p_new_token);
                    if (p_new_token == ((void *)0))
                    {
                        goto _catch_label_1;
                    }
                    p_new_token->type = 134;
                    char * temp = strdup(has_include ? "1" : "0");
                    if (temp == ((void *)0))
                    {
                        token_delete(p_new_token);
                        goto _catch_label_1;
                    }
                    p_new_token->lexeme = temp;
                    p_new_token->flags = 1;
                    token_list_add(&r, p_new_token);
                    token_list_pop_front(input_list);
                }
                else
                {
                    if (input_list->head->type == 8996 && strcmp(input_list->head->lexeme, "__has_c_attribute") == 0)
                    {
                        token_list_pop_front(input_list);
                        skip_blanks(ctx, &r, input_list);
                        token_list_pop_front(input_list);
                        skip_blanks(ctx, &r, input_list);
                        if (input_list->head == ((void *)0))
                        {
                            pre_unexpected_end_of_file(r.tail, ctx);
                            goto _catch_label_1;
                        }
                        char path[100] = {0};
                        while (input_list->head->type != 41)
                        {
                            strcat(path, input_list->head->lexeme);
                            token_list_pop_front(input_list);
                            if (input_list->head == ((void *)0))
                            {
                                pre_unexpected_end_of_file(r.tail, ctx);
                                goto _catch_label_1;
                            }
                        }
                        token_list_pop_front(input_list);
                        unsigned char   has_c_attribute = 0;
                        struct token * p_new_token = calloc(1, sizeof  *p_new_token);
                        if (p_new_token == ((void *)0))
                        {
                            goto _catch_label_1;
                        }
                        p_new_token->type = 134;
                        char * temp = strdup(has_c_attribute ? "1" : "0");
                        if (temp == ((void *)0))
                        {
                            token_delete(p_new_token);
                            goto _catch_label_1;
                        }
                        p_new_token->lexeme = temp;
                        p_new_token->flags = 1;
                        token_list_add(&r, p_new_token);
                        token_list_pop_front(input_list);
                    }
                    else
                    {
                        struct token * tk = token_list_pop_front_get(input_list);
                        if (tk)
                        {
                            token_list_add(&r, tk);
                        }
                    }
                }
            }
        }
    }
    else _catch_label_1:
    {
    }
    return r;
}

struct token_list process_identifiers(struct preprocessor_ctx * ctx, struct token_list * list)
{
    ;
    struct token_list  list2 = {0};
    if (1)
    {
        while (list->head != ((void *)0))
        {
            if (list->head->type == 8996)
            {
                struct macro * macro = find_macro(ctx, list->head->lexeme);
                struct token * p_new_token = token_list_pop_front_get(list);
                ;
                p_new_token->type = 134;
                if (macro)
                {
                    char * temp = strdup("1");
                    if (temp == ((void *)0))
                    {
                        token_delete(p_new_token);
                        goto _catch_label_1;
                    }
                    free(p_new_token->lexeme);
                    p_new_token->lexeme = temp;
                }
                else
                {
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
                            char * temp = strdup("0");
                            if (temp == ((void *)0))
                            {
                                token_delete(p_new_token);
                                goto _catch_label_1;
                            }
                            free(p_new_token->lexeme);
                            p_new_token->lexeme = temp;
                        }
                    }
                }
                token_list_add(&list2, p_new_token);
            }
            else
            {
                struct token * ptk = token_list_pop_front_get(list);
                ;
                token_list_add(&list2, ptk);
            }
        }
        ;
    }
    else _catch_label_1:
    {
    }
    return list2;
}

struct token_list ignore_preprocessor_line(struct token_list * input_list)
{
    struct token_list  r = {0};
    while (input_list->head && input_list->head->type != 10)
    {
        struct token * tk = token_list_pop_front_get(input_list);
        ;
        token_list_add(&r, tk);
    }
    return r;
}

struct token_list copy_replacement_list(struct token_list * list);
void token_list_swap(struct token_list * a, struct token_list * b);
struct token_list preprocessor(struct preprocessor_ctx * ctx, struct token_list * input_list, int level);
int pre_constant_expression(struct preprocessor_ctx * ctx, long long * pvalue);

long long preprocessor_constant_expression(struct preprocessor_ctx * ctx, struct token_list * output_list, struct token_list * input_list, int level)
{
    ;
    struct token * first = input_list->head;
    ctx->conditional_inclusion = 1;
    struct token_list  r = {0};
    while (input_list->head && input_list->head->type != 10)
    {
        struct token * tk = token_list_pop_front_get(input_list);
        ;
        token_list_add(&r, tk);
        ;
        r.tail->flags =  ~1024;
    }
    struct token_list  list1 = copy_replacement_list(&r);
    token_list_swap(output_list, &r);
    int flags = ctx->flags;
    ctx->flags = 1;
    struct token_list  list2 = preprocessor(ctx, &list1, 1);
    ctx->flags = flags;
    long long value = 0;
    if (list2.head == ((void *)0))
    {
        preprocessor_diagnostic_message(1170, ctx, first, "empty expression");
    }
    else
    {
        struct token_list  list3 = process_defined(ctx, &list2);
        struct token_list  list4 = process_identifiers(ctx, &list3);
        ;
        struct preprocessor_ctx  pre_ctx = {0};
        pre_ctx.input_list = list4;
        pre_ctx.current = pre_ctx.input_list.head;
        if (pre_constant_expression(&pre_ctx, &value) != 0)
        {
            preprocessor_diagnostic_message(1170, ctx, first, "expression error");
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
    if (1 || level == 0)
    {
        struct token * tk = token_list_pop_front_get(input_list);
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

char *get_token_name(int   tk);

int match_token_level(struct token_list * dest, struct token_list * input_list, int   type, int level, struct preprocessor_ctx * ctx)
{
    if (1)
    {
        if (input_list->head == ((void *)0) || input_list->head->type != type)
        {
            if (type == 10 && input_list->head == ((void *)0))
            {
            }
            else
            {
                if (input_list->head)
                {
                    preprocessor_diagnostic_message(970, ctx, input_list->head, "expected token %s got %s\n", get_token_name(type), get_token_name(input_list->head->type));
                }
                else
                {
                    preprocessor_diagnostic_message(970, ctx, dest->tail, "expected EOF \n");
                }
                goto _catch_label_1;
            }
        }
        if (input_list->head != ((void *)0))
        {
            if (1 || level == 0)
            {
                token_list_add(dest, token_list_pop_front_get(input_list));
            }
            else
            {
                token_list_pop_front(input_list);
            }
        }
    }
    else _catch_label_1:
    {
    }
    return ctx->n_errors > 0;
}

struct token_list if_group(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char   is_active, int level, unsigned char  * p_result)
{
     *p_result = 0;
    ;
    struct token_list  r = {0};
    if (1)
    {
        match_token_level(&r, input_list, 127, level, ctx);
        skip_blanks_level(ctx, &r, input_list, level);
        if (input_list->head == ((void *)0))
        {
            goto _catch_label_1;
        }
        ;
        if (strcmp(input_list->head->lexeme, "ifdef") == 0)
        {
            match_token_level(&r, input_list, 8996, level, ctx);
            skip_blanks_level(ctx, &r, input_list, level);
            if (input_list->head == ((void *)0))
            {
                pre_unexpected_end_of_file(r.tail, ctx);
                goto _catch_label_1;
            }
            if (is_active)
            {
                struct macro * macro = find_macro(ctx, input_list->head->lexeme);
                 *p_result = (macro != ((void *)0)) ? 1 : 0;
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
                if (input_list->head == ((void *)0))
                {
                    pre_unexpected_end_of_file(r.tail, ctx);
                    goto _catch_label_1;
                }
                if (is_active)
                {
                    struct macro * macro = find_macro(ctx, input_list->head->lexeme);
                     *p_result = (macro == ((void *)0)) ? 1 : 0;
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
                        struct token_list  r0 = {0};
                         *p_result = preprocessor_constant_expression(ctx, &r0, input_list, level);
                        token_list_append_list(&r, &r0);
                        token_list_destroy(&r0);
                    }
                    else
                    {
                        struct token_list  r0 = ignore_preprocessor_line(input_list);
                        token_list_append_list(&r, &r0);
                        token_list_destroy(&r0);
                    }
                    match_token_level(&r, input_list, 10, level, ctx);
                }
                else
                {
                    preprocessor_diagnostic_message(650, ctx, input_list->head, "unexpected");
                    goto _catch_label_1;
                }
            }
        }
        struct token_list  r2 = group_opt(ctx, input_list, is_active &&  *p_result, level);
        token_list_append_list(&r, &r2);
        ;
        ;
    }
    else _catch_label_1:
    {
    }
    return r;
}

struct token_list elif_group(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char   is_active, int level, unsigned char  * p_elif_result)
{
     *p_elif_result = 0;
    ;
    struct token_list  r = {0};
    if (1)
    {
        match_token_level(&r, input_list, 127, level, ctx);
        skip_blanks(ctx, &r, input_list);
        if (input_list->head == ((void *)0))
        {
            goto _catch_label_1;
        }
        unsigned long long result = 0;
        if (strcmp(input_list->head->lexeme, "elif") == 0)
        {
            match_token_level(&r, input_list, 8996, level, ctx);
            skip_blanks(ctx, &r, input_list);
            if (is_active)
            {
                struct token_list  r0 = {0};
                result = preprocessor_constant_expression(ctx, &r0, input_list, level);
                token_list_append_list(&r, &r0);
                token_list_destroy(&r0);
            }
            else
            {
                struct token_list  r0 = ignore_preprocessor_line(input_list);
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
                if (input_list->head == ((void *)0))
                {
                    goto _catch_label_1;
                }
                if (is_active)
                {
                    result = (hashmap_find(&ctx->macros, input_list->head->lexeme) != ((void *)0)) ? 1 : 0;
                }
                match_token_level(&r, input_list, 8996, level, ctx);
            }
            else
            {
                if (strcmp(input_list->head->lexeme, "elifndef") == 0)
                {
                    match_token_level(&r, input_list, 8996, level, ctx);
                    skip_blanks(ctx, &r, input_list);
                    if (input_list->head == ((void *)0))
                    {
                        goto _catch_label_1;
                    }
                    if (is_active)
                    {
                        result = (hashmap_find(&ctx->macros, input_list->head->lexeme) == ((void *)0)) ? 1 : 0;
                    }
                    match_token_level(&r, input_list, 8996, level, ctx);
                }
            }
        }
         *p_elif_result = (result != 0);
        skip_blanks(ctx, &r, input_list);
        match_token_level(&r, input_list, 10, level, ctx);
        struct token_list  r2 = group_opt(ctx, input_list, is_active &&  *p_elif_result, level);
        token_list_append_list(&r, &r2);
        token_list_destroy(&r2);
    }
    else _catch_label_1:
    {
    }
    return r;
}

struct token_list elif_groups(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char   is_active, int level, unsigned char  * pelif_result)
{
    ;
    struct token_list  r = {0};
    if (1)
    {
        unsigned char   already_found_elif_true = 0;
        unsigned char   elif_result = 0;
        struct token_list  r2 = elif_group(ctx, input_list, is_active, level, &elif_result);
        if (input_list->head == ((void *)0))
        {
            token_list_destroy(&r2);
            goto _catch_label_1;
        }
        token_list_append_list(&r, &r2);
        if (elif_result)
        {
            already_found_elif_true = 1;
        }
        if (input_list->head->type == 127 && (preprocessor_token_ahead_is_identifier(input_list->head, "elif") || preprocessor_token_ahead_is_identifier(input_list->head, "elifdef") || preprocessor_token_ahead_is_identifier(input_list->head, "elifndef")))
        {
            struct token_list  r3 = elif_groups(ctx, input_list, is_active &&  !already_found_elif_true, level, &elif_result);
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
    else _catch_label_1:
    {
    }
    return r;
}

struct token_list else_group(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char   is_active, int level)
{
    struct token_list  r = {0};
    if (1)
    {
        match_token_level(&r, input_list, 127, level, ctx);
        skip_blanks_level(ctx, &r, input_list, level);
        match_token_level(&r, input_list, 8996, level, ctx);
        skip_blanks_level(ctx, &r, input_list, level);
        match_token_level(&r, input_list, 10, level, ctx);
        struct token_list  r2 = group_opt(ctx, input_list, is_active, level);
        token_list_append_list(&r, &r2);
        token_list_destroy(&r2);
    }
    else _catch_label_1:
    {
    }
    return r;
}

struct token_list endif_line(struct preprocessor_ctx * ctx, struct token_list * input_list, int level)
{
    struct token_list  r = {0};
    match_token_level(&r, input_list, 127, level, ctx);
    skip_blanks_level(ctx, &r, input_list, level);
    match_token_level(&r, input_list, 8996, level, ctx);
    skip_blanks_level(ctx, &r, input_list, level);
    match_token_level(&r, input_list, 10, level, ctx);
    return r;
}

struct token_list if_section(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char   is_active, int level)
{
    ;
    struct token_list  r = {0};
    if (1)
    {
        unsigned char   if_result = 0;
        struct token_list  r2 = if_group(ctx, input_list, is_active, level, &if_result);
        if (ctx->n_errors > 0)
        {
            token_list_destroy(&r2);
            goto _catch_label_1;
        }
        if (input_list->head == ((void *)0))
        {
            token_list_destroy(&r2);
            goto _catch_label_1;
        }
        token_list_append_list(&r, &r2);
        unsigned char   elif_result = 0;
        if (input_list->head->type == 127 && (preprocessor_token_ahead_is_identifier(input_list->head, "elif") || preprocessor_token_ahead_is_identifier(input_list->head, "elifdef") || preprocessor_token_ahead_is_identifier(input_list->head, "elifndef")))
        {
            struct token_list  r3 = elif_groups(ctx, input_list, is_active &&  !if_result, level, &elif_result);
            token_list_append_list(&r, &r3);
            token_list_destroy(&r3);
        }
        if (input_list->head == ((void *)0))
        {
            token_list_destroy(&r2);
            pre_unexpected_end_of_file(r.tail, ctx);
            goto _catch_label_1;
        }
        if (input_list->head->type == 127 && preprocessor_token_ahead_is_identifier(input_list->head, "else"))
        {
            struct token_list  r4 = else_group(ctx, input_list, is_active &&  !if_result &&  !elif_result, level);
            token_list_append_list(&r, &r4);
            token_list_destroy(&r4);
        }
        if (ctx->n_errors > 0)
        {
            token_list_destroy(&r2);
            goto _catch_label_1;
        }
        struct token_list  r5 = endif_line(ctx, input_list, level);
        token_list_append_list(&r, &r5);
        token_list_destroy(&r5);
        token_list_destroy(&r2);
    }
    else _catch_label_1:
    {
    }
    return r;
}

struct token_list identifier_list(struct preprocessor_ctx * ctx, struct macro * macro, struct token_list * input_list, int level)
{
    ;
    struct token_list  r = {0};
    if (1)
    {
        skip_blanks(ctx, &r, input_list);
        if (input_list->head == ((void *)0))
        {
            goto _catch_label_1;
        }
        struct macro_parameter * p_macro_parameter = calloc(1, sizeof  *p_macro_parameter);
        if (p_macro_parameter == ((void *)0))
        {
            goto _catch_label_1;
        }
        char * temp = strdup(input_list->head->lexeme);
        if (temp == ((void *)0))
        {
            macro_parameters_delete(p_macro_parameter);
            goto _catch_label_1;
        }
        p_macro_parameter->name = temp;
        ;
        macro->parameters = p_macro_parameter;
        struct macro_parameter * p_last_parameter = macro->parameters;
        match_token_level(&r, input_list, 8996, level, ctx);
        skip_blanks(ctx, &r, input_list);
        if (input_list->head == ((void *)0))
        {
            goto _catch_label_1;
        }
        while (input_list->head->type == 44)
        {
            match_token_level(&r, input_list, 44, level, ctx);
            skip_blanks(ctx, &r, input_list);
            if (input_list->head == ((void *)0))
            {
                goto _catch_label_1;
            }
            if (input_list->head->type == 11822)
            {
                break;
            }
            struct macro_parameter * p_new_macro_parameter = calloc(1, sizeof  *p_new_macro_parameter);
            if (p_new_macro_parameter == ((void *)0))
            {
                goto _catch_label_1;
            }
            char * temp2 = strdup(input_list->head->lexeme);
            if (temp2 == ((void *)0))
            {
                macro_parameters_delete(p_new_macro_parameter);
                goto _catch_label_1;
            }
            p_new_macro_parameter->name = temp2;
            ;
            p_last_parameter->next = p_new_macro_parameter;
            p_last_parameter = p_last_parameter->next;
            match_token_level(&r, input_list, 8996, level, ctx);
            skip_blanks(ctx, &r, input_list);
            if (input_list->head == ((void *)0))
            {
                goto _catch_label_1;
            }
        }
    }
    else _catch_label_1:
    {
    }
    return r;
}

struct token_list replacement_list(struct preprocessor_ctx * ctx, struct macro * macro, struct token_list * input_list, int level)
{
    struct token_list  r = {0};
    if (1)
    {
        if (input_list->head == ((void *)0))
        {
            pre_unexpected_end_of_file(((void *)0), ctx);
            goto _catch_label_1;
        }
        while (input_list->head->type != 10)
        {
            match_level(&r, input_list, level);
            if (input_list->head == ((void *)0))
            {
                break;
            }
        }
        ;
        struct token_list  copy = copy_replacement_list(&r);
        token_list_append_list(&macro->replacement_list, &copy);
        token_list_destroy(&copy);
    }
    else _catch_label_1:
    {
    }
    return r;
}

struct token_list pp_tokens_opt(struct preprocessor_ctx * ctx, struct token_list * input_list, int level)
{
    struct token_list  r = {0};
    while (input_list->head && input_list->head->type != 10)
    {
        prematch_level(&r, input_list, level);
    }
    return r;
}

static unsigned char  is_empty_assert(struct token_list * replacement_list)
{
    struct token * token = replacement_list->head;
    if (token == ((void *)0))
    {
        return 0;
    }
    if (strcmp(token->lexeme, "("))
    {
        return 0;
    }
    token = token->next;
    if (token == ((void *)0))
    {
        return 0;
    }
    if (strcmp(token->lexeme, "("))
    {
        return 0;
    }
    token = token->next;
    if (token == ((void *)0))
    {
        return 0;
    }
    if (strcmp(token->lexeme, "void"))
    {
        return 0;
    }
    token = token->next;
    if (token == ((void *)0))
    {
        return 0;
    }
    if (strcmp(token->lexeme, ")"))
    {
        return 0;
    }
    token = token->next;
    if (token == ((void *)0))
    {
        return 0;
    }
    if (strcmp(token->lexeme, "0"))
    {
        return 0;
    }
    token = token->next;
    if (token == ((void *)0))
    {
        return 0;
    }
    if (strcmp(token->lexeme, ")"))
    {
        return 0;
    }
    token = token->next;
    if (token != ((void *)0))
    {
        return 0;
    }
    return 1;
}

char *dirname(char * path);
void naming_convention_macro(struct preprocessor_ctx * ctx, struct token * token);
void *hashmap_remove(struct hash_map * map, char * key, int  * p_type_opt);
unsigned long long get_warning_bit_mask(char * wname);

struct token_list control_line(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char   is_active, int level)
{
    struct token_list  r = {0};
    if (1)
    {
        if ( !is_active)
        {
            struct token_list  r7 = pp_tokens_opt(ctx, input_list, level);
            token_list_append_list(&r, &r7);
            match_token_level(&r, input_list, 10, level, ctx);
            token_list_destroy(&r7);
            return r;
        }
        if (input_list->head == ((void *)0))
        {
            pre_unexpected_end_of_file(r.tail, ctx);
            goto _catch_label_1;
        }
        struct token * ptoken = input_list->head;
        match_token_level(&r, input_list, 127, level, ctx);
        skip_blanks_level(ctx, &r, input_list, level);
        if (input_list->head == ((void *)0))
        {
            pre_unexpected_end_of_file(r.tail, ctx);
            goto _catch_label_1;
        }
        if (strcmp(input_list->head->lexeme, "include") == 0)
        {
            match_token_level(&r, input_list, 8996, level, ctx);
            skip_blanks_level(ctx, &r, input_list, level);
            if (input_list->head == ((void *)0))
            {
                pre_unexpected_end_of_file(r.tail, ctx);
                goto _catch_label_1;
            }
            char path[100] = {0};
            unsigned char   is_angle_bracket_form = 0;
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
                    if (input_list->head == ((void *)0))
                    {
                        pre_unexpected_end_of_file(r.tail, ctx);
                        goto _catch_label_1;
                    }
                }
                strcat(path, input_list->head->lexeme);
                prematch_level(&r, input_list, level);
            }
            while (input_list->head->type != 10)
            {
                prematch_level(&r, input_list, level);
                if (input_list->head == ((void *)0))
                {
                    pre_unexpected_end_of_file(r.tail, ctx);
                    goto _catch_label_1;
                }
            }
            match_token_level(&r, input_list, 10, level, ctx);
            path[strlen(path) - 1] = 0;
            char current_file_dir[300] = {0};
            snprintf(current_file_dir, sizeof current_file_dir, "%s", r.tail->token_origin->lexeme);
            dirname(current_file_dir);
            char full_path_result[200] = {0};
            unsigned char   already_included = 0;
            char * content = find_and_read_include_file(ctx, path + 1, current_file_dir, is_angle_bracket_form, &already_included, full_path_result, sizeof full_path_result);
            if (content != ((void *)0))
            {
                if (ctx->options.show_includes)
                {
                    printf("Note: including file:");
                    printf("%*c", level + 1, 32);
                    printf("%s\n", full_path_result);
                }
                struct tokenizer_ctx  tctx = {0};
                struct token_list  list = tokenizer(&tctx, content, full_path_result, level + 1, 0);
                free((void *)content);
                struct token_list  list2 = preprocessor(ctx, &list, level + 1);
                token_list_append_list(&r, &list2);
                token_list_destroy(&list2);
                token_list_destroy(&list);
            }
            else
            {
                if ( !already_included)
                {
                    preprocessor_diagnostic_message(1150, ctx, r.tail, "file %s not found", path + 1);
                    {
                        struct include_dir * p = ctx->include_dir.head;
                        for (; p; p = p->next)
                        {
                            preprocessor_diagnostic_message(63, ctx, r.tail, "dir = '%s'", p->path);
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
                struct token_list  discard0 = {0};
                struct token_list * p_list = &r;
                if (ctx->options.target < 0)
                {
                    p_list = &discard0;
                    char * temp = strdup(" ");
                    if (temp == ((void *)0))
                    {
                        goto _catch_label_1;
                    }
                    free(ptoken->lexeme);
                    ptoken->lexeme = temp;
                }
                struct token * p_embed_token = input_list->head;
                match_token_level(p_list, input_list, 8996, level, ctx);
                skip_blanks_level(ctx, p_list, input_list, level);
                if (input_list->head == ((void *)0))
                {
                    goto _catch_label_1;
                }
                char path[100] = {0};
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
                        if (input_list->head == ((void *)0))
                        {
                            goto _catch_label_1;
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
                        if (input_list->head == ((void *)0))
                        {
                            pre_unexpected_end_of_file(p_list->tail, ctx);
                            goto _catch_label_1;
                        }
                    }
                }
                match_token_level(p_list, input_list, 10, level, ctx);
                char fullpath[300] = {0};
                path[strlen(path) - 1] = 0;
                snprintf(fullpath, sizeof (fullpath), "%s", path + 1);
                int nlevel = level;
                int   f = 0;
                if (ctx->options.target < 0)
                {
                    f = 1;
                }
                else
                {
                    f = 1;
                    nlevel = nlevel + 1;
                }
                struct token_list  list = embed_tokenizer(ctx, p_embed_token, fullpath, nlevel, f);
                if (ctx->n_errors > 0)
                {
                    token_list_destroy(&list);
                    goto _catch_label_1;
                }
                token_list_append_list(&r, &list);
                token_list_destroy(&list);
                token_list_destroy(&discard0);
            }
            else
            {
                if (strcmp(input_list->head->lexeme, "define") == 0)
                {
                    struct macro * macro = calloc(1, sizeof  *macro);
                    if (macro == ((void *)0))
                    {
                        preprocessor_diagnostic_message(650, ctx, ctx->current, "_Out of mem");
                        goto _catch_label_1;
                    }
                    match_token_level(&r, input_list, 8996, level, ctx);
                    skip_blanks_level(ctx, &r, input_list, level);
                    if (input_list->head == ((void *)0))
                    {
                        macro_delete(macro);
                        pre_unexpected_end_of_file(r.tail, ctx);
                        goto _catch_label_1;
                    }
                    struct token * macro_name_token = input_list->head;
                    if (hashmap_find(&ctx->macros, input_list->head->lexeme) != ((void *)0))
                    {
                    }
                    char * temp = strdup(input_list->head->lexeme);
                    if (temp == ((void *)0))
                    {
                        macro_delete(macro);
                        goto _catch_label_1;
                    }
                    ;
                    macro->name = temp;
                    match_token_level(&r, input_list, 8996, level, ctx);
                    if (input_list->head == ((void *)0))
                    {
                        macro_delete(macro);
                        pre_unexpected_end_of_file(r.tail, ctx);
                        goto _catch_label_1;
                    }
                    if (input_list->head->type == 40)
                    {
                        macro->is_function = 1;
                        match_token_level(&r, input_list, 40, level, ctx);
                        skip_blanks_level(ctx, &r, input_list, level);
                        if (input_list->head == ((void *)0))
                        {
                            macro_delete(macro);
                            pre_unexpected_end_of_file(r.tail, ctx);
                            goto _catch_label_1;
                        }
                        if (input_list->head->type == 11822)
                        {
                            struct macro_parameter * p_macro_parameter = calloc(1, sizeof  *p_macro_parameter);
                            if (p_macro_parameter == ((void *)0))
                            {
                                macro_delete(macro);
                                goto _catch_label_1;
                            }
                            char * temp2 = strdup("__VA_ARGS__");
                            if (temp2 == ((void *)0))
                            {
                                macro_delete(macro);
                                macro_parameters_delete(p_macro_parameter);
                                goto _catch_label_1;
                            }
                            p_macro_parameter->name = temp2;
                            macro->parameters = p_macro_parameter;
                            match_token_level(&r, input_list, 11822, level, ctx);
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
                                struct token_list  r3 = identifier_list(ctx, macro, input_list, level);
                                token_list_append_list(&r, &r3);
                                token_list_destroy(&r3);
                                skip_blanks_level(ctx, &r, input_list, level);
                                if (input_list->head == ((void *)0))
                                {
                                    macro_delete(macro);
                                    pre_unexpected_end_of_file(r.tail, ctx);
                                    goto _catch_label_1;
                                }
                                if (input_list->head->type == 11822)
                                {
                                    struct macro_parameter * p_macro_parameter = calloc(1, sizeof  *p_macro_parameter);
                                    if (p_macro_parameter == ((void *)0))
                                    {
                                        macro_delete(macro);
                                        goto _catch_label_1;
                                    }
                                    char * temp3 = strdup("__VA_ARGS__");
                                    if (temp3 == ((void *)0))
                                    {
                                        macro_delete(macro);
                                        macro_parameters_delete(p_macro_parameter);
                                        goto _catch_label_1;
                                    }
                                    p_macro_parameter->name = temp3;
                                    struct macro_parameter * p_last = macro->parameters;
                                    ;
                                    while (p_last->next)
                                    {
                                        p_last = p_last->next;
                                    }
                                    p_last->next = p_macro_parameter;
                                    match_token_level(&r, input_list, 11822, level, ctx);
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
                    if (input_list->head == ((void *)0))
                    {
                        macro_delete(macro);
                        pre_unexpected_end_of_file(r.tail, ctx);
                        goto _catch_label_1;
                    }
                    struct token_list  r4 = replacement_list(ctx, macro, input_list, level);
                    token_list_append_list(&r, &r4);
                    token_list_destroy(&r4);
                    match_token_level(&r, input_list, 10, level, ctx);
                    if ( !ctx->options.disable_assert && strcmp(macro->name, "assert") == 0)
                    {
                        if ( !is_empty_assert(&macro->replacement_list))
                        {
                            macro_parameters_delete(macro->parameters);
                            struct macro_parameter * p_macro_parameter = calloc(1, sizeof  *p_macro_parameter);
                            if (p_macro_parameter == ((void *)0))
                            {
                                macro_delete(macro);
                                goto _catch_label_1;
                            }
                            char * temp2 = strdup("__VA_ARGS__");
                            if (temp2 == ((void *)0))
                            {
                                macro_delete(macro);
                                macro_parameters_delete(p_macro_parameter);
                                goto _catch_label_1;
                            }
                            p_macro_parameter->name = temp2;
                            macro->parameters = p_macro_parameter;
                            token_list_destroy(&macro->replacement_list);
                            struct tokenizer_ctx  tctx = {0};
                            macro->replacement_list = tokenizer(&tctx, "assert(__VA_ARGS__)", ((void *)0), level, 0);
                        }
                    }
                    naming_convention_macro(ctx, macro_name_token);
                    struct hash_item_set  item = {0};
                    item.p_macro = macro;
                    hashmap_set(&ctx->macros, macro->name, &item);
                    hash_item_set_destroy(&item);
                }
                else
                {
                    if (strcmp(input_list->head->lexeme, "undef") == 0)
                    {
                        match_token_level(&r, input_list, 8996, level, ctx);
                        skip_blanks_level(ctx, &r, input_list, level);
                        if (input_list->head == ((void *)0))
                        {
                            pre_unexpected_end_of_file(r.tail, ctx);
                            goto _catch_label_1;
                        }
                        struct macro * macro = (struct macro *)hashmap_remove(&ctx->macros, input_list->head->lexeme, ((void *)0));
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
                            match_token_level(&r, input_list, 8996, level, ctx);
                            struct token_list  r5 = pp_tokens_opt(ctx, input_list, level);
                            token_list_append_list(&r, &r5);
                            token_list_destroy(&r5);
                            match_token_level(&r, input_list, 10, level, ctx);
                        }
                        else
                        {
                            if (strcmp(input_list->head->lexeme, "error") == 0)
                            {
                                ctx->n_warnings++;
                                match_token_level(&r, input_list, 8996, level, ctx);
                                struct token_list  r6 = pp_tokens_opt(ctx, input_list, level);
                                preprocessor_diagnostic_message(1180, ctx, input_list->head, "#error");
                                token_list_append_list(&r, &r6);
                                token_list_destroy(&r6);
                                match_token_level(&r, input_list, 10, level, ctx);
                            }
                            else
                            {
                                if (strcmp(input_list->head->lexeme, "warning") == 0)
                                {
                                    ctx->n_warnings++;
                                    match_token_level(&r, input_list, 8996, level, ctx);
                                    struct token_list  r6 = pp_tokens_opt(ctx, input_list, level);
                                    preprocessor_diagnostic_message(0, ctx, input_list->head, "#warning");
                                    token_list_append_list(&r, &r6);
                                    match_token_level(&r, input_list, 10, level, ctx);
                                    token_list_destroy(&r6);
                                }
                                else
                                {
                                    if (strcmp(input_list->head->lexeme, "pragma") == 0)
                                    {
                                        match_token_level(&r, input_list, 8996, level, ctx);
                                        if (r.tail)
                                        {
                                            r.tail->type = 128;
                                            r.tail->flags = 1;
                                        }
                                        skip_blanks_level(ctx, &r, input_list, level);
                                        if (input_list->head == ((void *)0))
                                        {
                                            pre_unexpected_end_of_file(r.tail, ctx);
                                            goto _catch_label_1;
                                        }
                                        if (input_list->head->type == 8996)
                                        {
                                            if (strcmp(input_list->head->lexeme, "CAKE") == 0)
                                            {
                                                match_token_level(&r, input_list, 8996, level, ctx);
                                                if (r.tail)
                                                {
                                                    r.tail->flags = 1;
                                                }
                                                skip_blanks_level(ctx, &r, input_list, level);
                                            }
                                            if (input_list->head == ((void *)0))
                                            {
                                                pre_unexpected_end_of_file(r.tail, ctx);
                                                goto _catch_label_1;
                                            }
                                            if (strcmp(input_list->head->lexeme, "once") == 0)
                                            {
                                                pragma_once_add(ctx, input_list->head->token_origin->lexeme);
                                                match_token_level(&r, input_list, 8996, level, ctx);
                                                if (r.tail)
                                                {
                                                    r.tail->flags = 1;
                                                }
                                            }
                                            else
                                            {
                                                if (strcmp(input_list->head->lexeme, "dir") == 0)
                                                {
                                                    match_token_level(&r, input_list, 8996, level, ctx);
                                                    skip_blanks_level(ctx, &r, input_list, level);
                                                    if (input_list->head == ((void *)0))
                                                    {
                                                        pre_unexpected_end_of_file(r.tail, ctx);
                                                        goto _catch_label_1;
                                                    }
                                                    if (input_list->head->type != 130)
                                                    {
                                                        preprocessor_diagnostic_message(650, ctx, input_list->head, "expected string");
                                                        goto _catch_label_1;
                                                    }
                                                    char path[200] = {0};
                                                    strncpy(path, input_list->head->lexeme + 1, strlen(input_list->head->lexeme) - 2);
                                                    include_dir_add(&ctx->include_dir, path);
                                                    match_token_level(&r, input_list, 130, level, ctx);
                                                    if (r.tail)
                                                    {
                                                        r.tail->flags = 1;
                                                    }
                                                }
                                                else
                                                {
                                                    if (strcmp(input_list->head->lexeme, "expand") == 0)
                                                    {
                                                        match_token_level(&r, input_list, 8996, level, ctx);
                                                        if (r.tail)
                                                        {
                                                            r.tail->flags = 1;
                                                        }
                                                        skip_blanks_level(ctx, &r, input_list, level);
                                                        if (input_list->head == ((void *)0))
                                                        {
                                                            pre_unexpected_end_of_file(r.tail, ctx);
                                                            goto _catch_label_1;
                                                        }
                                                        struct macro * macro = find_macro(ctx, input_list->head->lexeme);
                                                        if (macro)
                                                        {
                                                            macro->expand = 1;
                                                        }
                                                        match_token_level(&r, input_list, 8996, level, ctx);
                                                    }
                                                    else
                                                    {
                                                        if (strcmp(input_list->head->lexeme, "nullchecks") == 0)
                                                        {
                                                            ;
                                                            match_token_level(&r, input_list, 8996, level, ctx);
                                                            ;
                                                            r.tail->flags = 1;
                                                            skip_blanks_level(ctx, &r, input_list, level);
                                                            ctx->options.null_checks_enabled = 1;
                                                        }
                                                    }
                                                }
                                            }
                                            if (input_list->head == ((void *)0))
                                            {
                                                pre_unexpected_end_of_file(r.tail, ctx);
                                                goto _catch_label_1;
                                            }
                                            if (strcmp(input_list->head->lexeme, "diagnostic") == 0)
                                            {
                                                match_token_level(&r, input_list, 8996, level, ctx);
                                                ;
                                                r.tail->flags = 1;
                                                skip_blanks_level(ctx, &r, input_list, level);
                                                if (input_list->head == ((void *)0))
                                                {
                                                    pre_unexpected_end_of_file(r.tail, ctx);
                                                    goto _catch_label_1;
                                                }
                                                if (strcmp(input_list->head->lexeme, "push") == 0)
                                                {
                                                    match_token_level(&r, input_list, 8996, level, ctx);
                                                    ;
                                                    r.tail->flags = 1;
                                                    if (ctx->options.diagnostic_stack.top_index < sizeof (ctx->options.diagnostic_stack) / sizeof (ctx->options.diagnostic_stack.stack[0]))
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
                                                        r.tail->flags = 1;
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
                                                            r.tail->flags = 1;
                                                            skip_blanks_level(ctx, &r, input_list, level);
                                                            if (input_list->head && input_list->head->type == 130)
                                                            {
                                                                match_token_level(&r, input_list, 130, level, ctx);
                                                                ;
                                                                r.tail->flags = 1;
                                                                unsigned long long w = get_warning_bit_mask(input_list->head->lexeme + 1 + 2);
                                                                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings = w;
                                                            }
                                                        }
                                                        else
                                                        {
                                                            if (strcmp(input_list->head->lexeme, "ignore") == 0)
                                                            {
                                                                match_token_level(&r, input_list, 8996, level, ctx);
                                                                ;
                                                                r.tail->flags = 1;
                                                                skip_blanks_level(ctx, &r, input_list, level);
                                                                if (input_list->head && input_list->head->type == 130)
                                                                {
                                                                    unsigned long long w = get_warning_bit_mask(input_list->head->lexeme + 1 + 2);
                                                                    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings =  ~w;
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        struct token_list  r7 = pp_tokens_opt(ctx, input_list, level);
                                        token_list_append_list(&r, &r7);
                                        match_token_level(&r, input_list, 10, level, ctx);
                                        ;
                                        r.tail->type = 129;
                                        r.tail->flags = 1;
                                        token_list_destroy(&r7);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else _catch_label_1:
    {
    }
    return r;
}

static struct token_list non_directive(struct preprocessor_ctx * ctx, struct token_list * input_list, int level)
{
    struct token_list  r = pp_tokens_opt(ctx, input_list, level);
    skip_blanks_level(ctx, &r, input_list, level);
    match_token_level(&r, input_list, 10, level, ctx);
    return r;
}

static struct macro_argument_list collect_macro_arguments(struct preprocessor_ctx * ctx, struct macro * macro, struct token_list * input_list, int level)
{
    struct macro_argument_list  macro_argument_list = {0};
    if (1)
    {
        if (input_list->head == ((void *)0))
        {
            goto _catch_label_1;
        }
        ;
        struct token * macro_name_token = input_list->head;
        match_token_level(&macro_argument_list.tokens, input_list, 8996, level, ctx);
        if ( !macro->is_function)
        {
            return macro_argument_list;
        }
        int count = 1;
        skip_blanks(ctx, &macro_argument_list.tokens, input_list);
        match_token_level(&macro_argument_list.tokens, input_list, 40, level, ctx);
        skip_blanks(ctx, &macro_argument_list.tokens, input_list);
        if (input_list->head == ((void *)0))
        {
            pre_unexpected_end_of_file(macro_argument_list.tokens.tail, ctx);
            goto _catch_label_1;
        }
        if (input_list->head->type == 41)
        {
            if (macro->parameters != ((void *)0))
            {
                struct macro_argument * p_argument = calloc(1, sizeof (struct macro_argument));
                if (p_argument == ((void *)0))
                {
                    goto _catch_label_1;
                }
                struct macro_parameter * p_current_parameter = macro->parameters;
                char * name_temp = strdup(p_current_parameter->name);
                if (name_temp == ((void *)0))
                {
                    macro_argument_delete(p_argument);
                    goto _catch_label_1;
                }
                p_argument->name = name_temp;
                argument_list_add(&macro_argument_list, p_argument);
            }
            match_token_level(&macro_argument_list.tokens, input_list, 41, level, ctx);
            return macro_argument_list;
        }
        if (macro->parameters == ((void *)0))
        {
            preprocessor_diagnostic_message(1191, ctx, macro_name_token, "too many arguments provided to function-like macro invocation\n");
            goto _catch_label_1;
        }
        struct macro_parameter * p_current_parameter = macro->parameters;
        struct macro_argument * p_argument = calloc(1, sizeof (struct macro_argument));
        if (p_argument == ((void *)0))
        {
            goto _catch_label_1;
        }
        char * temp2 = strdup(p_current_parameter->name);
        if (temp2 == ((void *)0))
        {
            macro_argument_delete(p_argument);
            goto _catch_label_1;
        }
        p_argument->name = temp2;
        while (input_list->head != ((void *)0))
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
                        p_argument = ((void *)0);
                        if (p_current_parameter->next != ((void *)0))
                        {
                            p_current_parameter = p_current_parameter->next;
                            if (strcmp(p_current_parameter->name, "__VA_ARGS__") == 0)
                            {
                                p_argument = calloc(1, sizeof (struct macro_argument));
                                if (p_argument == ((void *)0))
                                {
                                    goto _catch_label_1;
                                }
                                char * argument_name = strdup(p_current_parameter->name);
                                if (argument_name == ((void *)0))
                                {
                                    macro_argument_delete(p_argument);
                                    goto _catch_label_1;
                                }
                                p_argument->name = argument_name;
                                argument_list_add(&macro_argument_list, p_argument);
                                p_argument = ((void *)0);
                            }
                            else
                            {
                                preprocessor_diagnostic_message(1190, ctx, macro_name_token, "too few arguments provided to function-like macro invocation\n");
                                goto _catch_label_1;
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
                            p_argument = ((void *)0);
                            p_argument = calloc(1, sizeof (struct macro_argument));
                            if (p_argument == ((void *)0))
                            {
                                goto _catch_label_1;
                            }
                            if (p_current_parameter->next == ((void *)0))
                            {
                                preprocessor_diagnostic_message(1191, ctx, macro_argument_list.tokens.tail, "too many arguments provided to function-like macro invocation\n");
                                macro_argument_delete(p_argument);
                                p_argument = ((void *)0);
                                goto _catch_label_1;
                            }
                            p_current_parameter = p_current_parameter->next;
                            char * temp3 = strdup(p_current_parameter->name);
                            if (temp3 == ((void *)0))
                            {
                                macro_argument_delete(p_argument);
                                goto _catch_label_1;
                            }
                            p_argument->name = temp3;
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
    else _catch_label_1:
    {
    }
    return macro_argument_list;
}

int ss_fprintf(struct osstream * stream, char * fmt, ...);
void ss_close(struct osstream * stream);
void token_list_pop_back(struct token_list * list);

static struct token_list concatenate(struct preprocessor_ctx * ctx, struct token_list * input_list)
{
    struct token_list  r = {0};
    if (1)
    {
        while (input_list->head)
        {
            ;
            if (input_list->head->type == 8995)
            {
                if (r.tail == ((void *)0))
                {
                    preprocessor_diagnostic_message(1210, ctx, input_list->head, "missing macro argument (should be checked before)");
                    break;
                }
                token_list_pop_front(input_list);
                struct osstream  ss = {0};
                if (r.tail->lexeme[0] != 0)
                {
                    ss_fprintf(&ss, "%s", r.tail->lexeme);
                }
                if (input_list->head && input_list->head->lexeme[0] != 0)
                {
                    ss_fprintf(&ss, "%s", input_list->head->lexeme);
                }
                int level = input_list->head ? input_list->head->level : 0;
                token_list_pop_front(input_list);
                struct tokenizer_ctx  tctx = {0};
                struct token_list  newlist = {0};
                if (ss.c_str != ((void *)0))
                {
                    newlist = tokenizer(&tctx, ss.c_str, ((void *)0), level, 0);
                }
                if (newlist.head)
                {
                    newlist.head->flags = r.tail->flags;
                }
                else
                {
                    struct token * p_new_token = calloc(1, sizeof  *p_new_token);
                    if (p_new_token == ((void *)0))
                    {
                        ss_close(&ss);
                        goto _catch_label_1;
                    }
                    char * temp = strdup("");
                    if (temp == ((void *)0))
                    {
                        ss_close(&ss);
                        token_delete(p_new_token);
                        goto _catch_label_1;
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
                if (input_list->head == ((void *)0))
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
    else _catch_label_1:
    {
    }
    return r;
}

struct token_list replacement_list_reexamination(struct preprocessor_ctx * ctx, struct macro_expanded * p_list, struct token_list * oldlist, int level, struct token * origin);

static unsigned char  has_argument_list_empty_substitution(struct preprocessor_ctx * ctx, struct macro_expanded * p_list, struct macro_argument_list * p_macro_argument_list, struct token * origin)
{
    if (p_macro_argument_list->head == ((void *)0))
    {
        return 1;
    }
    struct macro_argument * p_va_args_argument = find_macro_argument_by_name(p_macro_argument_list, "__VA_ARGS__");
    if (p_va_args_argument)
    {
        if (p_va_args_argument->tokens.head == ((void *)0))
        {
            return 1;
        }
        struct token_list  argumentlist = copy_argument_list(p_va_args_argument);
        struct token_list  r4 = replacement_list_reexamination(ctx, p_list, &argumentlist, 0, origin);
        unsigned char   results_in_empty_substituition = (r4.head == ((void *)0) || r4.head->type == 142);
        token_list_destroy(&r4);
        token_list_destroy(&argumentlist);
        return results_in_empty_substituition;
    }
    return 0;
}

void token_list_remove(struct token_list * list, struct token * first, struct token * last);
char *token_list_join_tokens(struct token_list * list, unsigned char   bliteral);

static struct token_list replace_macro_arguments(struct preprocessor_ctx * ctx, struct macro_expanded * p_list, struct token_list * input_list, struct macro_argument_list * arguments, struct token * origin)
{
    struct token_list  r = {0};
    if (1)
    {
        while (input_list->head)
        {
            ;
            ;
            ;
            struct macro_argument * p_argument = ((void *)0);
            if (input_list->head->type == 8996)
            {
                if (strcmp(input_list->head->lexeme, "__VA_OPT__") == 0)
                {
                    token_list_pop_front(input_list);
                    token_list_pop_front(input_list);
                    int parenteses_count = 1;
                    unsigned char   discard_va_opt = has_argument_list_empty_substitution(ctx, p_list, arguments, origin);
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
                        struct token * p_token = input_list->head;
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
                if (r.tail != ((void *)0) && r.tail->type == 35)
                {
                    int   flags = input_list->head->flags;
                    token_list_pop_front(input_list);
                    while (token_is_blank(r.tail))
                    {
                        token_list_pop_back(&r);
                    }
                    token_list_pop_back(&r);
                    struct token_list  argumentlist = copy_argument_list(p_argument);
                    char * s = token_list_join_tokens(&argumentlist, 1);
                    if (s == ((void *)0))
                    {
                        token_list_destroy(&argumentlist);
                        preprocessor_diagnostic_message(650, ctx, input_list->head, "unexpected");
                        goto _catch_label_1;
                    }
                    struct token * p_new_token = calloc(1, sizeof  *p_new_token);
                    if (p_new_token == ((void *)0))
                    {
                        free(s);
                        token_list_destroy(&argumentlist);
                        goto _catch_label_1;
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
                    if (r.tail != ((void *)0) && r.tail->type == 8995)
                    {
                        token_list_pop_front(input_list);
                        struct token_list  argumentlist = copy_argument_list(p_argument);
                        token_list_append_list(&r, &argumentlist);
                        token_list_destroy(&argumentlist);
                    }
                    else
                    {
                        if (input_list->head->next && input_list->head->next->type == 8995)
                        {
                            int flags = input_list->head->flags;
                            token_list_pop_front(input_list);
                            struct token_list  argumentlist = copy_argument_list(p_argument);
                            if (argumentlist.head != ((void *)0))
                            {
                                argumentlist.head->flags = flags;
                            }
                            token_list_append_list(&r, &argumentlist);
                            prematch(&r, input_list);
                            token_list_destroy(&argumentlist);
                        }
                        else
                        {
                            int flags = input_list->head->flags;
                            token_list_pop_front(input_list);
                            struct token_list  argumentlist = copy_argument_list(p_argument);
                            if (argumentlist.head)
                            {
                                argumentlist.head->flags = flags;
                            }
                            struct token_list  r4 = replacement_list_reexamination(ctx, p_list, &argumentlist, 0, origin);
                            token_list_append_list(&r, &r4);
                            token_list_destroy(&r4);
                            if (ctx->n_errors > 0)
                            {
                                token_list_destroy(&argumentlist);
                                goto _catch_label_1;
                            }
                            token_list_destroy(&argumentlist);
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
    else _catch_label_1:
    {
    }
    return r;
}

static unsigned char  macro_already_expanded(struct macro_expanded * p_list, char * name)
{
    struct macro_expanded * p_item = p_list;
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

struct token_list expand_macro(struct preprocessor_ctx * ctx, struct macro_expanded * p_list, struct macro * macro, struct macro_argument_list * arguments, int level, struct token * origin);
void token_list_append_list_at_beginning(struct token_list * dest, struct token_list * source);

struct token_list replacement_list_reexamination(struct preprocessor_ctx * ctx, struct macro_expanded * p_list, struct token_list * oldlist, int level, struct token * origin)
{
    struct token_list  r = {0};
    if (1)
    {
        struct token_list  new_list = concatenate(ctx, oldlist);
        while (new_list.head != ((void *)0))
        {
            ;
            ;
            struct macro * macro = ((void *)0);
            if (new_list.head->type == 8996)
            {
                macro = find_macro(ctx, new_list.head->lexeme);
                if (macro && macro->is_function &&  !preprocessor_token_ahead_is(new_list.head, 40))
                {
                    macro = ((void *)0);
                }
                if (macro && macro_already_expanded(p_list, new_list.head->lexeme))
                {
                    new_list.head->type = 8997;
                    macro = ((void *)0);
                }
                if (ctx->conditional_inclusion)
                {
                    if (r.tail && r.tail->type == 8996 && strcmp(r.tail->lexeme, "defined") == 0)
                    {
                        macro = ((void *)0);
                    }
                    else
                    {
                        if (r.tail && r.tail->type == 40)
                        {
                            struct token * previous = r.tail->prev;
                            if (previous != ((void *)0) && previous->type == 8996 && strcmp(previous->lexeme, "defined") == 0)
                            {
                                macro = ((void *)0);
                            }
                        }
                    }
                }
            }
            if (macro)
            {
                int flags = new_list.head->flags;
                struct macro_argument_list  arguments = collect_macro_arguments(ctx, macro, &new_list, level);
                if (ctx->n_errors > 0)
                {
                    macro_argument_list_destroy(&arguments);
                    token_list_destroy(&new_list);
                    goto _catch_label_1;
                }
                struct token_list  r3 = expand_macro(ctx, p_list, macro, &arguments, level, origin);
                if (ctx->n_errors > 0)
                {
                    token_list_destroy(&new_list);
                    token_list_destroy(&r3);
                    macro_argument_list_destroy(&arguments);
                    goto _catch_label_1;
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
                new_list.head->flags = 2;
                ;
                prematch(&r, &new_list);
            }
        }
    }
    else _catch_label_1:
    {
    }
    return r;
}

int lexeme_cmp(char * s1, char * s2)
{
    while ( *s1 &&  *s2)
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
        if ( *s1 !=  *s2)
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
    return  *(unsigned char *)s1 -  *(unsigned char *)s2;
}

void remove_line_continuation(char * s)
{
    char * pread = s;
    char * pwrite = s;
    while ( *pread)
    {
        if (pread[0] == 92 && pread[1] == 10)
        {
            pread++;
            pread++;
        }
        else
        {
             *pwrite =  *pread;
            pread++;
            pwrite++;
        }
    }
     *pwrite =  *pread;
}

struct token_list copy_replacement_list(struct token_list * list)
{
    struct token_list  r = {0};
    struct token * current = list->head;
    while (current && token_is_blank(current))
    {
        current = current->next;
    }
    unsigned char   is_first = 1;
    for (; current; )
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
        struct token * token_added = token_list_clone_and_add(&r, current);
        if (token_added->flags & 8)
        {
            token_added->flags = token_added->flags &  ~8;
            token_added->flags = 4;
        }
        if (is_first)
        {
            token_added->flags = token_added->flags &  ~4;
            token_added->flags = token_added->flags &  ~8;
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

int stringify(char * input, int n, char output[]);

struct token_list macro_copy_replacement_list(struct preprocessor_ctx * ctx, struct macro * macro, struct token * origin)
{
    if (strcmp(macro->name, "__LINE__") == 0)
    {
        struct tokenizer_ctx  tctx = {0};
        char line[50] = {0};
        ;
        snprintf(line, sizeof line, "%d", origin->line);
        struct token_list  r = tokenizer(&tctx, line, "", 0, 0);
        token_list_pop_front(&r);
        if (r.head != ((void *)0))
        {
            r.head->flags = 0;
        }
        return r;
    }
    else
    {
        if (strcmp(macro->name, "__FILE__") == 0)
        {
            char buffer[300] = {0};
            if (stringify(origin->token_origin->lexeme, sizeof buffer, buffer) < 0)
            {
            }
            struct tokenizer_ctx  tctx = {0};
            struct token_list  r = tokenizer(&tctx, buffer, "", 0, 0);
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
                char line[50] = {0};
                ctx->count_macro_value++;
                snprintf(line, sizeof line, "%d", ctx->count_macro_value);
                struct tokenizer_ctx  tctx = {0};
                struct token_list  r = tokenizer(&tctx, line, "", 0, 0);
                token_list_pop_front(&r);
                if (r.head != ((void *)0))
                {
                    r.head->flags = 0;
                }
                return r;
            }
        }
    }
    return copy_replacement_list(&macro->replacement_list);
}

struct token_list expand_macro(struct preprocessor_ctx * ctx, struct macro_expanded * p_list_of_macro_expanded_opt, struct macro * macro, struct macro_argument_list * arguments, int level, struct token * origin)
{
    macro->usage++;
    struct token_list  r = {0};
    if (1)
    {
        ;
        struct macro_expanded  macro_expanded = {0};
        macro_expanded.name = macro->name;
        macro_expanded.p_previous = p_list_of_macro_expanded_opt;
        if (macro->is_function)
        {
            struct token_list  copy = macro_copy_replacement_list(ctx, macro, origin);
            struct token_list  copy2 = replace_macro_arguments(ctx, &macro_expanded, &copy, arguments, origin);
            struct token_list  r2 = replacement_list_reexamination(ctx, &macro_expanded, &copy2, level, origin);
            token_list_append_list(&r, &r2);
            token_list_destroy(&copy);
            token_list_destroy(&copy2);
            token_list_destroy(&r2);
            if (ctx->n_errors > 0)
            {
                goto _catch_label_1;
            }
        }
        else
        {
            struct token_list  copy = macro_copy_replacement_list(ctx, macro, origin);
            struct token_list  r3 = replacement_list_reexamination(ctx, &macro_expanded, &copy, level, origin);
            if (ctx->n_errors > 0)
            {
                token_list_destroy(&copy);
                token_list_destroy(&r3);
                goto _catch_label_1;
            }
            token_list_append_list(&r, &r3);
            token_list_destroy(&copy);
            token_list_destroy(&r3);
        }
    }
    else _catch_label_1:
    {
    }
    return r;
}

void token_list_set_file(struct token_list * list, struct token * filetoken, int line, int col);

static struct token_list text_line(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char   is_active, int level)
{
    struct token_list  r = {0};
    if (1)
    {
        while (input_list->head && input_list->head->type != 127)
        {
            struct macro * macro = ((void *)0);
            struct token * start_token = input_list->head;
            struct token * origin = ((void *)0);
            if (is_active && input_list->head->type == 8996)
            {
                origin = input_list->head;
                macro = find_macro(ctx, input_list->head->lexeme);
                if (macro && macro->is_function &&  !preprocessor_token_ahead_is(input_list->head, 40))
                {
                    macro = ((void *)0);
                }
                if (ctx->conditional_inclusion)
                {
                    if (r.tail && r.tail->type == 8996 && strcmp(r.tail->lexeme, "defined") == 0)
                    {
                        macro = ((void *)0);
                    }
                    else
                    {
                        if (r.tail && r.tail->type == 40)
                        {
                            struct token * previous = r.tail->prev;
                            if (previous != ((void *)0) && previous->type == 8996 && strcmp(previous->lexeme, "defined") == 0)
                            {
                                macro = ((void *)0);
                            }
                        }
                    }
                }
            }
            if (macro)
            {
                int   flags = input_list->head->flags;
                struct macro_argument_list  arguments = collect_macro_arguments(ctx, macro, input_list, level);
                if (ctx->n_errors > 0)
                {
                    macro_argument_list_destroy(&arguments);
                    goto _catch_label_1;
                }
                struct token_list  start_macro = expand_macro(ctx, ((void *)0), macro, &arguments, level, origin);
                if (start_macro.head)
                {
                    start_macro.head->flags = flags;
                }
                if (macro->expand)
                {
                    {
                        struct token * current = arguments.tokens.head;
                        for (; current && current != arguments.tokens.tail->next; current = current->next)
                        {
                            current->flags = 64;
                        }
                    }
                    {
                        struct token * current = start_macro.head;
                        for (; current && current != start_macro.tail->next; current = current->next)
                        {
                            current->flags =  ~(2 | 512 | 1024);
                        }
                    }
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
                    macro = ((void *)0);
                    if (input_list->head && input_list->head->type == 8996)
                    {
                        macro = find_macro(ctx, input_list->head->lexeme);
                        if (macro && macro->is_function &&  !preprocessor_token_ahead_is(input_list->head, 40))
                        {
                            macro = ((void *)0);
                        }
                        if (macro)
                        {
                            int flags2 = input_list->head->flags;
                            struct macro_argument_list  arguments2 = collect_macro_arguments(ctx, macro, input_list, level);
                            if (ctx->n_errors > 0)
                            {
                                macro_argument_list_destroy(&arguments2);
                                macro_argument_list_destroy(&arguments);
                                token_list_destroy(&start_macro);
                                goto _catch_label_1;
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
                            struct token_list  r3 = expand_macro(ctx, ((void *)0), macro, &arguments2, level, origin);
                            if (ctx->n_errors > 0)
                            {
                                macro_argument_list_destroy(&arguments2);
                                token_list_destroy(&r3);
                                macro_argument_list_destroy(&arguments);
                                token_list_destroy(&start_macro);
                                goto _catch_label_1;
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
                if (input_list->head->flags & 1024 &&  !(input_list->head->flags & 2))
                {
                    if (input_list->head->type == 130)
                    {
                        preprocessor_diagnostic_message(63, ctx, input_list->head, "you can use \"adjacent\" \"strings\"");
                    }
                    else
                    {
                        if (input_list->head->type == 132)
                        {
                            preprocessor_diagnostic_message(11, ctx, input_list->head, "multi-line //comment");
                        }
                        else
                        {
                            preprocessor_diagnostic_message(12, ctx, input_list->head, "unnecessary line-slicing");
                        }
                    }
                }
                unsigned char   blanks = token_is_blank(input_list->head) || input_list->head->type == 10;
                unsigned char   is_final = is_active &&  !is_never_final(input_list->head->type);
                if (ctx->flags & 1)
                {
                    if (is_final)
                    {
                        prematch(&r, input_list);
                        ;
                        r.tail->flags = 1;
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
                                r.tail->flags = 1;
                            }
                        }
                        else
                        {
                            if (is_final)
                            {
                                prematch(&r, input_list);
                                ;
                                r.tail->flags = 1;
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
    else _catch_label_1:
    {
    }
    return r;
}

struct token_list group_part(struct preprocessor_ctx * ctx, struct token_list * input_list, unsigned char   is_active, int level)
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
            if (preprocessor_token_ahead_is_identifier(input_list->head, "include") || preprocessor_token_ahead_is_identifier(input_list->head, "embed") || preprocessor_token_ahead_is_identifier(input_list->head, "define") || preprocessor_token_ahead_is_identifier(input_list->head, "undef") || preprocessor_token_ahead_is_identifier(input_list->head, "warning") || preprocessor_token_ahead_is_identifier(input_list->head, "line") || preprocessor_token_ahead_is_identifier(input_list->head, "error") || preprocessor_token_ahead_is_identifier(input_list->head, "pragma"))
            {
                return control_line(ctx, input_list, is_active, level);
            }
            else
            {
                return non_directive(ctx, input_list, level);
            }
        }
    }
    return text_line(ctx, input_list, is_active, level);
}

struct token_list preprocessor(struct preprocessor_ctx * ctx, struct token_list * input_list, int level)
{
    struct token_list  r = {0};
    if (input_list->head == ((void *)0))
    {
        return r;
    }
    if (input_list->head->type == 8998)
    {
        prematch_level(&r, input_list, 1);
    }
    struct token_list  g = group_opt(ctx, input_list, 1, level);
    token_list_append_list(&r, &g);
    token_list_destroy(&g);
    return r;
}

static void mark_macros_as_used(struct hash_map * map)
{
    if (map->table != ((void *)0))
    {
        {
            int i = 0;
            for (; i < map->capacity; i++)
            {
                struct map_entry * pentry = map->table[i];
                while (pentry != ((void *)0))
                {
                    ;
                    struct macro * macro = pentry->data.p_macro;
                    macro->usage = 1;
                    pentry = pentry->next;
                }
            }
        }
    }
}

void check_unused_macros(struct hash_map * map)
{
    if (map->table != ((void *)0))
    {
        {
            int i = 0;
            for (; i < map->capacity; i++)
            {
                struct map_entry * pentry = map->table[i];
                while (pentry != ((void *)0))
                {
                    ;
                    struct macro * macro = pentry->data.p_macro;
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

int include_config_header(struct preprocessor_ctx * ctx, char * file_name)
{
    char local_cakeconfig_path[260] = {0};
    snprintf(local_cakeconfig_path, sizeof local_cakeconfig_path, "%s", file_name);
    dirname(local_cakeconfig_path);
    snprintf(local_cakeconfig_path, sizeof local_cakeconfig_path, "%s", local_cakeconfig_path);
    char * str = read_file(local_cakeconfig_path, 1);
    while (str == ((void *)0))
    {
        dirname(local_cakeconfig_path);
        dirname(local_cakeconfig_path);
        if (local_cakeconfig_path[0] == 0)
        {
            break;
        }
        str = read_file(local_cakeconfig_path, 1);
    }
    if (str == ((void *)0))
    {
        char executable_path[246] = {0};
        get_self_path(executable_path, sizeof (executable_path));
        dirname(executable_path);
        char root_cakeconfig_path[260] = {0};
        snprintf(root_cakeconfig_path, sizeof root_cakeconfig_path, "%s", executable_path);
        str = read_file(root_cakeconfig_path, 1);
    }
    if (str == ((void *)0))
    {
        return 2;
    }
    int   w = ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings;
    struct tokenizer_ctx  tctx = {0};
    struct token_list  l = tokenizer(&tctx, str, "standard macros inclusion", 0, 0);
    struct token_list  l10 = preprocessor(ctx, &l, 0);
    mark_macros_as_used(&ctx->macros);
    token_list_destroy(&l);
    free(str);
    token_list_destroy(&l10);
    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings = w;
    return 0;
}


__int64 _time64(__int64 * _Time);
inline static __int64 time(__int64 * _Time)
{
    return _time64(_Time);
}

struct tm *_localtime64(__int64 * _Time);
inline static struct tm *localtime(__int64 * _Time)
{
    return _localtime64(_Time);
}

void add_standard_macros(struct preprocessor_ctx * ctx)
{
    struct diagnostic  w = ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index];
    struct diagnostic  __cmp_lt_0 = {0, 0, 0};
    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index] = __cmp_lt_0;
    static char mon[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    __int64 now = time(((void *)0));
    struct tm * tm = localtime(&now);
    struct tokenizer_ctx  tctx = {0};
    char datastr[100] = {0};
    snprintf(datastr, sizeof datastr, "#define __DATE__ \"%s %2d %d\"\n", mon[tm->tm_mon], tm->tm_mday, tm->tm_year + 1900);
    struct token_list  l1 = tokenizer(&tctx, datastr, "__DATE__ macro inclusion", 0, 0);
    struct token_list  tl1 = preprocessor(ctx, &l1, 0);
    token_list_destroy(&tl1);
    token_list_destroy(&l1);
    char timestr[100] = {0};
    snprintf(timestr, sizeof timestr, "#define __TIME__ \"%02d:%02d:%02d\"\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
    struct token_list  l2 = tokenizer(&tctx, timestr, "__TIME__ macro inclusion", 0, 0);
    struct token_list  tl2 = preprocessor(ctx, &l2, 0);
    token_list_destroy(&tl2);
    token_list_destroy(&l2);
    char * pre_defined_macros_text = "#define __CAKE__ 202311L\n";
    "\n";
    struct token_list  l = tokenizer(&tctx, pre_defined_macros_text, "standard macros inclusion", 0, 0);
    struct token_list  l10 = preprocessor(ctx, &l, 0);
    mark_macros_as_used(&ctx->macros);
    token_list_destroy(&l);
    token_list_destroy(&l10);
    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index] = w;
}

char *get_token_name(int   tk)
{
    switch (tk)
    {
        case 0 :
        return "TK_NONE";
        case 10 :
        return "TK_NEWLINE";
        case 32 :
        return "TK_WHITE_SPACE";
        case 33 :
        return "TK_EXCLAMATION_MARK";
        case 34 :
        return "TK_QUOTATION_MARK";
        case 35 :
        return "TK_NUMBER_SIGN";
        case 36 :
        return "TK_DOLLAR_SIGN";
        case 37 :
        return "TK_PERCENT_SIGN";
        case 38 :
        return "TK_AMPERSAND";
        case 39 :
        return "TK_APOSTROPHE";
        case 40 :
        return "TK_LEFT_PARENTHESIS";
        case 41 :
        return "TK_RIGHT_PARENTHESIS";
        case 42 :
        return "TK_ASTERISK";
        case 43 :
        return "TK_PLUS_SIGN";
        case 44 :
        return "TK_COMMA";
        case 45 :
        return "TK_HYPHEN_MINUS";
        case 46 :
        return "TK_FULL_STOP";
        case 47 :
        return "TK_SOLIDUS";
        case 58 :
        return "TK_COLON";
        case 59 :
        return "TK_SEMICOLON";
        case 60 :
        return "TK_LESS_THAN_SIGN";
        case 61 :
        return "TK_EQUALS_SIGN";
        case 62 :
        return "TK_GREATER_THAN_SIGN";
        case 63 :
        return "TK_QUESTION_MARK";
        case 64 :
        return "TK_COMMERCIAL_AT";
        case 91 :
        return "TK_LEFT_SQUARE_BRACKET";
        case 12079 :
        return "TK_REVERSE_SOLIDUS";
        case 93 :
        return "TK_RIGHT_SQUARE_BRACKET";
        case 94 :
        return "TK_CIRCUMFLEX_ACCENT";
        case 95 :
        return "TK_FLOW_LINE";
        case 96 :
        return "TK_GRAVE_ACCENT";
        case 123 :
        return "TK_LEFT_CURLY_BRACKET";
        case 124 :
        return "TK_VERTICAL_LINE";
        case 125 :
        return "TK_RIGHT_CURLY_BRACKET";
        case 126 :
        return "TK_TILDE";
        case 127 :
        return "TK_PREPROCESSOR_LINE";
        case 128 :
        return "TK_PRAGMA";
        case 130 :
        return "TK_STRING_LITERAL";
        case 131 :
        return "TK_CHAR_CONSTANT";
        case 132 :
        return "TK_LINE_COMMENT";
        case 133 :
        return "TK_COMMENT";
        case 134 :
        return "TK_PPNUMBER";
        case 135 :
        return "ANY_OTHER_PP_TOKEN";
        case 136 :
        return "TK_COMPILER_DECIMAL_CONSTANT";
        case 137 :
        return "TK_COMPILER_OCTAL_CONSTANT";
        case 138 :
        return "TK_COMPILER_HEXADECIMAL_CONSTANT";
        case 139 :
        return "TK_COMPILER_BINARY_CONSTANT";
        case 140 :
        return "TK_COMPILER_DECIMAL_FLOATING_CONSTANT";
        case 141 :
        return "TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT";
        case 142 :
        return "TK_PLACEMARKER";
        case 143 :
        return "TK_BLANKS";
        case 11051 :
        return "TK_PLUSPLUS";
        case 11565 :
        return "TK_MINUSMINUS";
        case 11582 :
        return "TK_ARROW";
        case 15420 :
        return "TK_SHIFTLEFT";
        case 15934 :
        return "TK_SHIFTRIGHT";
        case 31868 :
        return "TK_LOGICAL_OPERATOR_OR";
        case 9766 :
        return "TK_LOGICAL_OPERATOR_AND";
        case 8995 :
        return "TK_MACRO_CONCATENATE_OPERATOR";
        case 8996 :
        return "TK_IDENTIFIER";
        case 8997 :
        return "TK_IDENTIFIER_RECURSIVE_MACRO";
        case 8998 :
        return "TK_BEGIN_OF_FILE";
        case 8999 :
        return "TK_KEYWORD_AUTO";
        case 9000 :
        return "TK_KEYWORD_BREAK";
        case 9001 :
        return "TK_KEYWORD_CASE";
        case 9002 :
        return "TK_KEYWORD_CONSTEXPR";
        case 9003 :
        return "TK_KEYWORD_CHAR";
        case 9004 :
        return "TK_KEYWORD_CONST";
        case 9005 :
        return "TK_KEYWORD_CONTINUE";
        case 9006 :
        return "TK_KEYWORD_CATCH";
        case 9007 :
        return "TK_KEYWORD_DEFAULT";
        case 9008 :
        return "TK_KEYWORD_DO";
        case 9009 :
        return "TK_KEYWORD_DEFER";
        case 9010 :
        return "TK_KEYWORD_DOUBLE";
        case 9011 :
        return "TK_KEYWORD_ELSE";
        case 9012 :
        return "TK_KEYWORD_ENUM";
        case 9013 :
        return "TK_KEYWORD_EXTERN";
        case 9014 :
        return "TK_KEYWORD_FLOAT";
        case 9015 :
        return "TK_KEYWORD_FOR";
        case 9016 :
        return "TK_KEYWORD_GOTO";
        case 9017 :
        return "TK_KEYWORD_IF";
        case 9018 :
        return "TK_KEYWORD_INLINE";
        case 9019 :
        return "TK_KEYWORD_INT";
        case 9020 :
        return "TK_KEYWORD_LONG";
        case 9021 :
        return "TK_KEYWORD__INT8";
        case 9022 :
        return "TK_KEYWORD__INT16";
        case 9023 :
        return "TK_KEYWORD__INT32";
        case 9024 :
        return "TK_KEYWORD__INT64";
        case 9025 :
        return "TK_KEYWORD_REGISTER";
        case 9026 :
        return "TK_KEYWORD_RESTRICT";
        case 9027 :
        return "TK_KEYWORD_RETURN";
        case 9028 :
        return "TK_KEYWORD_SHORT";
        case 9029 :
        return "TK_KEYWORD_SIGNED";
        case 9030 :
        return "TK_KEYWORD_SIZEOF";
        case 9032 :
        return "TK_KEYWORD_STATIC";
        case 9033 :
        return "TK_KEYWORD_STRUCT";
        case 9034 :
        return "TK_KEYWORD_SWITCH";
        case 9035 :
        return "TK_KEYWORD_TYPEDEF";
        case 9036 :
        return "TK_KEYWORD_TRY";
        case 9037 :
        return "TK_KEYWORD_THROW";
        case 9038 :
        return "TK_KEYWORD_UNION";
        case 9039 :
        return "TK_KEYWORD_UNSIGNED";
        case 9040 :
        return "TK_KEYWORD_VOID";
        case 9041 :
        return "TK_KEYWORD_VOLATILE";
        case 9042 :
        return "TK_KEYWORD_WHILE";
        case 9043 :
        return "TK_KEYWORD__ALIGNAS";
        case 9044 :
        return "TK_KEYWORD__ALIGNOF";
        case 9045 :
        return "TK_KEYWORD__ATOMIC";
        case 9046 :
        return "TK_KEYWORD__ASM";
        case 9047 :
        return "TK_KEYWORD__BOOL";
        case 9048 :
        return "TK_KEYWORD__COMPLEX";
        case 9049 :
        return "TK_KEYWORD__DECIMAL128";
        case 9050 :
        return "TK_KEYWORD__DECIMAL32";
        case 9051 :
        return "TK_KEYWORD__DECIMAL64";
        case 9052 :
        return "TK_KEYWORD__GENERIC";
        case 9053 :
        return "TK_KEYWORD__IMAGINARY";
        case 9054 :
        return "TK_KEYWORD__NORETURN";
        case 9055 :
        return "TK_KEYWORD__STATIC_ASSERT";
        case 9056 :
        return "TK_KEYWORD_ASSERT";
        case 9057 :
        return "TK_KEYWORD__THREAD_LOCAL";
        case 9058 :
        return "TK_KEYWORD_TYPEOF";
        case 9059 :
        return "TK_KEYWORD_TRUE";
        case 9060 :
        return "TK_KEYWORD_FALSE";
        case 9061 :
        return "TK_KEYWORD_NULLPTR";
        case 9062 :
        return "TK_KEYWORD_TYPEOF_UNQUAL";
        case 9063 :
        return "TK_KEYWORD__BITINT";
        case 9064 :
        return "TK_KEYWORD__OWNER";
        case 9065 :
        return "TK_KEYWORD__OUT";
        case 9066 :
        return "TK_KEYWORD__OBJ_OWNER";
        case 9067 :
        return "TK_KEYWORD__VIEW";
        case 9068 :
        return "TK_KEYWORD__OPT";
        case 9069 :
        return "TK_KEYWORD_STATIC_DEBUG";
        case 9070 :
        return "TK_KEYWORD_STATIC_DEBUG_EX";
        case 9071 :
        return "TK_KEYWORD_STATIC_STATE";
        case 9072 :
        return "TK_KEYWORD_STATIC_SET";
        case 9073 :
        return "TK_KEYWORD_IS_POINTER";
        case 9074 :
        return "TK_KEYWORD_IS_LVALUE";
        case 9075 :
        return "TK_KEYWORD_IS_CONST";
        case 9076 :
        return "TK_KEYWORD_IS_OWNER";
        case 9077 :
        return "TK_KEYWORD_IS_ARRAY";
        case 9078 :
        return "TK_KEYWORD_IS_FUNCTION";
        case 9079 :
        return "TK_KEYWORD_IS_SCALAR";
        case 9080 :
        return "TK_KEYWORD_IS_ARITHMETIC";
        case 9081 :
        return "TK_KEYWORD_IS_FLOATING_POINT";
        case 9082 :
        return "TK_KEYWORD_IS_INTEGRAL";
        case 129 :
        return "TK_PRAGMA_END";
        case 9031 :
        return "TK_KEYWORD__LENGTHOF";
    }
    return "TK_X_MISSING_NAME";
}

int stringify(char * input, int n, char output[])
{
    int count = 0;
    if (count < n)
    {
        output[count++] = 34;
    }
    char * p = input;
    while ( *p)
    {
        if ( *p == 34 ||  *p == 92)
        {
            if (count < n)
            {
                output[count++] = 92;
            }
            if (count < n)
            {
                output[count++] =  *p;
            }
            p++;
        }
        else
        {
            if (count < n)
            {
                output[count++] =  *p;
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
        return  -count;
    }
    return count;
}

void print_literal(char * s)
{
    if (s == ((void *)0))
    {
        printf("\"");
        printf("\"");
        return;
    }
    printf("\"");
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
    printf("\"");
}

char *get_code_as_we_see_plus_macros(struct token_list * list)
{
    struct osstream  ss = {0};
    struct token * current = list->head;
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
            ss_fprintf(&ss, "\x1b");
        }
        current = current->next;
    }
    char * cstr = ss.c_str;
    ss.c_str = ((void *)0);
    ss_close(&ss);
    return cstr;
}

void print_code_as_we_see(struct token_list * list, unsigned char   remove_comments)
{
    if (list->head == ((void *)0) || list->tail == ((void *)0))
    {
        return;
    }
    struct token * current = list->head;
    while (current && current != list->tail->next)
    {
        if (current->level == 0 &&  !(current->flags & 2) &&  !(current->flags & 64) && current->type != 8998)
        {
            if ((current->flags & 4) && (current->prev != ((void *)0) && current->prev->type != 143))
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

char *get_code_as_we_see(struct token_list * list, unsigned char   remove_comments)
{
    if (list->head == ((void *)0) || list->tail == ((void *)0))
    {
        return ((void *)0);
    }
    struct osstream  ss = {0};
    struct token * current = list->head;
    while (current && current != list->tail->next)
    {
        if (current->level == 0 &&  !(current->flags & 2) &&  !(current->flags & 64) && current->type != 8998)
        {
            if ((current->flags & 4) && (current->prev != ((void *)0) && current->prev->type != 143))
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
    char * cstr = ss.c_str;
    ss.c_str = ((void *)0);
    ss_close(&ss);
    return cstr;
}

char *get_code_as_compiler_see(struct token_list * list)
{
    if (list->head == ((void *)0) || list->tail == ((void *)0))
    {
        return ((void *)0);
    }
    struct osstream  ss = {0};
    struct token * current = list->head;
    while (current && current != list->tail->next)
    {
        if ( !(current->flags & 64) && current->type != 8998 && (current->flags & 1))
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
    if (p_token == ((void *)0))
    {
        return strdup("(null)");
    }
    struct osstream  ss = {0};
    struct token * current = p_token;
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
    struct osstream  ss = {0};
    struct token * current = p_token;
    while ( !(current->flags & 1) || current->type == 143 || current->type == 133 || current->type == 132 || current->type == 10 || current->type == 127)
    {
        current = current->next;
        if (current == ((void *)0))
        {
            return ss.c_str;
        }
    }
    unsigned char   first = 1;
    while (current)
    {
        ;
        if (current->flags & 1)
        {
            if ( !first && current->flags & 8)
            {
                ss_fprintf(&ss, "\n");
            }
            else
            {
                if ( !first && current->flags & 4)
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
    char * s = print_preprocessed_to_string(p_token);
    if (s)
    {
        printf("%s", s);
        free((void *)s);
    }
}

static unsigned char  is_screaming_case(char * text)
{
    unsigned char   screaming_case = 0;
    while ( *text)
    {
        if (( *text >= 65 &&  *text <= 90) || ( *text >= 48 &&  *text <= 57) || ( *text == 95))
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

void print_all_macros(struct preprocessor_ctx * prectx)
{
    {
        int i = 0;
        for (; i < prectx->macros.capacity; i++)
        {
            struct map_entry * entry = prectx->macros.table[i];
            if (entry == ((void *)0))
            {
                continue;
            }
            ;
            struct macro * macro = entry->data.p_macro;
            printf("#define %s", macro->name);
            if (macro->is_function)
            {
                printf("(");
                struct macro_parameter * parameter = macro->parameters;
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
            struct token * token = macro->replacement_list.head;
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

void naming_convention_macro(struct preprocessor_ctx * ctx, struct token * token)
{
    if ( !is_screaming_case(token->lexeme))
    {
        preprocessor_diagnostic_message(63, ctx, token, "use SCREAMING_CASE for macros");
    }
}

