/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
 *
 *  struct object is used to compute the compile time expressions (including constexpr)
 *
 */

#pragma safety enable
#include "cake_compat.h"
#include "compile.h"
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include "tokenizer.h"
#include "parser.h"
#include <string.h>
#include "console.h"
#include "fs.h"
#include "json.h"

#ifdef _WIN32
#include <Windows.h>
#endif
#include "version.h"
#include "object.h"

#if defined _MSC_VER && !defined __POCC__
#include <crtdbg.h>
#include <debugapi.h>
#endif

#include "codegen.h"
#include <time.h>

static char* _Opt strrchr2(const char* s, int c)
{
    const char* _Opt last = NULL;
    unsigned char ch = (unsigned char)c;

    while (*s)
    {
        if ((unsigned char)*s == ch)
        {
            last = s; // record last match
        }
        s++;
    }

    // Handle case where c == '\0': return pointer to string terminator
    if (ch == '\0')
    {
        return (char*)s;
    }

    return (char*)last;
}

int fill_preprocessor_options(int argc, const char* const* argv, struct preprocessor_ctx* prectx)
{
    /* first loop used to collect options */
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
            continue;

        if (argv[i][1] == 'I')
        {
            const char* dir = argv[i] + 2;
            if (*dir == '\0' && i + 1 < argc)
            {
                /* `-I dir` (space-separated), same as `-Idir` */
                i++;
                dir = argv[i];
            }
            include_dir_add(&prectx->include_dir, dir);
            continue;
        }
        if (argv[i][1] == 'D')
        {
            char buffer[200] = { 0 };
            snprintf(buffer, sizeof buffer, "#define %s \n", argv[i] + 2);

            /* TODO make it more precise */
            char* p = &buffer[7];
            while (*p)
            {
                if (*p == '=')
                {
                    *p = ' ';
                    break;
                }
                p++;
            }

            struct tokenizer_ctx tctx = { 0 };
            struct token_list l1 = tokenizer(&tctx, buffer, "", 0, TK_FLAG_NONE);
            struct token_list r = preprocessor(prectx, &l1, 0);
            token_list_destroy(&l1);
            token_list_destroy(&r);
            continue;
        }
    }

    /* -D macros: their tokens were just destroyed */
    preprocessor_mark_predefined_macros(prectx);
    return 0;
}

#ifdef _WIN32

WINBASEAPI unsigned long WINAPI GetEnvironmentVariableA(const char* name,
    char* buffer,
    unsigned long size);

#endif

/* Collects this machine's system include directories into `dirs` - the same
   ones the platform compiler itself would search. Returns 0, or an error. */
static int collect_system_include_dirs(struct json_value* dirs)
{
#if defined(__TINYC__)

    /* built by tcc (default target is a tcc target): its own dirs, the indented lines after "include:" in -print-search-dirs */
#ifdef _WIN32
    FILE* _Owner _Opt fp = _popen("tcc -print-search-dirs", "r");
#else
    FILE* _Owner _Opt fp = popen("tcc -print-search-dirs", "r");
#endif
    if (fp == NULL)
        return errno;

    char line[400] = { 0 };
    int in_include_section = 0;
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
            line[--len] = '\0';

        if (line[0] != ' ')
        {
            if (in_include_section)
                break;
            in_include_section = strcmp(line, "include:") == 0;
            continue;
        }

        if (in_include_section)
        {
            const char* p = line;
            while (*p == ' ') p++;
            json_add_string(dirs, p);
        }
    }

#ifdef _WIN32
    _pclose(fp);
#else
    pclose(fp);
#endif
    return 0;

#elif defined(__linux__) || defined(__APPLE__)

    /* Parsed out of the platform compiler's own "-v -E" output, between
       "#include <...> search starts here:" and "End of search list.". */
#ifdef __linux__
    const char* command = "echo | gcc -v -E - 2>&1";
#else
    const char* command = "echo | clang -v -E - 2>&1";
#endif

    FILE* _Owner _Opt fp = popen(command, "r");
    if (fp == NULL)
        return errno;

    char path[400] = { 0 };
    int in_include_section = 0;
    while (fgets(path, sizeof(path), fp) != NULL)
    {
        if (strstr(path, "#include <...> search starts here:") != NULL)
        {
            in_include_section = 1;
            continue;
        }
        if (in_include_section && strstr(path, "End of search list.") != NULL)
            break;

        if (in_include_section)
        {
            const char* p = path;
            while (*p == ' ') p++;

            size_t len = strlen(path);
            if (len > 0 && path[len - 1] == '\n')
                path[len - 1] = '\0';

            /* clang labels macOS framework search paths with a trailing
               " (framework directory)" - it is not part of the path. */
            char* _Opt framework_tag = strstr(p, " (framework directory)");
            if (framework_tag != NULL)
                *framework_tag = '\0';

            json_add_string(dirs, p);
        }
    }

    pclose(fp);
    return 0;

#elif defined(_WIN32)

    /* Read straight out of INCLUDE, which a Visual Studio command prompt
       sets - hence the "run this inside one" message when it is absent. */
    char env[2000] = { 0 };
    int n = GetEnvironmentVariableA("INCLUDE", env, sizeof(env));
    if (n <= 0)
    {
        printf("INCLUDE not found.\nPlease, run cake -autoconfig inside visual studio command prompt.\n");
        return 1;
    }

    const char* p = env;
    for (;;)
    {
        if (*p == '\0')
            break;

        char filename_local[500] = { 0 };
        int count = 0;
        while (*p != '\0' && (*p != ';' && *p != '\n'))
        {
            filename_local[count] = *p;
            p++;
            count++;
        }
        filename_local[count] = 0;

        if (count > 0)
        {
            strcat(filename_local, "/");
            char* pch = filename_local;
            while (*pch)
            {
                if (*pch == '\\')
                    *pch = '/';
                pch++;
            }

            json_add_string(dirs, filename_local);
        }

        if (*p == '\0')
            break;
        p++;
    }

    return 0;

#else
    (void)dirs;
    return 0;
#endif
}

/* cake -auto-config: writes this machine's system include directories into
   cake.json's "include_dirs" array.

   The file is updated rather than overwritten: anything already in it -
   notably the "compile" object the IDE writes (see global_settings_save() in
   ide.c) - is read back and written out unchanged, so re-running auto-config
   never costs the user their compiler options. */
