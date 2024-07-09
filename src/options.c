//#pragma safety enable

#include "options.h"
#include <string.h>
#include "console.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>


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
    {W_DEPRECATED, "deprecated"},
    {W_ENUN_CONVERSION,"enum-conversion"},
    {W_FLOW_NON_NULL, "nonnull"},
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


    {W_STRING_SLICED,"string-slicing"},
    {W_DECLARATOR_STATE,"declarator-state"},
    {W_OWNERSHIP_MISSING_OWNER_QUALIFIER, "missing-owner-qualifier"},
    {W_OWNERSHIP_NOT_OWNER,"not-owner"},
    {W_OWNERSHIP_USING_TEMPORARY_OWNER,"temp-owner"},
    {W_OWNERSHIP_MOVE_ASSIGNMENT_OF_NON_OWNER, "non-owner-move"},
    {W_OWNERSHIP_NON_OWNER_TO_OWNER_ASSIGN, "non-owner-to-owner-move"},
    {W_OWNERSHIP_DISCARDING_OWNER, "discard-owner"},
    {W_FLOW_MISSING_DTOR, "missing-destructor"},
    {W_OWNERSHIP_NON_OWNER_MOVE, "non-owner-move"},
    {W_FLOW_MOVED, "using-moved-object"},
    {W_FLOW_UNINITIALIZED, "analyzer-maybe-uninitialized"},
    {W_FLOW_NULL_DEREFERENCE, "analyzer-null-dereference"}, // -fanalyzer
    {W_FLOW_MAYBE_NULL_TO_NON_OPT_ARG, "analyzer-non-opt-arg"},
    {W_FLOW_LIFETIME_ENDED, "lifetime-ended"},
    {W_MUST_USE_ADDRESSOF, "must-use-address-of"},
    {W_PASSING_NULL_AS_ARRAY, "null-as-array"},
    {W_INCOMPATIBLE_ENUN_TYPES, "incompatible-enum"},
    {W_MULTICHAR_ERROR, "multi-char"},
    {W_ARRAY_INDIRECTION,"array-indirection"},
    {W_OUT_OF_BOUNDS, "out-of-bounds"},
    {W_ASSIGNMENT_OF_ARRAY_PARAMETER, "array-parameter-assignment"},
    {W_CONDITIONAL_IS_CONSTANT,"conditional-constant"},
    {W_FLOW_NULLABLE_TO_NON_NULLABLE, "nullable-to-non-nullable"}

};

_Static_assert((sizeof(s_warnings) / sizeof(s_warnings[0])) < 64, "");

int get_diagnostic_type(struct diagnostic* d, enum diagnostic_id w)
{
    if (w >= 0 && w <= W_NOTE)
    {
        if ((d->errors & (1ULL << w)) != 0)
            return 3;

        if ((d->warnings & (1ULL << w)) != 0)
            return 2;

        if ((d->notes & (1ULL << w)) != 0)
            return 1;
    }

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
    assert(wname[0] == '-');
    for (int j = 0; j < sizeof(s_warnings) / sizeof(s_warnings[0]); j++)
    {
        if (strncmp(s_warnings[j].name, wname + 2, strlen(s_warnings[j].name)) == 0)
        {
            return (1ULL << ((unsigned long long)s_warnings[j].w));
        }
    }
    return 0;
}

