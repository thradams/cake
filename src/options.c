/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma safety enable

#include "options.h"
#include <string.h>
#include "console.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>


bool is_diagnostic_note(enum diagnostic_id id)
{
    if (id == W_NOTE ||
        id == W_LOCATION)
    {
        return true;
    }

    return false;
}

bool is_diagnostic_warning(enum diagnostic_id id)
{
    return id > W_NOTE && id <= C_ERROR_INVALID_QUALIFIER_FOR_POINTER;
}

bool is_diagnostic_error(enum diagnostic_id id)
{
    return id >= C_ERROR_INVALID_QUALIFIER_FOR_POINTER;
}

bool is_diagnostic_configurable(enum diagnostic_id id)
{
    //We have 0-63 configurable (bit set)
    //configurable diagnostic also have names. Other have numbers only    
    return id >= 0 && id < W_LOCATION;
}

int diagnostic_stack_push_empty(struct diagnostic_stack* diagnostic_stack)
{
    int index = diagnostic_stack->top_index;
    diagnostic_stack->top_index++;
    diagnostic_stack->stack[diagnostic_stack->top_index].warnings = 0;
    diagnostic_stack->stack[diagnostic_stack->top_index].errors = 0;
    diagnostic_stack->stack[diagnostic_stack->top_index].notes = 0;
    return index;
}

void diagnostic_stack_pop(struct diagnostic_stack* diagnostic_stack)
{
    if (diagnostic_stack->top_index > 0)
    {
        diagnostic_stack->top_index--;
    }
    else
    {
        assert(false);
    }
}


struct diagnostic default_diagnostic = {
      .warnings = (~0ULL) & ~(
        NULLABLE_DISABLE_REMOVED_WARNINGS |
        (1ULL << W_NOTE) |
        (1ULL << W_STYLE) |
        (1ULL << W_UNUSED_PARAMETER) |
        (1ULL << W_UNUSED_VARIABLE))
};