int generate_config_file(const char* configpath)
{
    struct json_value* _Opt _Owner root = NULL;

    /* Start from whatever is already there, so only include_dirs changes. */
    char* _Owner _Opt existing = read_file(configpath, true);
    if (existing)
    {
        root = json_parse(existing, NULL);
        free(existing);
    }

    if (root == NULL)
    {
        root = calloc(1, sizeof * root);
        if (root != NULL)
            root->type = JSON_OBJECT;
    }

    if (root == NULL)
    {
        printf("Out of memory generating '%s'.\n", configpath);
        return ENOMEM;
    }

    /* Replaces any existing array - json_set_array() overwrites a member of
       the same name rather than adding a second one. */
    struct json_value* _Opt dirs = json_set_array(root, "include_dirs");
    if (dirs == NULL)
    {
        json_delete(root);
        printf("Out of memory generating '%s'.\n", configpath);
        return ENOMEM;
    }

    int error = collect_system_include_dirs(dirs);
    if (error != 0)
    {
        json_delete(root);
        return error;
    }

    if (!json_write_file(configpath, root))
    {
        error = errno;
        printf("Cannot open the file '%s' for writing '%s'.\n",
               configpath, get_posix_error_message(error));
        json_delete(root);
        return error != 0 ? error : 1;
    }

    json_delete(root);

    printf("file '%s'\n", configpath);
    printf("successfully generated\n");
    return 0;
}

/*
  -find-definition: the parser already reported what it found
  (W_FIND_DEFINITION); this tells compile() through the report whether to
  keep searching in the next files.
*/
static void find_definition_to_report(const struct parser_ctx* ctx, struct report* report)
{
    const struct token* _Opt p = ctx->p_find_definition;
    if (p == NULL)
        return;

    report->find_definition_found = true;
    report->find_definition_is_declaration = ctx->find_definition_is_declaration;
    report->find_definition_is_static = ctx->find_definition_is_static;
    report->find_definition_is_tag = ctx->find_definition_is_tag;
    report->find_definition_is_local = ctx->find_definition_is_local;
    report->find_definition_line = p->line;
    report->find_definition_col = p->col;
    snprintf(report->find_definition_name, sizeof report->find_definition_name, "%s", p->lexeme);
    snprintf(report->find_definition_file, sizeof report->find_definition_file, "%s",
        p->token_origin ? p->token_origin->lexeme : "");
}

int compile_one_file(const char* file_name,
    struct options* options,
    const char* out_file_name,
    int argc,
    const char* const* argv,
    struct report* report)
{
#ifdef _CRTDBG_MAP_ALLOC
    _CrtMemState state1, state2, state_diff;
    _CrtMemCheckpoint(&state1);
#endif

    bool color_enabled = !options->color_disabled;

    if (!options_is_report_mode(options))
    {
        print_path(file_name, true);
        printf("\n");
    }

    struct preprocessor_ctx prectx = { 0 };
    prectx.options = *options;
    prectx.macros.capacity = 5000;

    add_standard_macros(&prectx, options->target);

    if (preprocessor_load_config(&prectx) != 0)
    {
        /* optional */
    }

    struct ast ast = { 0 };

    const char* _Owner _Opt p_output_string = NULL;

    _Opt struct parser_ctx ctx = { .p_input_list = &ast.token_list };

    struct tokenizer_ctx tctx = { 0 };
    struct token_list tokens = { 0 };

    tctx.options = *options;
    ctx.options = *options;
    ctx.p_report = report;
    char* _Owner _Opt content = NULL;

