/* Cake 0.12.26 x86_msvc */
struct __crt_locale_data;
struct __crt_multibyte_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct diagnostic_id_stack {
    int size;
    int stack[10];
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

struct _iobuf {
    void * _Placeholder;
};

struct __crt_multibyte_data;

struct __crt_locale_data;

struct w {
    int w;
    char * name;
};


unsigned char is_diagnostic_enabled(struct options * options, int w)
{
    if (w > 63)
    {
        return 1;
    }
    return ((options->diagnostic_stack.stack[options->diagnostic_stack.top_index].errors & (1ULL << w)) != 0) || ((options->diagnostic_stack.stack[options->diagnostic_stack.top_index].warnings & (1ULL << w)) != 0) || ((options->diagnostic_stack.stack[options->diagnostic_stack.top_index].notes & (1ULL << w)) != 0);
}


unsigned char is_diagnostic_note(int id)
{
    if (id == 63 || id == 62)
    {
        return 1;
    }
    return 0;
}


unsigned char is_diagnostic_warning(int id)
{
    return id > 63 && id <= 640;
}


unsigned char is_diagnostic_error(int id)
{
    return id >= 640;
}


unsigned char is_diagnostic_configurable(int id)
{
    return id >= 0 && id < 62;
}


int diagnostic_id_stack_push(struct diagnostic_id_stack * diagnostic_stack, int id)
{
    if (diagnostic_stack->size < 10)
    {
        diagnostic_stack->stack[diagnostic_stack->size] = id;
        diagnostic_stack->size++;
    }
    return 0;
}


void diagnostic_id_stack_pop(struct diagnostic_id_stack * diagnostic_stack)
{
    if (diagnostic_stack->size > 0)
    {
        diagnostic_stack->size--;
    }
    else
    {
        ;
    }
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


struct diagnostic  default_diagnostic = {0,9223371993905100731ULL,0};
void diagnostic_remove(struct diagnostic * d, int w)
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


int get_diagnostic_type(struct diagnostic * d, int w)
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


int get_diagnostic_phase(int w)
{
    /*switch*/
    {
        int __v0 = w;
        if (__v0 == 35) goto __L1; /*case 35*/
        if (__v0 == 29) goto __L2; /*case 29*/
        if (__v0 == 30) goto __L3; /*case 30*/
        if (__v0 == 32) goto __L4; /*case 32*/
        if (__v0 == 33) goto __L5; /*case 33*/
        if (__v0 == 34) goto __L6; /*case 34*/
        if (__v0 == 28) goto __L7; /*case 28*/
        if (__v0 == 31) goto __L8; /*case 31*/
        if (__v0 == 36) goto __L9; /*case 36*/
        goto __L10; /* default */

        {
            __L1: /*case 35*/ 
            __L2: /*case 29*/ 
            __L3: /*case 30*/ 
            __L4: /*case 32*/ 
            __L5: /*case 33*/ 
            __L6: /*case 34*/ 
            __L7: /*case 28*/ 
            __L8: /*case 31*/ 
            __L9: /*case 36*/ 
            return 2;
            __L10: /* default */ 
            goto __L0; /* break */

        }
        __L0:;
    }
    return 0;
}


int __cdecl strncmp(char * _Str1, char * _Str2, unsigned int _MaxCount);
static struct w s_warnings[57] = {2,"unused-variable",57,"unused-function",3,"deprecated",4,"enum-conversion",5,"address",6,"unused-parameter",7,"hide-declarator",8,"typeof-parameter",9,"attributes",10,"unused-value",11,"style",12,"comment",13,"line-slicing",45,"switch",46,"unusual-null",15,"discarded-qualifiers",17,"uninitialized",18,"return-local-addr",37,"div-by-zero",38,"constant-value",47,"sizeof-array-argument",14,"string-slicing",16,"declarator-state",21,"missing-owner-qualifier",22,"not-owner",23,"temp-owner",24,"non-owner-move",25,"non-owner-to-owner-move",26,"discard-owner",27,"non-owner-move",36,"flow-div-by-zero",28,"flow-not-null",29,"missing-destructor",32,"using-moved-object",30,"analyzer-maybe-uninitialized",33,"analyzer-null-dereference",34,"analyzer-non-opt-arg",31,"lifetime-ended",35,"nullable-to-non-nullable",19,"must-use-address-of",39,"null-as-array",40,"incompatible-enum",41,"multi-char",20,"array-indirection",42,"out-of-bounds",43,"array-parameter-assignment",44,"conditional-constant",48,"const-init",49,"null-conversion",58,"bool-comparison",50,"implicitly-unsigned-literal",51,"overflow",52,"array-size",53,"empty-statement",54,"incompatible-types",55,"unused-label",60,"null-non-null"};

unsigned int __cdecl strlen(char * _Str);
int __cdecl atoi(char * _String);

int get_warning(char * wname)
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
            for (; j < 57; j++)
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
    unsigned char  disable_warning;
    char * final_name;

    disable_warning = wname[2] == 110 && wname[3] == 111;
    final_name = disable_warning ? wname + 5 : wname + 2;
    ;
    {
        int j;

        j = 0;
        for (; j < 57; j++)
        {
            if (strncmp(s_warnings[j].name, final_name, strlen(s_warnings[j].name)) == 0)
            {
                return (1ULL << ((unsigned long long)s_warnings[j].w));
            }
        }
    }
    return 0;
}


static int __cdecl snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...);
static int __cdecl vsnprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, char * _ArgList);
int __cdecl __stdio_common_vsprintf(unsigned long long _Options, char * _Buffer, unsigned int _BufferCount, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;

int get_warning_name(int w, int n, char buffer[])
{
    if (is_diagnostic_configurable(w))
    {
        {
            int j;

            j = 0;
            for (; j < 57; j++)
            {
                if (s_warnings[j].w == w)
                {
                    snprintf(buffer, n, "-W%s", s_warnings[j].name);
                    return 0;
                }
            }
        }
        snprintf(buffer, n, "W%d", w);
    }
    else
    {
        snprintf(buffer, n, "E%d", w);
    }
    return 0;
}



static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &__c0__OptionsStorage;
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
int get_warning_name_and_number(int w, int n, char buffer[])
{
    if (is_diagnostic_configurable(w))
    {
        {
            int j;

            j = 0;
            for (; j < 57; j++)
            {
                if (s_warnings[j].w == w)
                {
                    snprintf(buffer, n, "-W%s/-W%d", s_warnings[j].name, w);
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


int __cdecl strcmp(char * _Str1, char * _Str2);
char *__cdecl strcpy(char * _Destination, char * _Source);
static int has_prefix(char * str, char * prefix);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
int parse_target(char * targetstr, int * target);
void print_target_options();

int fill_options(struct options * options, int argc, char ** argv)
{
    options->target = 1;
    options->diagnostic_stack.stack[0] = default_diagnostic;
    options->diagnostic_stack.stack[0].warnings &= 18446744073709549567ULL;
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
            if (has_prefix(argv[i], "-ownership="))
            {
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
                printf("Invalid option. Options are: enable, disable\n");
                continue;
            }
            if (strcmp(argv[i], "-test-mode") == 0)
            {
                options->test_mode = 1;
                continue;
            }
            if (has_prefix(argv[i], "-fdiagnostics"))
            {
                if (strcmp(argv[i], "-fdiagnostics-color=never") == 0)
                {
                    options->disable_colors = 1;
                    continue;
                }
                if (strcmp(argv[i], "-fdiagnostics-format=msvc") == 0)
                {
                    options->visual_studio_ouput_format = 1;
                    continue;
                }
                printf("Invalid. Valid options are:-fdiagnostics-color=never -fdiagnostics-format=msvc\n");
            }
            if (strcmp(argv[i], "-msvc-output") == 0)
            {
                options->visual_studio_ouput_format = 1;
                continue;
            }
            if (has_prefix(argv[i], "-style"))
            {
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
                printf("Invalid style. Options are: cake, gnu, microsoft\n");
            }
            if (has_prefix(argv[i], "-nullable="))
            {
                if (strcmp(argv[i], "-nullable=disable") == 0)
                {
                    unsigned long long w;

                    options->null_checks_enabled = 0;
                    w = 42949672960ULL;
                    options->diagnostic_stack.stack[0].warnings &= ~w;
                    continue;
                }
                if (strcmp(argv[i], "-nullable=enabled") == 0)
                {
                    options->null_checks_enabled = 1;
                    continue;
                }
                printf("Invalid option. Options are: disable, enabled\n");
                continue;
            }
            if (strcmp(argv[i], "-autoconfig") == 0 || strcmp(argv[i], "-auto-config") == 0)
            {
                options->auto_config = 1;
                continue;
            }
            if (has_prefix(argv[i], "-target="))
            {
                int r;

                r = parse_target(argv[i] + 8, &options->target);
                if (r != 0)
                {
                    printf("Invalid target. Options: ");
                    print_target_options();
                    printf("\n");
                }
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
                unsigned char  disable_warning;
                unsigned long long w;

                if (strcmp(argv[i], "-Wall") == 0)
                {
                    options->diagnostic_stack.stack[0].warnings = 18446744073709551615ULL;
                    continue;
                }
                disable_warning = (argv[i][2] == 110 && argv[i][3] == 111);
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
                    if (w == 11)
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



static int has_prefix(char * str, char * prefix)
{
    unsigned int len_prefix;

    len_prefix = strlen(prefix);
    return strncmp(str, prefix, len_prefix) == 0;
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
void print_help()
{
    char * options;

    options = "\x1b[92mUsage :\x1b[0m\x1b[36;1mcake \x1b[0m\x1b[34;1m[OPTIONS] source1.c source2.c ...\n\x1b[0m\n\x1b[92mSamples:\n\x1b[0m\n\x1b[97m    \x1b[36;1mcake \x1b[0m source.c\n\x1b[0m    Compiles source.c and outputs /out/source.c\n\n\x1b[97m    \x1b[36;1mcake \x1b[0m file.c -o file.cc && cl file.cc\n\x1b[0m    Compiles file.c and outputs file.cc then use cl to compile file.cc\n\n\x1b[97m    \x1b[36;1mcake \x1b[0m file.c -direct-compilation -o file.cc && cl file.cc\n\x1b[0m    Compiles file.c and outputs file.cc for direct compilation then use cl to compile file.cc\n\n\x1b[92mOptions:\n\x1b[0m\n\x1b[36;1m  -I                   \x1b[0m Adds a directory to the list of directories searched for include files \n                        (On windows, if you run cake at the visual studio command prompt cake \n                        uses the same include files used by msvc )\n\n\x1b[36;1m  -auto-config           \x1b[0mGenerates cakeconfig.h with include directories\n\n\x1b[36;1m  -no-output            \x1b[0mCake will not generate output\n\n\x1b[36;1m  -D                    \x1b[0mDefines a preprocessing symbol for a source file \n\n\x1b[36;1m  -E                    \x1b[0mCopies preprocessor output to standard output \n\n\x1b[36;1m  -o name.c             \x1b[0mDefines the output name when compiling one file\n\n\x1b[36;1m  -no-discard           \x1b[0mMakes [[nodiscard]] default implicitly \n\n\x1b[36;1m  -Wname -Wno-name      \x1b[0mEnables or disable warning\n\n\x1b[36;1m  -fanalyzer            \x1b[0mRuns flow analysis -  required for ownership\n\n\x1b[36;1m  -sarif                \x1b[0mGenerates sarif files\n\n\x1b[36;1m  -H                    \x1b[0mPrint the name of each header file used\n\n\x1b[36;1m  -sarif-path           \x1b[0mSet sarif output dir\n\n\x1b[36;1m  -msvc-output          \x1b[0mOutput is compatible with visual studio\n\n\x1b[36;1m  -dump-tokens          \x1b[0mOutput tokens before preprocessor\n\n\x1b[36;1m  -dump-pp-tokens       \x1b[0mOutput tokens after preprocessor\n\n\x1b[36;1m  -disable-assert       \x1b[0mdisables built-in assert\n\n\x1b[36;1m  -const-literal        \x1b[0mliteral string becomes const\n\n\x1b[36;1m  -preprocess-def-macro \x1b[0mpreprocess def macros after expansion\nMore details at http://thradams.com/cake/manual.html\n";
    printf("%s", options);
}


