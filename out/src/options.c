struct _iobuf {
    void * _Placeholder;
};

struct w {
    int   w;
    char * name;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct __crt_multibyte_data;
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

struct __crt_locale_data;

unsigned char  is_diagnostic_note(int   id)
{
    if (id == 63 || id == 62)
    {
        return 1;
    }
    return 0;
}

unsigned char  is_diagnostic_warning(int   id)
{
    return id > 63 && id <= 640;
}

unsigned char  is_diagnostic_error(int   id)
{
    return id >= 640;
}

unsigned char  is_diagnostic_configurable(int   id)
{
    return id >= 0 && id < 62;
}

int diagnostic_stack_push_empty(struct diagnostic_stack * diagnostic_stack)
{
    int index = diagnostic_stack->top_index;
    diagnostic_stack->top_index++;
    diagnostic_stack->stack[diagnostic_stack->top_index].warnings = 0;
    diagnostic_stack->stack[diagnostic_stack->top_index].errors = 0;
    diagnostic_stack->stack[diagnostic_stack->top_index].notes = 0;
    return index;
}

void diagnostic_stack_pop(struct diagnostic_stack * diagnostic_stack)
{
    if (diagnostic_stack->top_index > 0)
    {
        diagnostic_stack->top_index--;
    }
    else
    {
        ;
    }
}

struct diagnostic  default_diagnostic = {0, 4294966237ULL, 0};
static struct w s_warnings[53] = {1, "unused-variable", 2, "deprecated", 3, "enum-conversion", 4, "address", 5, "unused-parameter", 6, "hide-declarator", 7, "typeof-parameter", 8, "attributes", 9, "unused-value", 10, "style", 11, "comment", 12, "line-slicing", 44, "switch", 45, "unusual-null", 14, "discarded-qualifiers", 16, "uninitialized", 17, "return-local-addr", 36, "div-by-zero", 37, "constant-value", 46, "sizeof-array-argument", 13, "string-slicing", 15, "declarator-state", 20, "missing-owner-qualifier", 21, "not-owner", 22, "temp-owner", 23, "non-owner-move", 24, "non-owner-to-owner-move", 25, "discard-owner", 26, "non-owner-move", 35, "flow-div-by-zero", 27, "flow-not-null", 28, "missing-destructor", 31, "using-moved-object", 29, "analyzer-maybe-uninitialized", 32, "analyzer-null-dereference", 33, "analyzer-non-opt-arg", 30, "lifetime-ended", 34, "nullable-to-non-nullable", 18, "must-use-address-of", 38, "null-as-array", 39, "incompatible-enum", 40, "multi-char", 19, "array-indirection", 41, "out-of-bounds", 42, "array-parameter-assignment", 43, "conditional-constant", 47, "const-init", 48, "null-conversion", 49, "implicitly-unsigned-literal", 50, "overflow", 51, "array-size", 52, "empty-statement", 53, "incompatible-types"};
void diagnostic_remove(struct diagnostic * d, int   w)
{
    if ( !is_diagnostic_configurable(w))
    {
        return;
    }
    if ((d->errors & (1ULL << w)) != 0)
    {
        d->errors =  ~(1ULL << w);
    }
    if ((d->warnings & (1ULL << w)) != 0)
    {
        d->warnings =  ~(1ULL << w);
    }
    if ((d->notes & (1ULL << w)) != 0)
    {
        d->notes =  ~(1ULL << w);
    }
}

int get_diagnostic_type(struct diagnostic * d, int   w)
{
    if (is_diagnostic_configurable(w))
    {
        if ((d->errors & (1ULL << w)) != 0)
        {
            return 3;
        }
        if ((d->warnings & (1ULL << w)) != 0)
        {
            return 2;
        }
        if ((d->notes & (1ULL << w)) != 0)
        {
            return 1;
        }
    }
    if (is_diagnostic_note(w))
    {
        return 1;
    }
    if (is_diagnostic_warning(w))
    {
        return 2;
    }
    if (is_diagnostic_error(w))
    {
        return 3;
    }
    return 3;
}

int get_diagnostic_phase(int   w)
{
    switch (w)
    {
        case 28 :
        case 29 :
        case 31 :
        case 32 :
        case 33 :
        case 27 :
        case 30 :
        case 35 :
        return 2;
        default:
        break;
    }
    return 0;
}

int strncmp(char * _Str1, char * _Str2, unsigned int _MaxCount);
unsigned int strlen(char * _Str);
int atoi(char * _String);

int  get_warning(char * wname)
{
    if ( !(wname[0] == 45 || wname[0] == 69))
    {
        return 0;
    }
    if (wname[0] == 45 && wname[1] == 87)
    {
        {
            int j = 0;
            for (; j < sizeof (s_warnings) / sizeof (s_warnings[0]); j++)
            {
                if (strncmp(s_warnings[j].name, wname + 2, strlen(s_warnings[j].name)) == 0)
                {
                    return s_warnings[j].w;
                }
            }
        }
    }
    else
    {
        if (wname[1] == 69)
        {
            int ec = atoi(wname + 2);
            return ec;
        }
    }
    return 0;
}

unsigned long long get_warning_bit_mask(char * wname)
{
    unsigned char   disable_warning = wname[2] == 110 && wname[3] == 111;
    char * final_name = disable_warning ? wname + 5 : wname + 2;
    ;
    {
        int j = 0;
        for (; j < sizeof (s_warnings) / sizeof (s_warnings[0]); j++)
        {
            if (strncmp(s_warnings[j].name, final_name, strlen(s_warnings[j].name)) == 0)
            {
                return (1ULL << ((unsigned long long)s_warnings[j].w));
            }
        }
    }
    return 0;
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
inline int snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...)
{
    int _Result;
    char * _ArgList;
    ((void)(_ArgList = (char *)(&(_Format)) + ((sizeof (_Format) + sizeof (int) - 1) &  ~(sizeof (int) - 1))));
    _Result = vsnprintf(_Buffer, _BufferCount, _Format, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}

int get_warning_name(int   w, int n, char buffer[])
{
    if (is_diagnostic_configurable(w))
    {
        {
            int j = 0;
            for (; j < sizeof (s_warnings) / sizeof (s_warnings[0]); j++)
            {
                if (s_warnings[j].w == w)
                {
                    snprintf(buffer, n, "-W%s", s_warnings[j].name);
                    return 0;
                }
            }
        }
    }
    else
    {
        snprintf(buffer, n, "E%d", w);
    }
    return 0;
}

extern struct diagnostic  default_diagnostic;
int strcmp(char * _Str1, char * _Str2);
char *strcpy(char * _Destination, char * _Source);


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

int fill_options(struct options * options, int argc, char ** argv)
{
    options->diagnostic_stack.stack[0] = default_diagnostic;
    options->diagnostic_stack.stack[0].warnings =  ~(1ULL << 10);
    {
        int i = 1;
        for (; i < argc; i++)
        {
            if (argv[i][0] != 45)
            {
                continue;
            }
            if (argv[i][1] == 73 || argv[i][1] == 68)
            {
                continue;
            }
            if (strcmp(argv[i], "-no-output") == 0)
            {
                options->no_output = 1;
                continue;
            }
            if (strcmp(argv[i], "-o") == 0)
            {
                if (i + 1 < argc)
                {
                    strcpy(options->output, argv[i + 1]);
                    i++;
                }
                else
                {
                }
                continue;
            }
            if (strcmp(argv[i], "-sarif-path") == 0)
            {
                if (i + 1 < argc)
                {
                    strcpy(options->sarifpath, argv[i + 1]);
                    i++;
                }
                else
                {
                }
                continue;
            }
            if (strcmp(argv[i], "-show-includes") == 0)
            {
                options->show_includes = 1;
                continue;
            }
            if (strcmp(argv[i], "-E") == 0)
            {
                options->preprocess_only = 1;
                continue;
            }
            if (strcmp(argv[i], "-sarif") == 0)
            {
                options->sarif_output = 1;
                continue;
            }
            if (strcmp(argv[i], "-fanalyzer") == 0)
            {
                options->flow_analysis = 1;
                continue;
            }
            if (strcmp(argv[i], "-nullchecks") == 0)
            {
                options->null_checks_enabled = 1;
                continue;
            }
            if (strcmp(argv[i], "-ownership=enable") == 0)
            {
                options->ownership_enabled = 1;
                continue;
            }
            if (strcmp(argv[i], "-ownership=disable") == 0)
            {
                options->ownership_enabled = 0;
                continue;
            }
            if (strcmp(argv[i], "-test-mode") == 0)
            {
                options->test_mode = 1;
                continue;
            }
            if (strcmp(argv[i], "-msvc-output") == 0 || strcmp(argv[i], "-fdiagnostics-format=msvc") == 0)
            {
                options->visual_studio_ouput_format = 1;
                continue;
            }
            if (strcmp(argv[i], "-style=cake") == 0)
            {
                options->style = 0;
                continue;
            }
            if (strcmp(argv[i], "-style=gnu") == 0)
            {
                options->style = 7;
                continue;
            }
            if (strcmp(argv[i], "-style=microsoft") == 0)
            {
                options->style = 7;
                continue;
            }
            if (strcmp(argv[i], "-nullable=disable") == 0)
            {
                options->null_checks_enabled = 0;
                unsigned long long w = ((1ULL << 32) | (1ULL << 34));
                options->diagnostic_stack.stack[0].warnings =  ~w;
                continue;
            }
            if (strcmp(argv[i], "-nullable=enabled") == 0)
            {
                options->null_checks_enabled = 1;
                continue;
            }
            if (strcmp(argv[i], "-autoconfig") == 0 || strcmp(argv[i], "-auto-config") == 0)
            {
                options->auto_config = 1;
                continue;
            }
            if (strcmp(argv[i], "-std=c2x") == 0 || strcmp(argv[i], "-std=c23") == 0)
            {
                options->input = 0;
                continue;
            }
            if (strcmp(argv[i], "-std=cxx") == 0)
            {
                options->input = 2;
                continue;
            }
            if (argv[i][1] == 87)
            {
                if (strcmp(argv[i], "-Wall") == 0)
                {
                    options->diagnostic_stack.stack[0].warnings =  ~0ULL;
                    continue;
                }
                unsigned char   disable_warning = (argv[i][2] == 110 && argv[i][3] == 111);
                unsigned long long w = get_warning_bit_mask(argv[i]);
                if (w == 0)
                {
                    printf("unknown warning '%s'", argv[i]);
                    return 1;
                }
                if (disable_warning)
                {
                    options->diagnostic_stack.stack[0].warnings =  ~w;
                }
                else
                {
                    if (w == 10)
                    {
                        options->diagnostic_stack.stack[0].warnings = w;
                    }
                    else
                    {
                        options->diagnostic_stack.stack[0].notes = w;
                    }
                }
                continue;
            }
            if (strcmp(argv[i], "-dump-tokens") == 0)
            {
                options->dump_tokens = 1;
                continue;
            }
            if (strcmp(argv[i], "-dump-pp-tokens") == 0)
            {
                options->dump_pptokens = 1;
                continue;
            }
            if (strcmp(argv[i], "-disable-assert") == 0)
            {
                options->disable_assert = 1;
                continue;
            }
            printf("unknown option '%s'", argv[i]);
            return 1;
        }
    }
    return 0;
}

void print_help()
{
    char * options = "\x1b[92m";
    printf("%s", options);
}