static struct w {
    enum diagnostic_id w;
    const char* name;
}
s_warnings[] = {
    {W_UNUSED_VARIABLE, "unused-variable"},
    {W_UNUSED_FUNCTION, "unused-function"},
    {W_DEPRECATED, "deprecated"},
    {W_ENUN_CONVERSION,"enum-conversion"},

    {W_ADDRESS, "address"},
    {W_UNUSED_PARAMETER, "unused-parameter"},
    {W_DECLARATOR_HIDE, "hide-declarator"},
    {W_TYPEOF_ARRAY_PARAMETER, "typeof-parameter"},
    {W_ATTRIBUTES, "attributes"},
    {W_UNUSED_VALUE, "unused-value"},
    {W_STYLE, "style"},
    {W_COMMENT,"comment"},
    {W_LINE_SLICING,"line-slicing"},
    {W_SWITCH, "switch"},
    {W_UNSUAL_NULL_POINTER_CONSTANT, "unusual-null"},

    {W_DISCARDED_QUALIFIERS, "discarded-qualifiers"},
    {W_UNINITIALZED, "uninitialized"},
    {W_RETURN_LOCAL_ADDR, "return-local-addr"},
    {W_DIVIZION_BY_ZERO,"div-by-zero"},
    {W_CONSTANT_VALUE, "constant-value"},
    {W_SIZEOF_ARRAY_ARGUMENT, "sizeof-array-argument"},

    {W_STRING_SLICED,"string-slicing"},
    {W_DECLARATOR_STATE,"declarator-state"},
    {W_OWNERSHIP_MISSING_OWNER_QUALIFIER, "missing-owner-qualifier"},
    {W_OWNERSHIP_NOT_OWNER,"not-owner"},
    {W_OWNERSHIP_USING_TEMPORARY_OWNER,"temp-owner"},
    {W_OWNERSHIP_MOVE_ASSIGNMENT_OF_NON_OWNER, "non-owner-move"},
    {W_OWNERSHIP_NON_OWNER_TO_OWNER_ASSIGN, "non-owner-to-owner-move"},
    {W_OWNERSHIP_DISCARDING_OWNER, "discard-owner"},

    {W_OWNERSHIP_NON_OWNER_MOVE, "non-owner-move"},
    {W_FLOW_DIVIZION_BY_ZERO, "flow-div-by-zero"},

    /////////////////////////////////////////////////////////////////////////
    {W_FLOW_NON_NULL, "flow-not-null"},
    {W_FLOW_MISSING_DTOR, "missing-destructor"},
    {W_FLOW_MOVED, "using-moved-object"},
    {W_FLOW_UNINITIALIZED, "analyzer-maybe-uninitialized"},
    {W_FLOW_NULL_DEREFERENCE, "analyzer-null-dereference"}, // -fanalyzer
    {W_FLOW_MAYBE_NULL_TO_NON_OPT_ARG, "analyzer-non-opt-arg"},
    {W_FLOW_LIFETIME_ENDED, "lifetime-ended"},
    {W_FLOW_NULLABLE_TO_NON_NULLABLE, "nullable-to-non-nullable"},

    /////////////////////////////////////////////////////////////////////
    {W_MUST_USE_ADDRESSOF, "must-use-address-of"},
    {W_PASSING_NULL_AS_ARRAY, "null-as-array"},
    {W_INCOMPATIBLE_ENUN_TYPES, "incompatible-enum"},
    {W_MULTICHAR_ERROR, "multi-char"},
    {W_ARRAY_INDIRECTION,"array-indirection"},
    {W_OUT_OF_BOUNDS, "out-of-bounds"},
    {W_ASSIGNMENT_OF_ARRAY_PARAMETER, "array-parameter-assignment"},
    {W_CONDITIONAL_IS_CONSTANT,"conditional-constant"},

    {W_CONST_NOT_INITIALIZED, "const-init"},
    {W_NULL_CONVERTION, "null-conversion"},
    {W_IMPLICITLY_UNSIGNED_LITERAL, "implicitly-unsigned-literal"},
    {W_INTEGER_OVERFLOW, "overflow"},
    {W_ARRAY_SIZE, "array-size"},
    {W_EMPTY_STATEMENT, "empty-statement"},
    {W_ERROR_INCOMPATIBLE_TYPES, "incompatible-types"},
    {W_UNUSED_LABEL, "unused-label"}
};

void diagnostic_remove(struct diagnostic* d, enum diagnostic_id w)
{
    if (!is_diagnostic_configurable(w))
        return; //ops

    if ((d->errors & (1ULL << w)) != 0)
        d->errors &= ~(1ULL << w);

    if ((d->warnings & (1ULL << w)) != 0)
        d->warnings &= ~(1ULL << w);

    if ((d->notes & (1ULL << w)) != 0)
        d->notes &= ~(1ULL << w);
}

int get_diagnostic_type(struct diagnostic* d, enum diagnostic_id w)
{
    if (is_diagnostic_configurable(w))
    {
        if ((d->errors & (1ULL << w)) != 0)
            return 3;

        if ((d->warnings & (1ULL << w)) != 0)
            return 2;

        if ((d->notes & (1ULL << w)) != 0)
            return 1;
    }


    if (is_diagnostic_note(w))
        return 1;

    if (is_diagnostic_warning(w))
        return 2;

    if (is_diagnostic_error(w))
        return 3;

    return 3; //errors
}

int get_diagnostic_phase(enum diagnostic_id w)
{
    switch (w)
    {
        //TODO should be everything that starts with FLOW
    case W_FLOW_MISSING_DTOR:
    case W_FLOW_UNINITIALIZED:
    case W_FLOW_MOVED:
    case W_FLOW_NULL_DEREFERENCE:
    case W_FLOW_MAYBE_NULL_TO_NON_OPT_ARG:
    case W_FLOW_NON_NULL:
    case W_FLOW_LIFETIME_ENDED:
    case W_FLOW_DIVIZION_BY_ZERO:

        return 2; /*returns 2 if it flow analysis*/
    default:
        break;
    }
    return 0;
}


