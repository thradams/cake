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

#define _Countof(X) (sizeof(X)/sizeof(X[0]))

bool is_diagnostic_enabled(const struct options* options, enum diagnostic_id w)
{
    if (w > W_NOTE)
        return true;

    return ((options->diagnostic_stack.stack[options->diagnostic_stack.top_index].errors & (1ULL << w)) != 0) ||
        ((options->diagnostic_stack.stack[options->diagnostic_stack.top_index].warnings & (1ULL << w)) != 0) ||
        ((options->diagnostic_stack.stack[options->diagnostic_stack.top_index].notes & (1ULL << w)) != 0);
}

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

int diagnostic_id_stack_push(struct diagnostic_id_stack* diagnostic_stack, enum diagnostic_id id)
{
    if (diagnostic_stack->size < _Countof(diagnostic_stack->stack))
    {
        diagnostic_stack->stack[diagnostic_stack->size] = id;
        diagnostic_stack->size++;
    }
    return 0;
}

void diagnostic_id_stack_pop(struct diagnostic_id_stack* diagnostic_stack)
{
    if (diagnostic_stack->size > 0)
    {
        diagnostic_stack->size--;
    }
    else
    {
        assert(false);
    }
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
    case W_FLOW_NULLABLE_TO_NON_NULLABLE:
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

        if (has_prefix(argv[i], "-ownership="))
        {
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

            printf("Invalid option. Options are: "
                   "enable, disable"
                   "\n");
            continue;
        }

        if (strcmp(argv[i], "-test-mode") == 0)
        {
            options->test_mode = true;
            continue;
        }

        if (has_prefix(argv[i], "-fdiagnostics"))
        {
            if (strcmp(argv[i], "-fdiagnostics-color=never") == 0)
            {
                options->color_disabled = true;
                continue;
            }

            if (strcmp(argv[i], "-fdiagnostics-format=msvc") == 0) //same as clang
            {
                options->visual_studio_ouput_format = true;
                continue;
            }

            printf("Invalid. Valid options are:"
                   "-fdiagnostics-color=never" " "
                   "-fdiagnostics-format=msvc"
                   "\n");
        }

        if (strcmp(argv[i], "-msvc-output") == 0) //same as clang
        {
            options->visual_studio_ouput_format = true;
            continue;
        }

        if (has_prefix(argv[i], "-style"))
        {
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

            printf("Invalid style. Options are: "
                   "cake, gnu, microsoft"
                   "\n");
        }


        if (has_prefix(argv[i], "-nullable="))
        {
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

            printf("Invalid option. Options are: "
               "disable, enabled"
               "\n");
            continue;
        }

        if (strcmp(argv[i], "-autoconfig") == 0 ||
            strcmp(argv[i], "-auto-config") == 0)
        {
            options->auto_config = true;
            continue;
        }

        if (has_prefix(argv[i], "-target="))
        {
            int r = parse_target(argv[i] + (sizeof("-target=") - 1), &options->target);
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
        if (argv[i][1] == 'w')
        {
            if (strcmp(argv[i], "-wall") == 0)
            {
                options->diagnostic_stack.stack[0].warnings = ~0ULL;
                continue;
            }
            const bool disable_warning = (argv[i][2] == 'd');

            unsigned long long w = atoi(argv[i] + 3);
            w = (1ULL << ((unsigned long long)w));

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

static void print_option(const char* option, const char* description)
{
    const char* p = option;
    int count = 0;
    int first_colum = 28;



    printf(LIGHTCYAN " ");
    while (*p)
    {
        printf("%c", *p);
        count++;
        p++;
    }

    printf("%s", COLOR_RESET);


    for (; count < first_colum; count++)
        printf(" ");

    p = description;

    bool breakline = false;
    while (*p)
    {
        printf("%c", *p);
        count++;
        if (count > 70)
            breakline = true;

        if (breakline && *p == ' ')
        {
            breakline = false;
            printf("\n ");
            count = 0;
            for (; count < first_colum; count++)
                printf(" ");
        }
        p++;
    }

    printf("%s\n", COLOR_RESET);
}

void print_help()
{
    const char* sample =
        LIGHTGREEN "Usage : " COLOR_RESET "cake " LIGHTBLUE "[OPTIONS] source1.c source2.c ...\n" COLOR_RESET
        "\n"
        LIGHTGREEN "Samples:\n" COLOR_RESET
        "\n"
        WHITE "    cake source.c\n" COLOR_RESET
        "    Compiles source.c and outputs /out/source.c\n"
        "\n"
        WHITE "    cake file.c -o file.cc && cl file.cc\n" COLOR_RESET
        "    Compiles file.c and outputs file.cc then use cl to compile file.cc\n"
        "\n"
        LIGHTGREEN "Options:\n\n" COLOR_RESET;

    printf("%s", sample);

    print_option("-I", "Adds a directory to the list of directories searched for include files");
    print_option("-auto-config", "Generates cakeconfig.h with include directories");
    print_option("-no-output", "Cake will not generate output");
    print_option("-D", "Defines a preprocessing symbol for a source file");
    print_option("-E", "Copies preprocessor output to standard output");
    print_option("-o name", "Defines the output name when compiling one file");
    print_option("-no-discard", "Makes [[nodiscard]] default implicitly");
    print_option("-w -wd", "Enables or disable warning number");
    print_option("-wall", "Enables all warnings");
    print_option("-fanalyzer ", "Runs flow analysis -  required for ownership");
    print_option("-sarif ", "Generates sarif files");
    print_option("-H", "Print the name of each header file used");
    print_option("-sarif-path", "Set sarif output dir");
    print_option("-msvc-output", "Output is compatible with visual studio");
    print_option("-fdiagnostics-color=never", "Output will not use colors");
    print_option("-dump-tokens", "Output tokens before preprocessor");
    print_option("-dump-pp-tokens", "Output tokens after preprocessor");
    print_option("-disable-assert", "disables built-in assert");
    print_option("-const-literal", "literal string becomes const");
    print_option("-preprocess-def-macro", "preprocess def macros after expansion");

    printf("\n");
    printf("More details at http://cakecc.org/manual.html\n");

}

