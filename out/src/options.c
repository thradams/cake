struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
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

struct w {
    int   w;
    char * name;
};

struct _iobuf {
    void * _Placeholder;
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
    return !!(id > 63 && id <= 640);
}

unsigned char  is_diagnostic_error(int   id)
{
    return !!(id >= 640);
}

unsigned char  is_diagnostic_configurable(int   id)
{
    return !!(id >= 0 && id < 62);
}

int diagnostic_stack_push_empty(struct diagnostic_stack * diagnostic_stack)
{
    int index;

    index = diagnostic_stack->top_index;
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

struct diagnostic  default_diagnostic = {0, 9223372015379938269ULL, 0};
static struct w s_warnings[55] = {1, "unused-variable", 56, "unused-function", 2, "deprecated", 3, "enum-conversion", 4, "address", 5, "unused-parameter", 6, "hide-declarator", 7, "typeof-parameter", 8, "attributes", 9, "unused-value", 10, "style", 11, "comment", 12, "line-slicing", 44, "switch", 45, "unusual-null", 14, "discarded-qualifiers", 16, "uninitialized", 17, "return-local-addr", 36, "div-by-zero", 37, "constant-value", 46, "sizeof-array-argument", 13, "string-slicing", 15, "declarator-state", 20, "missing-owner-qualifier", 21, "not-owner", 22, "temp-owner", 23, "non-owner-move", 24, "non-owner-to-owner-move", 25, "discard-owner", 26, "non-owner-move", 35, "flow-div-by-zero", 27, "flow-not-null", 28, "missing-destructor", 31, "using-moved-object", 29, "analyzer-maybe-uninitialized", 32, "analyzer-null-dereference", 33, "analyzer-non-opt-arg", 30, "lifetime-ended", 34, "nullable-to-non-nullable", 18, "must-use-address-of", 38, "null-as-array", 39, "incompatible-enum", 40, "multi-char", 19, "array-indirection", 41, "out-of-bounds", 42, "array-parameter-assignment", 43, "conditional-constant", 47, "const-init", 48, "null-conversion", 49, "implicitly-unsigned-literal", 50, "overflow", 51, "array-size", 52, "empty-statement", 53, "incompatible-types", 54, "unused-label"};
void diagnostic_remove(struct diagnostic * d, int   w)
{
    if (!is_diagnostic_configurable(w))
    {
        return;
    }
    if ((d->errors & (1ULL << w)) != 0)
    {
        d->errors &= ~(1ULL << w);
    }
    if ((d->warnings & (1ULL << w)) != 0)
    {
        d->warnings &= ~(1ULL << w);
    }
    if ((d->notes & (1ULL << w)) != 0)
    {
        d->notes &= ~(1ULL << w);
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
    /*switch*/
    {
        register int   _R0 = w;
        if (_R0 == 28) goto _CKL1; /*case 28*/
        if (_R0 == 29) goto _CKL2; /*case 29*/
        if (_R0 == 31) goto _CKL3; /*case 31*/
        if (_R0 == 32) goto _CKL4; /*case 32*/
        if (_R0 == 33) goto _CKL5; /*case 33*/
        if (_R0 == 27) goto _CKL6; /*case 27*/
        if (_R0 == 30) goto _CKL7; /*case 30*/
        if (_R0 == 35) goto _CKL8; /*case 35*/
        goto /*default*/ _CKL9;

        {
            /*case 28*/ _CKL1:
            /*case 29*/ _CKL2:
            /*case 31*/ _CKL3:
            /*case 32*/ _CKL4:
            /*case 33*/ _CKL5:
            /*case 27*/ _CKL6:
            /*case 30*/ _CKL7:
            /*case 35*/ _CKL8:
            return 2;
            /*default*/ _CKL9:
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    return 0;
}

int __cdecl strncmp(char * _Str1, char * _Str2, unsigned int _MaxCount);
unsigned int __cdecl strlen(char * _Str);
int __cdecl atoi(char * _String);

int  get_warning(char * wname)
{
    if (!(wname[0] == 45 || wname[0] == 69))
    {
        return 0;
    }
    if (wname[0] == 45 && wname[1] == 87)
    {
        {
            int j;
            j = 0;
            for (; j < 55U; j++)
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
            int ec;

            ec = atoi(wname + 2);
            return ec;
        }
    }
    return 0;
}

unsigned long long get_warning_bit_mask(char * wname)
{
    unsigned char   disable_warning;
    char * final_name;

    disable_warning = !!(wname[2] == 110 && wname[3] == 111);
    final_name = disable_warning ? wname + 5 : wname + 2;
    ;
    {
        int j;
        j = 0;
        for (; j < 55U; j++)
        {
            if (strncmp(s_warnings[j].name, final_name, strlen(s_warnings[j].name)) == 0)
            {
                return (1ULL << ((unsigned long long)s_warnings[j].w));
            }
        }
    }
    return 0;
}



int __cdecl __stdio_common_vsprintf(unsigned __int64 _Options, char * _Buffer, unsigned int _BufferCount, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);

static unsigned __int64 __ck__OptionsStorage1;
inline unsigned __int64 *__cdecl __local_stdio_printf_options(void)
{
    return &__ck__OptionsStorage1;
}
inline int __cdecl vsnprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, char * _ArgList)
{
    int _Result;

    _Result = __stdio_common_vsprintf((*__local_stdio_printf_options()) | 2ULL, _Buffer, _BufferCount, _Format, 0U, _ArgList);
    return _Result < 0 ? -1 : _Result;
}
inline int __cdecl snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4U));
    _Result = vsnprintf(_Buffer, _BufferCount, _Format, _ArgList);
    ((void)(_ArgList = 0U));
    return _Result;
}