enum diagnostic_id  get_warning(const char* wname)
{
    if (!(wname[0] == '-' || wname[0] == 'E'))
    {
        return 0;
    }

    if (wname[0] == '-' && wname[1] == 'W')
    {
        for (int j = 0; j < sizeof(s_warnings) / sizeof(s_warnings[0]); j++)
        {
            if (strncmp(s_warnings[j].name, wname + 2, strlen(s_warnings[j].name)) == 0)
            {
                return s_warnings[j].w;
            }
        }
    }
    else if (wname[1] == 'E')
    {
        int ec = atoi(wname + 2);
        return ec;

    }
    return 0;
}

unsigned long long  get_warning_bit_mask(const char* wname)
{
    const bool disable_warning = wname[2] == 'n' && wname[3] == 'o';
    const char* final_name = disable_warning ? wname + 5 : wname + 2;
    assert(wname[0] == '-');
    for (int j = 0; j < sizeof(s_warnings) / sizeof(s_warnings[0]); j++)
    {

        if (strncmp(s_warnings[j].name, final_name, strlen(s_warnings[j].name)) == 0)
        {
            return (1ULL << ((unsigned long long)s_warnings[j].w));
        }
    }
    return 0;
}

int get_warning_name(enum diagnostic_id w, int n, char buffer[/*n*/])
{
    if (is_diagnostic_configurable(w))
    {
        //TODO because s_warnings is _Ctor of order ....
        //this is a linear seatch instead of just index! TODOD
        for (int j = 0; j < sizeof(s_warnings) / sizeof(s_warnings[0]); j++)
        {
            if (s_warnings[j].w == w)
            {
                snprintf(buffer, n, "-W%s", s_warnings[j].name);
                return 0;
            }
        }
    }
    else
    {
        snprintf(buffer, n, "E%d", w);
    }

    return 0;//"";
}

static int has_prefix(const char* str, const char* prefix)
{
    size_t len_prefix = strlen(prefix);
    return strncmp(str, prefix, len_prefix) == 0;
}