    try
    {
        // -D , -I etc..
        if (fill_preprocessor_options(argc, argv, &prectx) != 0)
        {
            throw;
        }

        prectx.options = *options;
        prectx.include_listener = report->include_listener;
        prectx.source_file = file_name;

        content = read_file(file_name, true /* append new line */);
        if (content == NULL)
        {
            report->error_count++;
            printf("file not found '%s'\n", file_name);
            throw;
        }

        if (options->sarif_output)
        {
            char sarif_file_name[260] = { 0 };
            if (options->sarifpath[0] != '\0')
            {
                mkdir(options->sarifpath, 0777);
                snprintf(sarif_file_name, sizeof sarif_file_name, "%s/%s.cake.sarif", options->sarifpath, basename(file_name));
            }
            else
            {
                snprintf(sarif_file_name, sizeof sarif_file_name, "%s.cake.sarif", file_name);
            }

            ctx.sarif_file = (FILE * _Owner _Opt) fopen(sarif_file_name, "w");
            if (ctx.sarif_file)
            {
                const char* begin_sarif =
                    "{\n"
                    "  \"version\": \"2.1.0\",\n"
                    "  \"$schema\": \"https://json.schemastore.org/sarif-2.1.0.json\",\n"
                    "  \"runs\": [\n"
                    "    {\n"
                    "      \"results\": [\n"
                    "\n";

                fprintf(ctx.sarif_file, "%s", begin_sarif);
            }
            else
            {
                report->error_count++;
                printf("cannot open Sarif output file '%s' - %s\n", sarif_file_name, get_posix_error_message(errno));
                throw;
            }
        }

        tokens = tokenizer(&tctx, content, file_name, 0, TK_FLAG_NONE);

        if (tctx.n_errors > 0)
            throw;

        const char* builtin = target_get_builtins(ctx.options.target);
        if (builtin[0] != '\0')
        {
            struct token_list builtin_tokens = tokenizer(&tctx, builtin, "builtins", 1, TK_FLAG_NONE);
            token_list_append_list_at_beginning(&tokens, &builtin_tokens);
            token_list_destroy(&builtin_tokens);
        }

        if (options->dump_tokens)
        {
            print_tokens(color_enabled, tokens.head);
        }

        ast.token_list = preprocessor(&prectx, &tokens, 0);

        if (options->p_unused_functions)
            preprocessor_register_unused_macros(&prectx, options->p_unused_functions);

        report->warnings_count += prectx.n_warnings;
        report->error_count += prectx.n_errors;

        if (prectx.preprocessor_auto_complete_done)
            throw; /* the cursor is on a directive line: no parse */

        if (prectx.n_errors > 0)
        {
            throw;
        }

        if (options->dump_pptokens)
        {
            if (ast.token_list.head != NULL)
                print_tokens(color_enabled, ast.token_list.head);
        }

        if (options->preprocess_only || options->copy_headers[0] != 0)
        {
            if (options->copy_headers[0] != '\0')
            {
                preprocessor_copy_included_headers(&prectx, options->copy_headers);
            }
            else
            {
                p_output_string = print_preprocessed_to_string2(ast.token_list.head);
                if (p_output_string)
                    printf("%s", p_output_string);

                FILE* _Owner _Opt outfile = fopen(out_file_name, "w");
                if (outfile)
                {
                    if (p_output_string)
                        fprintf(outfile, "%s", p_output_string);

                    const bool write_error = ferror(outfile) != 0;
                    if (fclose(outfile) != 0 || write_error)
                    {
                        report->error_count++;
                        printf("error writing output file '%s' - %s\n", out_file_name, get_posix_error_message(errno));
                        throw;
                    }
                }
                else
                {
                    report->error_count++;
                    printf("cannot open output file '%s' - %s\n", out_file_name, get_posix_error_message(errno));
                    throw;
                }
            }
        }
        else if (options->rename_macro ||
                 (options->rename_old_name[0] != '\0' && !prectx.rename_old_name_found))
        {
            /* -rename phase 2: a macro (the preprocessor already recorded it) or a name not in this file */
        }
        else if (prectx.p_find_definition)
        {
            /* -find-definition on a macro name: resolved by the preprocessor, no parse */
            ctx.p_find_definition = prectx.p_find_definition;
            find_definition_report(&ctx);
            diagnostic_queue_flush(&ctx.diagnostic_queue, &ctx);
            find_definition_to_report(&ctx, report);
            report->find_definition_is_macro = true;
        }
        else
        {
            bool berror = false;
            ast.declaration_list = parse(&ctx, &ast.token_list, &ast.file_scope, &berror);

            if (options_is_find_request(options))
                find_definition_to_report(&ctx, report);

            if (berror || report->error_count > 0)
                throw;

            if (options->format)
            {
                p_output_string = get_code_as_we_see(&ast.token_list, false);
                if (p_output_string)
                    printf("%s", p_output_string);
            }
            else if (!options->no_output && !report->has_errors)
            {

                struct osstream ss = { 0 };

                struct codegen_ctx ctx2 = { 0 };
                ctx2.p_ast = &ast;
                ctx2.options = ctx.options;
                const int codegen_error = codegen_visit(&ctx2, &ss);
                p_output_string = ss.c_str; // MOVE
                codegen_visit_ctx_destroy(&ctx2);

                if (codegen_error != 0)
                {
                    report->error_count++;
                    printf("code generation failed for '%s'\n", out_file_name);
                    throw;
                }

                FILE* _Owner _Opt outfile = fopen(out_file_name, "w");
                if (outfile)
                {
                    if (p_output_string)
                        fprintf(outfile, "%s", p_output_string);

                    const bool write_error = ferror(outfile) != 0;
                    if (fclose(outfile) != 0 || write_error)
                    {
                        report->error_count++;
                        printf("error writing output file '%s' - %s\n", out_file_name, get_posix_error_message(errno));
                        throw;
                    }
                }
                else
                {
                    report->error_count++;
                    printf("cannot open output file '%s' - %s\n", out_file_name, get_posix_error_message(errno));
                    throw;
                }
            }
        }

    }
    catch
    {
        // printf("Error %s\n", error->message);
    }

    if (ctx.sarif_file)
    {
#define SARIF_FOOTER                                                    \
    "      ],\n"                                                        \
    "      \"tool\": {\n"                                               \
    "        \"driver\": {\n"                                           \
    "          \"name\": \"cake\",\n"                                   \
    "          \"fullName\": \"cake code analysis\",\n"                 \
    "          \"version\": \"" CAKE_VERSION  "\",\n"                   \
    "          \"informationUri\": \"https://github.com/thradams/cake\"\n" \
    "        }\n"                                                       \
    "      }\n"                                                         \
    "    }\n"                                                           \
    "  ]\n"                                                             \
    "}\n"

        fprintf(ctx.sarif_file, "%s", SARIF_FOOTER);
        if (fclose(ctx.sarif_file) != 0)
        {
            report->error_count++;
            printf("error writing Sarif output file - %s\n", get_posix_error_message(errno));
        }
        ctx.sarif_file = NULL;
    }

    token_list_destroy(&tokens);

    parser_ctx_destroy(&ctx);
    free((void* _Owner _Opt)p_output_string);
    free(content);
    ast_destroy(&ast);
    preprocessor_ctx_destroy(&prectx);

#ifdef _CRTDBG_MAP_ALLOC
    _CrtMemCheckpoint(&state2);

    // Compare snapshots
    if (_CrtMemDifference(&state_diff, &state1, &state2))
    {
        report->test_failed++;
        printf("==================================================\n");
        printf("Memory leak\n");
        printf("==================================================\n");
        _CrtDumpMemoryLeaks();
        _CrtMemDumpStatistics(&state_diff);
    }
#endif

    return report->error_count > 0;
}

static int compile_many_files(const char* file_name,
    struct options* options,
    const char* out_file_name,
    int argc,
    const char* const* argv,
    struct report* report)
{
    const char* const file_name_name = basename(file_name);
    const char* _Opt const file_name_extension = strrchr2((char*)file_name_name, '.');

    if (file_name_extension == NULL)
    {
        _Assert(false);
    }

    int num_files = 0;

    char path[FS_MAX_PATH] = { 0 };
    snprintf(path, sizeof path, "%s", file_name);
    dirname(path);
    DIR* _Owner _Opt dir = opendir(path);

    if (dir == NULL)
    {
        return errno;
    }

    struct dirent* _Opt dp;
    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
        {
            /* skip self and parent */
            continue;
        }