int get_warning_name(enum diagnostic_id w, int n, char buffer[/*n*/])
{
    if (w >= 0 && w <= W_NOTE)
    {
        //TODO because s_warnings is _Out of order ....
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

int fill_options(struct options* options,
    int argc,
    const char** argv)
{

    /*
       default at this moment is same as -Wall
    */
    options->diagnostic_stack[0] = default_diagnostic;

    options->diagnostic_stack[0].warnings &= ~(1ULL << W_STYLE);
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

        if (strcmp(argv[i], "-showIncludes") == 0)
        {
            options->show_includes = true;
            continue;
        }
        if (strcmp(argv[i], "-E") == 0)
        {
            options->preprocess_only = true;
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

        if (strcmp(argv[i], "-remove-comments") == 0)
        {
            options->remove_comments = true;
            continue;
        }

        if (strcmp(argv[i], "-test-mode") == 0)
        {
            options->test_mode = true;
            continue;
        }
       
        if (strcmp(argv[i], "-direct-compilation") == 0 ||
            strcmp(argv[i], "-rm") == 0)
        {
            options->direct_compilation = true;
            continue;
        }

        if (strcmp(argv[i], "-fi") == 0)
        {
            options->format_input = true;
            continue;
        }

        if (strcmp(argv[i], "-fo") == 0)
        {
            options->format_ouput = true;
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
            options->diagnostic_stack[0].warnings &= ~w;
            continue;
        }

        if (strcmp(argv[i], "-nullable=enabled") == 0)
        {
            options->null_checks_enabled = true;
            continue;
        }

        if (strcmp(argv[i], "-autoconfig") == 0)
        {
            options->auto_config = true;
            continue;
        }

        //
        if (strcmp(argv[i], "-target=c89") == 0)
        {
            options->target = LANGUAGE_C89;
            continue;
        }

        if (strcmp(argv[i], "-target=c99") == 0)
        {
            options->target = LANGUAGE_C99;
            continue;
        }
        if (strcmp(argv[i], "-target=c11") == 0)
        {
            options->target = LANGUAGE_C11;
            continue;
        }
        if (strcmp(argv[i], "-target=c2x") == 0)
        {
            options->target = LANGUAGE_C2X;
            continue;
        }
        if (strcmp(argv[i], "-target=cxx") == 0)
        {
            options->target = LANGUAGE_CXX;
            continue;
        }



        //
        if (strcmp(argv[i], "-std=c99") == 0)
        {
            options->input = LANGUAGE_C99;
            continue;
        }
        if (strcmp(argv[i], "-std=c11") == 0)
        {
            options->input = LANGUAGE_C11;
            continue;
        }
        if (strcmp(argv[i], "-std=c2x") == 0)
        {
            options->input = LANGUAGE_C2X;
            continue;
        }
        if (strcmp(argv[i], "-std=cxx") == 0)
        {
            options->input = LANGUAGE_CXX;
            continue;
        }

        //warnings
        if (argv[i][1] == 'W')
        {
            if (strcmp(argv[i], "-Wall") == 0)
            {
                options->diagnostic_stack[0].warnings = ~0ULL;
                continue;
            }
            const bool disable_warning = (argv[i][2] == 'n' && argv[i][3] == 'o');

            unsigned long long w = 0;

            if (disable_warning)
                w = get_warning_bit_mask(argv[i] + 3);
            else
                w = get_warning_bit_mask(argv[i]);

            if (w == 0)
            {
                printf("unknown warning '%s'", argv[i]);
                return 1;
            }


            if (disable_warning)
            {
                options->diagnostic_stack[0].warnings &= ~w;
            }
            else
            {
                if (w == W_STYLE)
                    options->diagnostic_stack[0].warnings |= w;
                else
                    options->diagnostic_stack[0].notes |= w;
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
        "    Compiles source.c and outputs /_Out/source.c\n"
        "\n"
        WHITE "    " CAKE " -target=C11 source.c\n" RESET
        "    Compiles source.c and outputs C11 code at /_Out/source.c\n"
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
        LIGHTCYAN "  -autoconfig           " RESET "Generates cakeconfig.h with include directories\n"
        "\n"
        LIGHTCYAN "  -no-output            " RESET "Cake will not generate ouput\n"
        "\n"
        LIGHTCYAN "  -D                    " RESET "Defines a preprocessing symbol for a source file \n"
        "\n"
        LIGHTCYAN "  -E                    " RESET "Copies preprocessor output to standard output \n"
        "\n"
        LIGHTCYAN "  -o name.c             " RESET "Defines the ouput name. used when we compile one file\n"
        "\n"
        LIGHTCYAN "  -remove-comments      " RESET "Remove all comments from the ouput file \n"
        "\n"
        LIGHTCYAN "  -direct-compilation   " RESET "output without macros/preprocessor parts\n"
        "\n"
        LIGHTCYAN "  -target=standard      " RESET "Output target C standard (c89, c99, c11, c2x, cxx) \n"
        "                        C99 is the default and C89 (ANSI C) is the minimum target \n"
        "\n"
        LIGHTCYAN "  -std=standard         " RESET "Assume that the input sources are for standard (c89, c99, c11, c2x, cxx) \n"
        "                        (not implemented yet, input is considered C23)                    \n"
        "\n"
        LIGHTCYAN "  -fi                   " RESET "Format input (format before language convertion)\n"
        "\n"
        LIGHTCYAN "  -fo                   " RESET "Format output (format after language convertion, result parsed again)\n"
        "\n"
        LIGHTCYAN "  -no-discard           " RESET "Makes [[nodiscard]] default implicitly \n"
        "\n"
        LIGHTCYAN "  -Wname -Wno-name      " RESET "Enables or disable warning\n"
        "\n"
        LIGHTCYAN "  -fanalyzer            " RESET "Runs flow analysis -  required for onwership\n"
        "\n"
        LIGHTCYAN "  -sarif                " RESET "Generates sarif files\n"
        "\n"
        LIGHTCYAN "  -msvc-output          " RESET "Ouput is compatible with visual studio\n"
        "\n"
        LIGHTCYAN "  -dump-tokens          " RESET "Output tokens before preprocessor\n"
        "\n"
        LIGHTCYAN "  -dump-pp-tokens       " RESET "Output tokens after preprocessor\n"
        "\n"
        LIGHTCYAN "  -disable-assert       " RESET "disables built-in assert\n"
        "\n"
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