int fill_options(struct options* options,
    int argc,
    const char** argv)
{

    options->target = CAKE_COMPILE_TIME_SELECTED_TARGET;

    /*
       default at this moment is same as -Wall
    */
    options->diagnostic_stack.stack[0] = default_diagnostic;

    options->diagnostic_stack.stack[0].warnings &= ~(1ULL << W_STYLE);
    //&~items;


    /*first loop used to collect options*/
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
            continue;

        if (argv[i][1] == 'I' ||
            argv[i][1] == 'D')
        {
            /*
              Valid, but handled with preprocessor
            */
            continue;
        }

        if (strcmp(argv[i], "-no-output") == 0)
        {
            options->no_output = true;
            continue;
        }

        if (strcmp(argv[i], "-const-literal") == 0)
        {
            options->const_literal = true;
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
                //ops
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
                //ops
            }
            continue;
        }

        if (strcmp(argv[i], "-H") == 0)
        {
            options->show_includes = true;
            continue;
        }

        if (strcmp(argv[i], "-E") == 0)
        {
            options->preprocess_only = true;
            continue;
        }

        if (strcmp(argv[i], "-preprocess-def-macro") == 0)
        {
            options->preprocess_def_macro = true;
            continue;
        }

        if (strcmp(argv[i], "-sarif") == 0)
        {
            options->sarif_output = true;
            continue;
        }

        if (strcmp(argv[i], "-fanalyzer") == 0)
        {
            options->flow_analysis = true;
            continue;
        }

        if (strcmp(argv[i], "-nullchecks") == 0)
        {
            options->null_checks_enabled = true;
            continue;
        }

        if (strcmp(argv[i], "-debug") == 0)
        {
            options->do_static_debug = true;
            if (i + 1 < argc)
            {
                i++;
                options->static_debug_lines = atoi(argv[i]);
            }
            else
            {
                //ops
            }
            continue;
        }

        if (strcmp(argv[i], "-ownership=enable") == 0)
        {
            options->ownership_enabled = true;
            continue;
        }

        if (strcmp(argv[i], "-ownership=disable") == 0)
        {
            options->ownership_enabled = false;
            continue;
        }

        if (strcmp(argv[i], "-test-mode") == 0)
        {
            options->test_mode = true;
            continue;
        }

        if (strcmp(argv[i], "-msvc-output") == 0 ||
            strcmp(argv[i], "-fdiagnostics-format=msvc") == 0) //same as clang
        {
            options->visual_studio_ouput_format = true;
            continue;
        }


        //
        if (strcmp(argv[i], "-style=cake") == 0)
        {
            options->style = STYLE_CAKE;
            continue;
        }

        if (strcmp(argv[i], "-style=gnu") == 0)
        {
            options->style = STYLE_GNU;
            continue;
        }

        if (strcmp(argv[i], "-style=microsoft") == 0)
        {
            options->style = STYLE_GNU;
            continue;
        }

        if (strcmp(argv[i], "-nullable=disable") == 0)
        {
            options->null_checks_enabled = false;
            unsigned long long w = NULLABLE_DISABLE_REMOVED_WARNINGS;
            options->diagnostic_stack.stack[0].warnings &= ~w;
            continue;
        }

        if (strcmp(argv[i], "-nullable=enabled") == 0)
        {
            options->null_checks_enabled = true;
            continue;
        }

        if (strcmp(argv[i], "-autoconfig") == 0 ||
            strcmp(argv[i], "-auto-config") == 0)
        {
            options->auto_config = true;
            continue;
        }

        if (has_prefix(argv[i], "-target"))
        {
            int r = parse_target(argv[i], &options->target);
            if (r != 0)
            {
                printf("Invalid target. Options: ");
                print_target_options();
                printf("\n");
            }
            continue;
        }


        if (strcmp(argv[i], "-std=c2x") == 0 ||
            strcmp(argv[i], "-std=c23") == 0)
        {
            options->input = LANGUAGE_C23;
            continue;
        }
        if (strcmp(argv[i], "-std=cxx") == 0)
        {
            options->input = LANGUAGE_CAK;
            continue;
        }

        //warnings
        if (argv[i][1] == 'W')
        {
            if (strcmp(argv[i], "-Wall") == 0)
            {
                options->diagnostic_stack.stack[0].warnings = ~0ULL;
                continue;
            }
            const bool disable_warning = (argv[i][2] == 'n' && argv[i][3] == 'o');

            unsigned long long w = get_warning_bit_mask(argv[i]);

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
                if (w == W_STYLE)
                    options->diagnostic_stack.stack[0].warnings |= w;
                else
                    options->diagnostic_stack.stack[0].notes |= w;
            }
            continue;
        }

        if (strcmp(argv[i], "-dump-tokens") == 0)
        {
            options->dump_tokens = true;
            continue;
        }

        if (strcmp(argv[i], "-dump-pp-tokens") == 0)
        {
            options->dump_pptokens = true;
            continue;
        }

        if (strcmp(argv[i], "-disable-assert") == 0)
        {
            options->disable_assert = true;
            continue;
        }

        printf("unknown option '%s'", argv[i]);
        return 1;
    }
    return 0;
}