        if (dp->d_type & DT_DIR)
        {

        }
        else
        {
            const char* const file_name_iter = basename(dp->d_name);
            const char* _Opt const file_extension = strrchr2((char*)file_name_iter, '.');

            if (file_name_extension &&
                file_extension &&
                strcmp(file_name_extension, file_extension) == 0)
            {
                // Fixes the output file name replacing the current name
                char out_file_name_final[FS_MAX_PATH] = { 0 };
                strcpy(out_file_name_final, out_file_name);
                dirname(out_file_name_final);
                strcat(out_file_name_final, "/");
                strcat(out_file_name_final, file_name_iter);

                char in_file_name_final[FS_MAX_PATH] = { 0 };
                strcpy(in_file_name_final, file_name);
                dirname(in_file_name_final);
                strcat(in_file_name_final, "/");
                strcat(in_file_name_final, file_name_iter);

                struct report report_local = { 0 };
                report_local.test_mode = report->test_mode;
                compile_one_file(in_file_name_final,
                    options,
                    out_file_name_final,
                    argc,
                    argv,
                    &report_local);

                report->error_count += report_local.error_count;
                report->warnings_count += report_local.warnings_count;
                report->info_count += report_local.info_count;
                report->test_succeeded += report_local.test_succeeded;
                report->test_failed += report_local.test_failed;
                num_files++;
            }
        }
    }

    closedir(dir);
    return num_files;
}

/* realpath, falling back to the original path when it fails (e.g. file does not exist) */
static void full_path_or_copy(const char* path, char resolved[FS_MAX_PATH])
{
    if (realpath(path, resolved) == NULL)
        snprintf(resolved, FS_MAX_PATH, "%s", path);
}

static void longest_common_path(int argc, const char* const* argv, char root_dir[FS_MAX_PATH])
{
    /*
     * find the longest common path
     */
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-o") == 0 ||
            strcmp(argv[i], "-sarif-path") == 0 ||
            strcmp(argv[i], "-I") == 0)
        {
            // ignore these files and consume next.
            i++;
            continue;
        }

        if (strcmp(argv[i], "-find-definition") == 0 || strcmp(argv[i], "-find-declaration") == 0 ||
            strcmp(argv[i], "-find-usages") == 0 || strcmp(argv[i], "-complete") == 0)
        {
            // consumes line and col
            i += 2;
            continue;
        }

        if (strcmp(argv[i], "-rename") == 0)
        {
            // consumes line, col and new name
            i += 3;
            continue;
        }

        if (argv[i][0] == '-')
            continue;

        char fullpath_i[FS_MAX_PATH] = { 0 };
        full_path_or_copy(argv[i], fullpath_i);
        strcpy(root_dir, fullpath_i);
        dirname(root_dir);

        for (int k = 0; k < FS_MAX_PATH; k++)
        {
            const char ch = fullpath_i[k];
            for (int j = 2; j < argc; j++)
            {
                if (argv[j][0] == '-')
                    continue;

                char fullpath_j[FS_MAX_PATH] = { 0 };
                full_path_or_copy(argv[j], fullpath_j);
                if (fullpath_j[k] != ch)
                {
                    memcpy(root_dir, fullpath_j, k);
                    root_dir[k] = '\0';
                    dirname(root_dir);
                    goto exit;
                }
            }
            if (ch == '\0')
                break;
        }
    }
exit:;
}

void print_report(const struct report* report)
{
    if (report->ignore_this_report)
        return;

    if (!report->quiet ||
        report->test_mode ||
        report->error_count != 0 ||
        report->warnings_count != 0 ||
        report->info_count != 0)
    {

        printf("\n");
        printf("%d" " errors ", report->error_count);
        printf("%d" " warnings ", report->warnings_count);
        printf("%d" " notes ", report->info_count);
        printf("\n");
        printf("%d files in %.2f seconds", report->no_files, report->cpu_time_used_sec);

        if (report->test_mode)
        {
            if (report->error_count > 0 || report->warnings_count > 0)
                printf(RED " - TEST FAILED" COLOR_RESET);
            else
                printf(GREEN " - TEST SUCCEEDED" COLOR_RESET);

        }
        printf("\n");

    }

    printf("\n");
}

static bool path_is_header(const char* path)
{
    const char* _Opt dot = strrchr(path, '.');
    return dot && (strcmp(dot, ".h") == 0 || strcmp(dot, ".H") == 0);
}

/*
  -find-definition: files[0] has the cursor; the caller orders the others by
  the chance of finding it (for a header, its .c first). The cursor is
  resolved in its own file, or, for a header that does not parse on its
  own, in the first file that includes it. When it resolves only to a
  declaration, the definition is searched by name, first in that same file
  and then, for external names, in the others.
*/
/* -find-definition: index of the .c named like file (file1.h -> file1.c), or -1 */
static int find_definition_counterpart(const char* const* files, int count, const char* file)
{
    char name[FS_MAX_PATH] = { 0 };
    remove_file_extension(basename(file), (int)sizeof name - 3, name);
    strcat(name, ".c");

    for (int i = 0; i < count; i++)
    {
        const char* a = basename(files[i]);
        const char* b = name;
        /* file names are case insensitive only on Windows */
#ifdef _WIN32
        while (*a && tolower((unsigned char)*a) == tolower((unsigned char)*b))
#else
        while (*a && *a == *b)
#endif
        {
            a++;
            b++;
        }
        if (*a == '\0' && *b == '\0')
            return i;
    }
    return -1;
}

/* -find-definition: the existing .c next to a header (dir/file1.h -> dir/file1.c), or false */
static bool find_definition_sibling(const char* file, char* out, int out_size)
{
    snprintf(out, out_size, "%s", file);
    char* dot = strrchr(basename(out), '.');
    if (dot == NULL || strcmp(dot, ".c") == 0 || (int)(dot - out) + 3 > out_size)
        return false;
    strcpy(dot, ".c");

    FILE* _Owner _Opt f = fopen(out, "rb");
    if (f == NULL)
        return false;
    fclose(f);
    return true;
}

static bool find_definition_search_by_name(const char** files, int count, int cursor_index,
    const struct report* p_report, struct options* options, int argc, const char* const* argv);