int get_warning_name(int   w, int n, char buffer[])
{
    if (is_diagnostic_configurable(w))
    {
        {
            int j;
            j = 0;
            for (; j < 55U; j++)
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
int __cdecl strcmp(char * _Str1, char * _Str2);
char *__cdecl strcpy(char * _Destination, char * _Source);


int __cdecl __stdio_common_vfprintf(unsigned __int64 _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
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

int fill_options(struct options * options, int argc, char ** argv)
{
    options->diagnostic_stack.stack[0] = default_diagnostic;
    options->diagnostic_stack.stack[0].warnings &= 18446744073709550591ULL;
    {
        int i;
        i = 1;
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
            if (strcmp(argv[i], "-const-literal") == 0)
            {
                options->const_literal = 1;
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
            if (strcmp(argv[i], "-H") == 0)
            {
                options->show_includes = 1;
                continue;
            }
            if (strcmp(argv[i], "-E") == 0)
            {
                options->preprocess_only = 1;
                continue;
            }
            if (strcmp(argv[i], "-preprocess-def-macro") == 0)
            {
                options->preprocess_def_macro = 1;
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
            if (strcmp(argv[i], "-debug") == 0)
            {
                options->do_static_debug = 1;
                if (i + 1 < argc)
                {
                    i++;
                    options->static_debug_lines = atoi(argv[i]);
                }
                else
                {
                }
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
                unsigned long long w;

                options->null_checks_enabled = 0;
                w = 21474836480ULL;
                options->diagnostic_stack.stack[0].warnings &= ~w;
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
                unsigned char   disable_warning;
                unsigned long long w;

                if (strcmp(argv[i], "-Wall") == 0)
                {
                    options->diagnostic_stack.stack[0].warnings = 18446744073709551615ULL;
                    continue;
                }
                disable_warning = !!((argv[i][2] == 110 && argv[i][3] == 111));
                w = get_warning_bit_mask(argv[i]);
                if (w == 0)
                {
                    printf("unknown warning '%s'", argv[i]);
                    return 1;
                }
                if (disable_warning)
                {
                    options->diagnostic_stack.stack[0].warnings &= ~w;
                }
                else
                {
                    if (w == 10)
                    {
                        options->diagnostic_stack.stack[0].warnings |= w;
                    }
                    else
                    {
                        options->diagnostic_stack.stack[0].notes |= w;
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
    char * options;

    options = "\x1b[92m""Usage :""\x1b[0m""\x1b[36;1m""cake ""\x1b[0m""\x1b[34;1m""[OPTIONS] source1.c source2.c ...\n""\x1b[0m""\n""\x1b[92m""Samples:\n""\x1b[0m""\n""\x1b[97m""    ""\x1b[36;1m""cake ""\x1b[0m"" source.c\n""\x1b[0m""    Compiles source.c and outputs /out/source.c\n""\n""\x1b[97m""    ""\x1b[36;1m""cake ""\x1b[0m"" -target=c11 source.c\n""\x1b[0m""    Compiles source.c and outputs C11 code at /out/source.c\n""\n""\x1b[97m""    ""\x1b[36;1m""cake ""\x1b[0m"" file.c -o file.cc && cl file.cc\n""\x1b[0m""    Compiles file.c and outputs file.cc then use cl to compile file.cc\n""\n""\x1b[97m""    ""\x1b[36;1m""cake ""\x1b[0m"" file.c -direct-compilation -o file.cc && cl file.cc\n""\x1b[0m""    Compiles file.c and outputs file.cc for direct compilation then use cl to compile file.cc\n""\n""\x1b[92m""Options:\n""\x1b[0m""\n""\x1b[36;1m""  -I                   ""\x1b[0m"" Adds a directory to the list of directories searched for include files \n""                        (On windows, if you run cake at the visual studio command prompt cake \n""                        uses the same include files used by msvc )\n""\n""\x1b[36;1m""  -auto-config           ""\x1b[0m""Generates cakeconfig.h with include directories\n""\n""\x1b[36;1m""  -no-output            ""\x1b[0m""Cake will not generate output\n""\n""\x1b[36;1m""  -D                    ""\x1b[0m""Defines a preprocessing symbol for a source file \n""\n""\x1b[36;1m""  -E                    ""\x1b[0m""Copies preprocessor output to standard output \n""\n""\x1b[36;1m""  -o name.c             ""\x1b[0m""Defines the output name. used when we compile one file\n""\n""\x1b[36;1m""  -remove-comments      ""\x1b[0m""Remove all comments from the output file \n""\n""\x1b[36;1m""  -direct-compilation   ""\x1b[0m""output without macros/preprocessor parts\n""\n""\x1b[36;1m""  -target=standard      ""\x1b[0m""Output target C standard (c89, c99, c11, c23, c2y, cxx) \n""                        C99 is the default and C89 (ANSI C) is the minimum target \n""\n""\x1b[36;1m""  -std=standard         ""\x1b[0m""Assume that the input sources are for standard (c89, c99, c11, c2x, cxx) \n""                        (not implemented yet, input is considered C23)                    \n""\n""\x1b[36;1m""  -fi                   ""\x1b[0m""Format input (format before language conversion)\n""\n""\x1b[36;1m""  -fo                   ""\x1b[0m""Format output (format after language conversion, result parsed again)\n""\n""\x1b[36;1m""  -no-discard           ""\x1b[0m""Makes [[nodiscard]] default implicitly \n""\n""\x1b[36;1m""  -Wname -Wno-name      ""\x1b[0m""Enables or disable warning\n""\n""\x1b[36;1m""  -fanalyzer            ""\x1b[0m""Runs flow analysis -  required for ownership\n""\n""\x1b[36;1m""  -sarif                ""\x1b[0m""Generates sarif files\n""\n""\x1b[36;1m""  -H                    ""\x1b[0m""Print the name of each header file used\n""\n""\x1b[36;1m""  -sarif-path           ""\x1b[0m""Set sarif output dir\n""\n""\x1b[36;1m""  -msvc-output          ""\x1b[0m""Output is compatible with visual studio\n""\n""\x1b[36;1m""  -dump-tokens          ""\x1b[0m""Output tokens before preprocessor\n""\n""\x1b[36;1m""  -dump-pp-tokens       ""\x1b[0m""Output tokens after preprocessor\n""\n""\x1b[36;1m""  -disable-assert       ""\x1b[0m""disables built-in assert\n""\n""\x1b[36;1m""  -const-literal        ""\x1b[0m""literal string becomes const\n""\n""\x1b[36;1m""  -preprocess-def-macro ""\x1b[0m""preprocess def macros after expansion\n""More details at http://thradams.com/cake/manual.html\n";
    printf("%s", options);
}