void print_help()
{
#define CAKE LIGHTCYAN "cake " RESET 

    const char* options =
        LIGHTGREEN "Usage :" RESET CAKE LIGHTBLUE "[OPTIONS] source1.c source2.c ...\n" RESET
        "\n"
        LIGHTGREEN "Samples:\n" RESET
        "\n"
        WHITE "    " CAKE " source.c\n" RESET
        "    Compiles source.c and outputs /out/source.c\n"
        "\n"
        WHITE "    " CAKE " file.c -o file.cc && cl file.cc\n" RESET
        "    Compiles file.c and outputs file.cc then use cl to compile file.cc\n"
        "\n"
        WHITE "    " CAKE " file.c -direct-compilation -o file.cc && cl file.cc\n" RESET
        "    Compiles file.c and outputs file.cc for direct compilation then use cl to compile file.cc\n"
        "\n"
        LIGHTGREEN "Options:\n" RESET
        "\n"
        LIGHTCYAN "  -I                   " RESET " Adds a directory to the list of directories searched for include files \n"
        "                        (On windows, if you run cake at the visual studio command prompt cake \n"
        "                        uses the same include files used by msvc )\n"
        "\n"
        LIGHTCYAN "  -auto-config           " RESET "Generates cakeconfig.h with include directories\n"
        "\n"
        LIGHTCYAN "  -no-output            " RESET "Cake will not generate output\n"
        "\n"
        LIGHTCYAN "  -D                    " RESET "Defines a preprocessing symbol for a source file \n"
        "\n"
        LIGHTCYAN "  -E                    " RESET "Copies preprocessor output to standard output \n"
        "\n"
        LIGHTCYAN "  -o name.c             " RESET "Defines the output name when compiling one file\n"
        "\n"
        LIGHTCYAN "  -std=standard         " RESET "Assume that the input sources are for standard (c89, c99, c11, c2x, cxx) \n"
        "                        (not implemented yet, input is considered C23)                    \n"
        "\n"
        LIGHTCYAN "  -no-discard           " RESET "Makes [[nodiscard]] default implicitly \n"
        "\n"
        LIGHTCYAN "  -Wname -Wno-name      " RESET "Enables or disable warning\n"
        "\n"
        LIGHTCYAN "  -fanalyzer            " RESET "Runs flow analysis -  required for ownership\n"
        "\n"
        LIGHTCYAN "  -sarif                " RESET "Generates sarif files\n"
        "\n"
        LIGHTCYAN "  -H                    " RESET "Print the name of each header file used\n"
        "\n"
        LIGHTCYAN "  -sarif-path           " RESET "Set sarif output dir\n"
        "\n"
        LIGHTCYAN "  -msvc-output          " RESET "Output is compatible with visual studio\n"
        "\n"
        LIGHTCYAN "  -dump-tokens          " RESET "Output tokens before preprocessor\n"
        "\n"
        LIGHTCYAN "  -dump-pp-tokens       " RESET "Output tokens after preprocessor\n"
        "\n"
        LIGHTCYAN "  -disable-assert       " RESET "disables built-in assert\n"
        "\n"
        LIGHTCYAN "  -const-literal        " RESET "literal string becomes const\n"
        "\n"
        LIGHTCYAN "  -preprocess-def-macro " RESET "preprocess def macros after expansion\n"

        "More details at http://thradams.com/cake/manual.html\n"
        ;

    printf("%s", options);
}

#ifdef TEST
#include "unit_test.h"
#include <string.h>

void test_get_warning_name()
{
    char dbg_name[100];
    get_warning_name(W_FLOW_MISSING_DTOR, sizeof dbg_name, dbg_name);
    assert(strcmp(dbg_name, "-Wmissing-destructor") == 0);

    unsigned long long  flags = get_warning_bit_mask(dbg_name);
    assert(flags == (1ULL << W_FLOW_MISSING_DTOR));


    get_warning_name(W_STYLE, sizeof dbg_name, dbg_name);
    assert(strcmp(dbg_name, "-Wstyle") == 0);

    unsigned long long  flags2 = get_warning_bit_mask(dbg_name);
    assert(flags2 == (1ULL << W_STYLE));
}

#endif