static void find_definition_run(const char** files, int count, struct options* options, int argc, const char* const * argv)
{
    if (count == 0)
        return;

    char fullpath[FS_MAX_PATH] = { 0 };
    full_path_or_copy(files[0], fullpath);
    snprintf(options->find_definition_file, sizeof options->find_definition_file, "%s", fullpath);

    /* -find-definition shows only the definition; the declaration only when there is none */
    options->find_definition_hide_declaration = options->request == REQUEST_FIND_DEFINITION;
    const int cursor_line = options->find_definition_line;
    const int cursor_col = options->find_definition_col;

    struct report report = { 0 };
    int cursor_index = -1;
    for (int i = 0; i < count; i++)
    {
        full_path_or_copy(files[i], fullpath);
        memset(&report, 0, sizeof report);
        compile_one_file(fullpath, options, "", argc, argv, &report);
        if (report.find_definition_found)
        {
            cursor_index = i;
            break;
        }

        if (!path_is_header(files[0]))
            break;
    }

    if (cursor_index < 0 || !report.find_definition_is_declaration || options->request == REQUEST_FIND_DECLARATION)
        return;

    if (find_definition_search_by_name(files, count, cursor_index, &report, options, argc, argv))
        return;

    /* no definition anywhere (e.g. a library function): the declaration is the answer */
    options->find_definition_hide_declaration = false;
    options->find_definition_line = cursor_line;
    options->find_definition_col = cursor_col;
    options->find_definition_name[0] = '\0';
    full_path_or_copy(files[cursor_index], fullpath);
    struct report report_declaration = { 0 };
    compile_one_file(fullpath, options, "", argc, argv, &report_declaration);
}

/* -find-definition, phase 2: the definition of the declaration phase 1 found,
   searched by name in the files; true when it was found (and reported) */
static bool find_definition_search_by_name(const char** files, int count, int cursor_index,
    const struct report* p_report, struct options* options, int argc, const char* const* argv)
{
    const struct report report = *p_report;
    char fullpath[FS_MAX_PATH] = { 0 };

    options->find_definition_line = 0;
    options->find_definition_col = 0;
    snprintf(options->find_definition_name, sizeof options->find_definition_name, "%s", report.find_definition_name);
    options->find_definition_name_static = report.find_definition_is_static;
    options->find_definition_name_is_tag = report.find_definition_is_tag;

    /* the .c named like the declaration's file (file1.h -> file1.c) goes to the first position after the cursor file */
    const int first = cursor_index == 0 ? 1 : 0;
    const int counterpart = find_definition_counterpart(files, count, report.find_definition_file);
    if (counterpart > first && counterpart != cursor_index)
    {
        const char* temp = files[first];
        files[first] = files[counterpart];
        files[counterpart] = temp;
    }

    /* a standalone file (only one passed, no project): the .c next to the declaration's file */
    char sibling[FS_MAX_PATH] = { 0 };
    const bool has_sibling = count == 1 &&
        find_definition_sibling(report.find_definition_file, sibling, (int)sizeof sibling);

    /* -2 is the file where the cursor resolved, -1 the .c next to the declaration; static names do not leave the first */
    for (int k = -2; k < count; k++)
    {
        if (k > -2 && options->find_definition_name_static)
            break;

        if (k == -1)
        {
            if (!has_sibling)
                continue;
            snprintf(fullpath, sizeof fullpath, "%s", sibling);
        }
        else
        {
            if (k == cursor_index)
                continue;
            full_path_or_copy(files[k < 0 ? cursor_index : k], fullpath);
        }

        struct report report_name = { 0 };
        compile_one_file(fullpath, options, "", argc, argv, &report_name);
        if (report_name.find_definition_found)
            return true;
    }
    return false;
}

static int rename_item_compare(const void* a, const void* b)
{
    const struct rename_item* x = a;
    const struct rename_item* y = b;
    const int c = strcmp(x->file, y->file);
    if (c != 0)
        return c;
    if (x->line != y->line)
        return x->line < y->line ? -1 : 1;
    return x->col < y->col ? -1 : (x->col > y->col ? 1 : 0);
}

/*
  -rename: replaces old_name with new_name at the items [begin, end) of one
  file (sorted by line and col) and writes it. Each position must still hold
  old_name, or the file is left untouched. Returns true if written.
*/
static bool rename_apply_file(const struct rename_list* list, int begin, int end, const char* new_name)
{
    const char* file = list->data[begin].file;
    char* _Owner _Opt content = read_file_binary(file);
    if (content == NULL)
    {
        printf("rename: cannot read '%s'\n", file);
        return false;
    }

    const size_t old_len = strlen(list->old_name);
    struct osstream out = { 0 };
    const char* copied = content;  /* content before it is already in out */
    const char* line_start = content;
    int line = 1;
    bool ok = true;

    for (int i = begin; i < end && ok; i++)
    {
        while (line < list->data[i].line && *line_start)
        {
            const char* eol = strchr(line_start, '\n');
            line_start = eol ? eol + 1 : line_start + strlen(line_start);
            line++;
        }

        const char* p = line_start + (list->data[i].col - 1);
        if (line != list->data[i].line || p < copied || strncmp(p, list->old_name, old_len) != 0)
        {
            ok = false;
            break;
        }

        ss_fprintf(&out, "%.*s%s", (int)(p - copied), copied, new_name);
        copied = p + old_len;
    }

    bool written = false;
    if (ok)
    {
        ss_fprintf(&out, "%s", copied);
        FILE* _Owner _Opt f = fopen(file, "wb");
        if (f)
        {
            written = fwrite(out.c_str ? out.c_str : "", 1, (size_t)out.size, f) == (size_t)out.size;
            if (fclose(f) != 0)
                written = false;
        }
        if (!written)
            printf("rename: cannot write '%s'\n", file);
    }
    else
    {
        printf("rename: '%s' changed since it was compiled, not renamed\n", file);
    }

    ss_close(&out);
    free(content);
    return written;
}

/*
  -find-usages: prints the items [begin, end) of one file as notes (the
  format of -find-definition, so the IDE opens them) with their source line.
*/
static void find_usages_print_file(const struct rename_list* list, int begin, int end, const struct options* options)
{
    const char* file = list->data[begin].file;
    char* _Owner _Opt content = read_file_binary(file);

    char text[300];
    snprintf(text, sizeof text, "usage of '%s'", list->old_name);

    struct osstream ss = { 0 };
    const char* _Opt line_start = content;
    int line = 1;
    for (int i = begin; i < end; i++)
    {
        ss_print_diagnostic_header(&ss, file, list->data[i].line, list->data[i].col,
            options->diagnostic_ouput_format, !options->color_disabled, true,
            W_FIND_DEFINITION, false, false, true, text);

        if (line_start == NULL)
            continue;

        while (line < list->data[i].line && *line_start)
        {
            const char* eol = strchr(line_start, '\n');
            line_start = eol ? eol + 1 : line_start + strlen(line_start);
            line++;
        }

        size_t len = strcspn(line_start, "\r\n");
        ss_fprintf(&ss, " %5d | %.*s\n", line, (int)len, line_start);
    }

    if (ss.c_str)
        fputs(ss.c_str, stdout);
    ss_close(&ss);
    free(content);
}

/*
  Porque precisa da Fase 3?
  
    ========================== s.h ======================== 
    struct S { int x; };      // alvo: s.h:1 (1)
    =======================================================

    ======================== t.h ============================
    struct S;   // outra declaração do mesmo S (2)
    =======================================================

    ======================= s.c ===========================
    #include "s.h"
    struct S a;
           ^
         cursor (linha 2, coluna 8, rename)
    =======================================================

    ====================== first.c ========================
    #include "t.h"
    struct S* p;             // aqui o parser liga S a t.h:1
    =======================================================

    ==================== both.c ===========================
    #include "s.h"
    #include "t.h"            // aqui t.h:1 é ligado a s.h:1
    =======================================================

    cake -find-usages 2 8 s.c first.c both.c

  
    1. s.c: o par aponta para o alvo s.h:1.  (1)
       Entram s.h:1 e s.c:2.
 
    2. first.c: os pares apontam para t.h:1 (2), que não é o 
       alvo e ainda não está na lista. 
       Nada liga first.c ao alvo, então ele fica undecided.
 
    3. both.c: o struct S; de t.h:1 é ligado (2) a s.h:1 (1), 
       o alvo.
       Entra t.h:1.

    4. Fase 3: o item t.h:1 (2) de first.c agora está na lista. 
       A declaração t.h:1 é aceita, e first.c:2 entra.
    
    Sem a Fase 3, first.c ficaria de fora nessa ordem, mas entraria na 
    ordem s.c both.c first.c. 
    Sem o both.c, nenhuma unidade liga t.h ao alvo, e o resultado é 2
    usos (s.c e s.h). 
    Isso está correto, porque não há como saber que os dois são o 
    mesmo símbolo.
*/
static void rename_run(const char** files, int count, struct options* options, int argc, const char* const* argv)
{
    if (count == 0)
        return;

    const bool find_usages = options->request == REQUEST_FIND_USAGES;
    const char* request_name = find_usages ? "find-usages" : "rename";

    char fullpath[FS_MAX_PATH] = { 0 };
    full_path_or_copy(files[0], fullpath);
    snprintf(options->find_definition_file, sizeof options->find_definition_file, "%s", fullpath);

    struct report report = { 0 };
    int cursor_index = -1;
    for (int i = 0; i < count; i++)
    {
        full_path_or_copy(files[i], fullpath);
        memset(&report, 0, sizeof report);
        compile_one_file(fullpath, options, "", argc, argv, &report);
        if (report.find_definition_found)
        {
            cursor_index = i;
            break;
        }

        if (!path_is_header(files[0]))
            break;
    }

    if (cursor_index < 0)
    {
        printf("%s: no symbol at the cursor\n", request_name);
        return;
    }

    /* declared outside the project (e.g. printf in stdio.h): cannot be renamed */
    char root_dir[FS_MAX_PATH] = { 0 };
    longest_common_path(argc, argv, root_dir);
    if (!find_usages && !path_is_under(report.find_definition_file, root_dir))
    {
        printf("rename: '%s' is declared outside the project (%s)\n",
            report.find_definition_name, report.find_definition_file);
        return;
    }

    struct rename_list list = { 0 };
    snprintf(list.old_name, sizeof list.old_name, "%s", report.find_definition_name);

    options->find_definition_line = 0;
    options->find_definition_col = 0;
    snprintf(options->rename_target_file, sizeof options->rename_target_file, "%s", report.find_definition_file);
    options->rename_target_line = report.find_definition_line;
    options->rename_target_col = report.find_definition_col;
    snprintf(options->rename_old_name, sizeof options->rename_old_name, "%s", report.find_definition_name);
    options->p_rename_list = &list;

    options->rename_macro = report.find_definition_is_macro;

    /* a macro defined in a .c exists only in that file */
    const bool only_cursor_file = report.find_definition_is_local || report.find_definition_is_static ||
        (report.find_definition_is_macro && !path_is_header(report.find_definition_file));
    /*
      a file is undecided when its occurrences only link to positions kept by
      files after it: its pairs are kept and decided again in phase 3
    */
    struct rename_pairs* _Owner _Opt undecided = calloc(count, sizeof(struct rename_pairs));
    for (int i = 0; i < count; i++)
    {
        if (only_cursor_file && i != cursor_index)
            continue;

        full_path_or_copy(files[i], fullpath);
        struct report report_file = { 0 };
        compile_one_file(fullpath, options, "", argc, argv, &report_file);
        if (!rename_list_commit(&list, options))
        {
            struct rename_pairs dropped = { 0 };
            rename_list_save_pending(&list, undecided ? &undecided[i] : &dropped);
            rename_pairs_clear(&dropped);
        }
    }

    /* phase 3: the saved pairs again (no compilation) while a pass keeps new occurrences */
    bool progress = undecided != NULL;
    while (progress)
    {
        progress = false;
        for (int i = 0; i < count; i++)
        {
            if (undecided[i].size > 0 && rename_list_commit_saved(&list, &undecided[i], options))
                progress = true;
        }
    }

    if (undecided)
    {
        for (int i = 0; i < count; i++)
            rename_pairs_clear(&undecided[i]);
        free(undecided);
    }

    if (list.size > 0)
        qsort(list.data, list.size, sizeof list.data[0], rename_item_compare);

    int files_changed = 0;
    for (int begin = 0; begin < list.size;)
    {
        int end = begin + 1;
        while (end < list.size && strcmp(list.data[end].file, list.data[begin].file) == 0)
            end++;
        if (find_usages)
            find_usages_print_file(&list, begin, end, options);
        else if (rename_apply_file(&list, begin, end, options->rename_new_name))
            files_changed++;
        begin = end;
    }

    if (find_usages)
        printf("%d usage(s) of '%s'\n", list.size, list.old_name);
    else
        printf("rename: '%s' -> '%s', %d occurrence(s), %d file(s) changed\n",
            list.old_name, options->rename_new_name, list.size, files_changed);

    options->p_rename_list = NULL;
    rename_list_clear(&list);
}

int compile(int argc, const char** argv, struct report* report)
{
    struct options options = { 0 };
    if (fill_options(&options, argc, argv) != 0)
    {
        report->error_count++;
        return 1;
    }

    if (options.target != TARGET_DEFAULT && !options_is_report_mode(&options))
    {
        printf("emulating %s\n", get_platform(options.target)->name);
    }

    char executable_path[FS_MAX_PATH - sizeof(CAKE_CONFIG_FILE_NAME)] = { 0 };
    get_self_path(executable_path, sizeof(executable_path));
    dirname(executable_path);
    char cakeconfig_path[FS_MAX_PATH] = { 0 };
    snprintf(cakeconfig_path, sizeof cakeconfig_path, "%s/" CAKE_CONFIG_FILE_NAME, executable_path);

    if (options.auto_config) // -autoconfig
    {
        report->ignore_this_report = true;
        return generate_config_file(cakeconfig_path);
    }

    report->test_mode = options.test_mode;
    report->quiet = options.quiet;

    clock_t begin_clock = clock();
    int no_files = 0;

    /* -find-definition: the files are compiled by find_definition_run() after the loop */
    const char* find_definition_files[256] = { 0 };
    int find_definition_count = 0;

    struct global_unused_list unused_functions_state = { 0 };
    if (options.request == REQUEST_REPORT_UNUSED)
        options.p_unused_functions = &unused_functions_state;

    char root_dir[FS_MAX_PATH] = { 0 };

    if (!options.no_output || options.request == REQUEST_REPORT_UNUSED)
    {
        longest_common_path(argc, argv, root_dir);
    }

    /* the unused report filters inputs, so it uses the common path of the inputs */
    if (options.request == REQUEST_REPORT_UNUSED && root_dir[0] != '\0')
        unused_functions_state.root_dir = strdup(root_dir);

    if (options.output_root[0] != '\0')
    {
        full_path_or_copy(options.output_root, root_dir);
    }

    const size_t root_dir_len = strlen(root_dir);

    /* second loop to compile each file */
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-o") == 0 ||
            strcmp(argv[i], "-sarif-path") == 0 ||
            strcmp(argv[i], "-I") == 0)
        {
            // consumes next
            i++;
            continue;
        }

        if (strcmp(argv[i], "-find-definition") == 0 || strcmp(argv[i], "-find-declaration") == 0 ||
            strcmp(argv[i], "-find-usages") == 0 || strcmp(argv[i], "-complete") == 0)
        {
            // consumes line and col
            i += 2;
            continue;
        }

        if (strcmp(argv[i], "-rename") == 0)
        {
            // consumes line, col and new name
            i += 3;
            continue;
        }

        if (argv[i][0] == '-')
            continue;

        no_files++;
        char output_file[FS_MAX_PATH] = { 0 };

        if (!options.no_output)
        {
            if (no_files == 1 && options.output[0] != '\0')
            {
                /*
                 * -o outputname
                 * works when we compile just one file
                 */
                strcat(output_file, options.output);
            }
            else
            {
                char fullpath[FS_MAX_PATH] = { 0 };
                full_path_or_copy(argv[i], fullpath);

                /* file outside root: output goes next to it */
                char file_root[FS_MAX_PATH] = { 0 };
                if (strncmp(fullpath, root_dir, root_dir_len) == 0 &&
                    (fullpath[root_dir_len] == '/' || fullpath[root_dir_len] == '\\'))
                {
                    strcpy(file_root, root_dir);
                }
                else
                {
                    strcpy(file_root, fullpath);
                    dirname(file_root);
                }

                strcpy(output_file, file_root);
                strcat(output_file, "/");
                strcat(output_file, get_platform(options.target)->name);

                strcat(output_file, fullpath + strlen(file_root));

                char outdir[FS_MAX_PATH] = { 0 };
                strcpy(outdir, output_file);
                dirname(outdir);
                if (create_multiple_paths(file_root, outdir) != 0)
                {
                    report->error_count++;
                    printf("error creating directory\n");
                    return 1;
                }
            }
        }

        char fullpath[FS_MAX_PATH] = { 0 };
        full_path_or_copy(argv[i], fullpath);

        const char* file_extension = basename(fullpath);

        if (file_extension[0] == '*')
        {
            no_files--; // does not count *.c 
            no_files += compile_many_files(fullpath, &options, output_file, argc, argv, report);
        }
        else if (options_is_find_request(&options) || options.request == REQUEST_COMPLETE)
        {
            if (find_definition_count < (int)_Countof(find_definition_files))
                find_definition_files[find_definition_count++] = argv[i];
        }
        else
        {
            struct report report_local = { 0 };
            report_local.include_listener = report->include_listener;
            compile_one_file(fullpath, &options, output_file, argc, argv, &report_local);

            if (report->include_listener && report->include_listener->file_done)
                report->include_listener->file_done(report->include_listener->data, fullpath, report_local.error_count);

            report->error_count += report_local.error_count;
            report->warnings_count += report_local.warnings_count;
            report->info_count += report_local.info_count;
            report->test_succeeded += report_local.test_succeeded;
            report->test_failed += report_local.test_failed;
        }
    }

    if (options.request == REQUEST_RENAME || options.request == REQUEST_FIND_USAGES)
    {
        rename_run(find_definition_files, find_definition_count, &options, argc, argv);
    }
    else if (options.request == REQUEST_COMPLETE)
    {
        /* -complete: only the file with the cursor */
        if (find_definition_count > 0)
        {
            char fullpath[FS_MAX_PATH] = { 0 };
            full_path_or_copy(find_definition_files[0], fullpath);
            snprintf(options.find_definition_file, sizeof options.find_definition_file, "%s", fullpath);
            struct report report_local = { 0 };
            compile_one_file(fullpath, &options, "", argc, argv, &report_local);
        }
    }
    else if (options_is_find_request(&options))
    {
        find_definition_run(find_definition_files, find_definition_count, &options, argc, argv);
    }

    if (options.request == REQUEST_REPORT_UNUSED)
    {
        global_unused_functions_report(&unused_functions_state, &options, report);
    }

    global_unused_functions_clear(&unused_functions_state);

    clock_t end_clock = clock();
    double cpu_time_used = ((double)(end_clock - begin_clock)) / CLOCKS_PER_SEC;
    report->no_files = no_files;
    report->cpu_time_used_sec = cpu_time_used;

    if (!options_is_report_mode(&options))
        print_report(report);

    if (report->test_mode)
    {
        if (report->error_count > 0 || report->warnings_count > 0)
        {
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    return (report->error_count > 0) ? EXIT_FAILURE : EXIT_SUCCESS;
}

/*
 * given a string s, produce argv by modifying the input string
 * return argc
 */
static int strtoargv(char* s, int n, const char* _Opt  argv[ /* n */])
{
    int argvc = 0;
    char* p = s;
    while (*p)
    {
        while (*p == ' ')
            p++;
        if (*p == 0)
            break;
        argv[argvc] = p;
        argvc++;
        while (*p != ' ' && *p != '\0')
            p++;
        if (*p == 0)
            break;
        *p = 0;
        p++;
        if (argvc >= n)
            break; /* nao tem mais lugares */
    }
    return argvc;
}

const char* _Owner _Opt compile_source(const char* pszoptions, const char* content, struct report* report)
{
    const char* argv[100] = { 0 };
    char string[200] = { 0 };
    snprintf(string, sizeof string, "exepath %s", pszoptions);

    const int argc = strtoargv(string, 100, argv);

    const char* _Owner _Opt s = NULL;

    struct preprocessor_ctx prectx = { 0 };
    struct ast ast = { 0 };
    struct options options = { .input = STD_EXT };

    try
    {
        if (fill_options(&options, argc, argv) != 0)
        {
            throw;
        }

        prectx.options = options;
        add_standard_macros(&prectx, options.target);

        if (options.preprocess_only || options.copy_headers[0] != '\0')
        {
            struct tokenizer_ctx tctx = { 0 };
            struct token_list tokens = tokenizer(&tctx, content, "c:/main.c", 0, TK_FLAG_NONE);

            struct token_list token_list = preprocessor(&prectx, &tokens, 0);
            if (prectx.n_errors == 0)
            {
                if (options.copy_headers[0] != '\0')
                {
                    preprocessor_copy_included_headers(&prectx, options.copy_headers);
                }
                else
                {
                    s = print_preprocessed_to_string2(token_list.head);
                }
            }

            token_list_destroy(&tokens);
            token_list_destroy(&token_list);
        }
        else
        {
            ast = get_ast(&options, "c:/main.c", content, report);
            if (report->error_count > 0)
                throw;

            struct osstream ss = { 0 };
            struct codegen_ctx ctx2 = { 0 };
            ctx2.p_ast = &ast;
            ctx2.options = options;
            const int codegen_error = codegen_visit(&ctx2, &ss);
            s = ss.c_str; // MOVED
            codegen_visit_ctx_destroy(&ctx2);

            if (codegen_error != 0)
            {
                report->error_count++;
                free((void* _Owner _Opt)s);
                s = NULL;
                throw;
            }
        }
    }
    catch
    {
    }

    preprocessor_ctx_destroy(&prectx);

    ast_destroy(&ast);

    return s;
}

const char* _Owner _Opt cake_format(const char* pszoptions, const char* _Opt path, const char* content, struct report* report)
{
    const char* real_filename = (path != NULL && path[0] != '\0') ? path : "c:/main.c";

    /*
     * Not get_ast(): that function assigns ast.token_list from the
     * preprocessor *before* checking prectx.n_errors, but never adds
     * prectx.n_errors/n_warnings into *report - so a preprocessor error
     * (e.g. a missing #include) leaves report->error_count at 0 while
     * ast.token_list holds only the tokens up to the failure. A caller
     * that trusts report->error_count == 0, like this one used to, would
     * then print that truncated token list instead of bailing out.
     */
    const char* argv[100] = { 0 };
    char string[200] = { 0 };
    snprintf(string, sizeof string, "exepath %s", pszoptions);
    const int argc = strtoargv(string, 100, argv);

    struct options options = { .input = STD_EXT };
    if (fill_options(&options, argc, argv) != 0)
    {
        return NULL;
    }

    struct ast ast = { 0 };
    struct tokenizer_ctx tctx = { 0 };
    tctx.options = options;
    struct preprocessor_ctx prectx = { 0 };
    struct token_list list = { 0 };
    const char* _Owner _Opt s = NULL;

    try
    {
        list = tokenizer(&tctx, content, real_filename, 0, TK_FLAG_NONE);
        if (tctx.n_errors > 0)
            throw;

        const char* builtin = target_get_builtins(options.target);
        if (builtin[0] != '\0')
        {
            /* level 1 - see the identical comment in compile_one_file() above. */
            struct token_list builtin_tokens = tokenizer(&tctx, builtin, "builtins", 1, TK_FLAG_NONE);
            token_list_append_list_at_beginning(&list, &builtin_tokens);
            token_list_destroy(&builtin_tokens);
        }

        prectx.options = options;
        prectx.macros.capacity = 5000;
        add_standard_macros(&prectx, options.target);

        if (preprocessor_load_config(&prectx) != 0)
        {
            // cake.json is optional
        }

        ast.token_list = preprocessor(&prectx, &list, 0);
        report->warnings_count += prectx.n_warnings;
        report->error_count += prectx.n_errors;
        if (prectx.n_errors > 0)
            throw;

        _Opt struct parser_ctx ctx = { .p_input_list = &ast.token_list };
        ctx.options = options;
        ctx.p_report = report;

        bool berror = false;
        ast.declaration_list = parse(&ctx, &ast.token_list, &ast.file_scope, &berror);
        parser_ctx_destroy(&ctx);
        if (berror || report->error_count > 0)
            throw;

        s = get_code_as_we_see(&ast.token_list, false);
    }
    catch
    {
    }

    token_list_destroy(&list);
    preprocessor_ctx_destroy(&prectx);
    ast_destroy(&ast);

    return s;
}

char* _Owner _Opt CompileText(const char* pszoptions, const char* content)
{
    /*
     * This function is called by the web playground
     */
    printf(WHITE "cake %s main.c\n", pszoptions);

    printf(WHITE "Cake " CAKE_VERSION COLOR_RESET "\n");

    struct report report = { 0 };
    char* _Owner _Opt s = (char* _Owner _Opt)compile_source(pszoptions, content, &report);

    return s;
}