/*
 * Build system for Cake C Compiler
 *
 * WINDOWS
 *   cl -DTEST build.c && build
 *   Debugging: cl /Zi build.c && devenv /DebugExe build.exe
 *
 * LINUX/MACOS
 *   gcc build.c -o build && ./build
 *   ./build fast             (incremental: only recompile changed files)
 *   ./build test             (pass compile cake with -DTEST and run tests)
 *   ./build debug            (debug build: no optimization, debug runtime)
 *   ./build fast debug test  (flags combine freely)
 */

#include "build.h"
#include <sys/stat.h>


 /*---------------------------------------------------------------------------
  * Source file lists
  *---------------------------------------------------------------------------*/

#define CAKE_LIB_SOURCE_FILES \
    " token.c "               \
    " hashmap.c "             \
    " console.c "             \
    " tokenizer.c "           \
    " osstream.c "            \
    " fs.c "                  \
    " options.c "             \
    " object.c "              \
    " expressions.c "         \
    " pre_expressions.c "     \
    " parser.c "              \
    " compile.c "             \
    " defer.c "               \
    " codegen.c "             \
    " flow1.c "               \
    " flow3.c "               \
    " error.c "               \
    " target.c "              \
    " type.c "

#define CAKE_SOURCE_FILES \
    CAKE_LIB_SOURCE_FILES \
    " main.c "

#define HOEDOWN_SOURCE_FILES \
    " autolink.c "           \
    " buffer.c "             \
    " document.c "           \
    " escape.c "             \
    " hoedown.c "            \
    " html.c "               \
    " html_blocks.c "        \
    " html_smartypants.c "   \
    " stack.c "              \
    " version.c "


  /*---------------------------------------------------------------------------
   * Compiler flags (per platform/config)
   *---------------------------------------------------------------------------*/

#if defined COMPILER_MSVC

#define MSVC_DEBUG_CONFIG_FLAGS \
        " /D_CRTDBG_MAP_ALLOC " \
        " /Od /MDd /RTC1 "      \
        " /Dstrdup=_strdup "

#define MSVC_RELEASE_CONFIG_FLAGS \
        " /GL /Gy /O2 /MT /Ot " \
        " /DNDEBUG "             \
        " /Dstrdup=_strdup "

#define MSVC_COMMON_FLAGS        \
      " /D_CRT_NONSTDC_NO_WARNINGS " \
      " /wd4996 "                    \
      " /wd4100 "                    \
      " /wd4068 "                    \
      " /permissive- "               \
      " /GS "                        \
      " /Zc:preprocessor- "          \
      " /utf-8 "                     \
      " /W4 "                        \
      " /Zi "                        \
      " /Gm- "                       \
      " /std:clatest "               \
      " /Zc:inline "                 \
      " /Gd "                        \
      " /Oy- "                       \
      " /FC "                        \
      " /EHsc "                      \
      " /D_CRT_SECURE_NO_WARNINGS "

#define MSVC_DEBUG_LINK_FLAGS    \
      " /link "                    \
      " /NODEFAULTLIB "            \
      " ucrtd.lib vcruntimed.lib msvcrtd.lib " \
      " Kernel32.lib User32.lib Advapi32.lib " \
      " uuid.lib Ws2_32.lib Rpcrt4.lib Bcrypt.lib "

#define MSVC_RELEASE_LINK_FLAGS  \
      " /link "                    \
      " /NODEFAULTLIB "            \
      " ucrt.lib vcruntime.lib msvcrt.lib " \
      " Kernel32.lib User32.lib Advapi32.lib " \
      " uuid.lib Ws2_32.lib Rpcrt4.lib Bcrypt.lib "

#endif /* COMPILER_MSVC */

#if defined PLATFORM_WINDOWS && defined COMPILER_CLANG

#define CLANG_WIN_DEBUG_FLAGS    " -D_DEBUG "
#define CLANG_WIN_RELEASE_FLAGS  " -DNDEBUG "

#define CLANG_WIN_FLAGS          \
      " -Dstrdup=_strdup "         \
      " -Wno-switch "              \
      " -DWIN32 "                  \
      " -D_CRT_SECURE_NO_WARNINGS " \
      " -std=c17 -Wno-multichar "  \
      " -D_MT "                    \
      " -Xlinker /NODEFAULTLIB "   \
      " -lucrt.lib -lvcruntime.lib -lmsvcrt.lib " \
      " -lKernel32.lib -lUser32.lib -lAdvapi32.lib " \
      " -luuid.lib -lWs2_32.lib -lRpcrt4.lib -lBcrypt.lib "

#endif /* PLATFORM_WINDOWS && COMPILER_CLANG */

#if (defined PLATFORM_LINUX || defined PLATFORM_MACOS) && defined COMPILER_CLANG

#define CLANG_UNIX_FLAGS   \
      " -g "                 \
      " -Wall "              \
      " -D_DEFAULT_SOURCE "  \
      " -Wno-unknown-pragmas " \
      " -Wno-multichar "     \
      " -std=c17 "

#endif /* (PLATFORM_LINUX || PLATFORM_MACOS) && COMPILER_CLANG */

#if defined COMPILER_GCC && !defined COMPILER_TINYC

#define GCC_FLAGS            \
      " -Wall "                \
      " -Wno-multichar "       \
      " -Wno-unknown-pragmas " \
      " -g "

#endif /* COMPILER_GCC && !COMPILER_TINYC */


static void print_header(const char* text)
{
    printf("================================================\n");
    printf(" %s\n", text);
    printf("================================================\n");
}

static void generate_doc(const char* mdfilename, const char* outfile)
{
    static const char header[] =
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "  <link rel=\"stylesheet\" href=\"default.min.css\">\n"
        "  <script src=\"highlight.min.js\"></script>\n"
        "  <script>hljs.highlightAll();</script>\n"
        "  <link rel=\"stylesheet\" href=\"style.css\" />\n"
        "  <title>Cake C Compiler</title>\n"
        "  <meta name=\"description\" content=\"Cake C Compiler\">\n"
        "  <link rel=\"icon\" type=\"image/x-icon\" href=\"favicon.ico\">\n"
        "  <script>\n"
        "  function Try(elm) {\n"
        "    var source = elm.parentElement.previousElementSibling.innerText;\n"
        "    var link = \"./playground.html?code=\" + encodeURIComponent(btoa(source))\n"
        "             + \"&to=\" + encodeURI(\"-2\")\n"
        "             + \"&options=\" + encodeURI(\"\");\n"
        "    window.open(link, '_blank');\n"
        "  }\n"
        "  // find-replace for this:\n"
        "  // <button onclick=\"Try(this)\">try</button>\n"
        "  </script>\n"
        "</head>\n"
        "<body>\n"
        "  <article style=\"max-width: 40em; margin:auto\">\n"
        "    <p><a href=\"index.html\">Home</a>"
        " | <a href=\"manual.html\">Manual</a>"
        " | <a href=\"ownership.html\">Static Analysis</a>"
        " | <a href=\"playground.html\">Playground</a></p>\n"
        "    <article>\n"
        "    <h1>Cake - C23 and Beyond</h1>\n";

    static const char footer[] = "</article></body></html>";

    char cmd[200];

    FILE* f = fopen(outfile, "w");
    if (!f) { printf("error: could not open %s for writing\n", outfile); exit(1); }
    fwrite(header, 1, strlen(header), f);
    fclose(f);

    snprintf(cmd, sizeof cmd,
             RUN "hoedown.exe --html-toc --toc-level 3 --autolink --fenced-code %s >> %s",
             mdfilename, outfile);
    execute_cmd(cmd);

    snprintf(cmd, sizeof cmd,
             RUN "hoedown.exe --toc-level 3 --autolink --fenced-code %s >> %s",
             mdfilename, outfile);
    execute_cmd(cmd);

    f = fopen(outfile, "a");
    if (!f) { printf("error: could not open %s for appending\n", outfile); exit(1); }
    fwrite(footer, 1, strlen(footer), f);
    fclose(f);
}

static void build_tools(void)
{
    print_header("Build tools");

#ifdef _WIN32
    execute_cmd(CC " -D_CRT_SECURE_NO_WARNINGS install.c advapi32.lib user32.lib "
                CC_OUTPUT("install.exe"));
#else
    execute_cmd(CC " -D_CRT_SECURE_NO_WARNINGS install.c " CC_OUTPUT("install.exe"));
#endif

    execute_cmd(CC " server.c "  CC_OUTPUT("cakeserver.exe"));
    execute_cmd(CC " install.c " CC_OUTPUT("install.exe"));

    echo_chdir("./tools");
    execute_cmd(CC " -D_CRT_SECURE_NO_WARNINGS maketest.c "           CC_OUTPUT("../maketest.exe"));
    execute_cmd(CC " -D_CRT_SECURE_NO_WARNINGS amalgamator.c "        CC_OUTPUT("../amalgamator.exe"));
    execute_cmd(CC " -D_CRT_SECURE_NO_WARNINGS -I.. embed.c ../fs.c ../error.c "
                CC_OUTPUT("../embed.exe"));

    echo_chdir("./hoedown");
    execute_cmd(CC HOEDOWN_SOURCE_FILES CC_OUTPUT("../../hoedown.exe"));

    echo_chdir("../..");
}

static void build_docs(void)
{
    print_header("Build docs");

    generate_doc("../manual.md", "./web/manual.html");
    generate_doc("../README.md", "./web/index.html");
    generate_doc("../warnings.md", "./web/warnings.html");
    generate_doc("../ownership.md", "./web/ownership.html");

    remove("hoedown.exe");
}

static void build_inner_tests(void)
{
    print_header("Build inner tests");
    execute_cmd(RUN "maketest.exe unit_test.c " CAKE_SOURCE_FILES);
    remove("maketest.exe");
}

static void build_embedded_files(void)
{
    print_header("Build embedded files");
    execute_cmd(RUN "embed.exe \"./include\" ");
}

static void build_amalgamation(void)
{
    print_header("Build amalgamated file");
    execute_cmd(RUN "amalgamator.exe -olib.c" CAKE_LIB_SOURCE_FILES);
    remove("amalgamator.exe");
}

static time_t get_mtime(const char* path)
{
    struct stat st;
    if (stat(path, &st) != 0)
        return 0;
    return st.st_mtime;
}

/*
 * Iterates space-separated `sources`, compiles each .c to .o if the .o
 * is missing or older than the .c, then links all .o files into `output`.
 * `compiler` is the compiler binary (e.g. "gcc", "clang").
 * `compile_flags` are the flags used for both compile and link steps.
 * `link_flags` are appended only at link time (libs, /link, etc.).
 * `obj_flag` is the flag for specifying the object output ("-o" or "/Fo").
 * `out_flag` is the flag for specifying the final output ("-o" or "/out:").
 */
static void build_incremental(const char* compiler,
                               const char* compile_flags,
                               const char* sources,
                               const char* link_flags,
                               const char* obj_flag,
                               const char* out_flag,
                               const char* output)
{
    char src[64];
    char obj[72];
    char cmd[2048];
    char obj_list[4096];
    int  any_changed = 0;

    obj_list[0] = '\0';

    const char* p = sources;
    while (*p)
    {
        /* skip whitespace */
        while (*p == ' ' || *p == '\t') p++;
        if (!*p) break;

        /* read token */
        int i = 0;
        while (*p && *p != ' ' && *p != '\t' && i < (int)(sizeof src) - 1)
            src[i++] = *p++;
        src[i] = '\0';
        if (i == 0) continue;

        /* derive .o name: "flow3.c" -> "flow3.o" */
        snprintf(obj, sizeof obj, "%s", src);
        char* dot = strrchr(obj, '.');
        if (dot) strcpy(dot, ".o");
        else      strcat(obj, ".o");

        /* append to object list */
        strcat(obj_list, " ");
        strcat(obj_list, obj);

        /* compile if .o missing or .c newer */
        if (get_mtime(src) > get_mtime(obj))
        {
            printf("compiling: %s\n", src);
            snprintf(cmd, sizeof cmd, "%s %s -c %s %s%s",
                     compiler, compile_flags, src, obj_flag, obj);
            execute_cmd(cmd);
            any_changed = 1;
        }
    }

    /* link if anything changed or output missing */
    if (any_changed || get_mtime(output) == 0)
    {
        printf("linking: %s\n", output);
        snprintf(cmd, sizeof cmd, "%s %s%s %s %s",
                 compiler, out_flag, output, obj_list, link_flags);
        execute_cmd(cmd);
    }
    else
    {
        printf("No sources changed, skipping.\n");
    }
}

static void build_cake(int fastbuild, int debug, const char* test_flag)
{
    print_header("Build cake");

#if defined COMPILER_MSVC

    const char* msvc_config = debug ? MSVC_DEBUG_CONFIG_FLAGS : MSVC_RELEASE_CONFIG_FLAGS;
    const char* msvc_link = debug ? MSVC_DEBUG_LINK_FLAGS : MSVC_RELEASE_LINK_FLAGS;

    if (fastbuild)
    {
        char flags[512];
        snprintf(flags, sizeof flags, "%s %s %s", msvc_config, MSVC_COMMON_FLAGS, test_flag);
        build_incremental("cl ",
                          flags,
                          CAKE_SOURCE_FILES,
                          msvc_link,
                          " /Fo ",
                          " -o ",
                          "cake.exe");
    }
    else
    {
        char* cmd = calloc(2000, sizeof(char));

        snprintf(cmd, 2000, "cl %s%s%s -o cake.exe " CAKE_SOURCE_FILES "%s",
                 msvc_config,
                 MSVC_COMMON_FLAGS, 
                 test_flag,
                 msvc_link);

        execute_cmd(cmd);
        free(cmd);
    }

#ifndef CAKE_HEADERS
    execute_cmd("cake.exe -autoconfig");
#endif

    if (!fastbuild)
    {
        print_header("Run cake on its own source");

        
        execute_cmd("cake.exe -DTEST -const-literal -style=cake " CAKE_SOURCE_FILES);

        print_header("Build cake89");

#ifdef _WIN64
        echo_chdir("./x64_msvc/");
#else
        echo_chdir("./x86_msvc/");
#endif
        
        
        char* cmd = calloc(2000, sizeof(char));
        snprintf(cmd, 2000, "cl %s -o cake89.exe " CAKE_SOURCE_FILES, test_flag);
        execute_cmd(cmd);
        free(cmd);
        
        copy_file("cake89.exe", "../../src/cake89.exe");
        echo_chdir("../../src");
    }

#endif /* COMPILER_MSVC */

#if defined PLATFORM_WINDOWS && defined COMPILER_CLANG

    const char* clang_win_config = debug ? CLANG_WIN_DEBUG_FLAGS : CLANG_WIN_RELEASE_FLAGS;

    if (fastbuild)
    {
        char flags[512];
        snprintf(flags, sizeof flags, "%s%s%s", clang_win_config, CLANG_WIN_FLAGS, test_flag);
        build_incremental("clang",
                          flags,
                          CAKE_SOURCE_FILES,
                          "",
                          "-o ",
                          "-o ",
                          "cake.exe");
    }
    else
    {
        char cmd[512];
        snprintf(cmd, sizeof cmd, "clang %s%s%s -o cake.exe %s",
                 clang_win_config, CLANG_WIN_FLAGS, test_flag, CAKE_SOURCE_FILES);
        execute_cmd(cmd);
    }

    print_header("Run cake on its own source");
    execute_cmd("cake.exe -DTEST -style=cake " CAKE_SOURCE_FILES);

#endif /* PLATFORM_WINDOWS && COMPILER_CLANG */

#if (defined PLATFORM_LINUX || defined PLATFORM_MACOS) && defined COMPILER_CLANG

    const char* clang_unix_config = debug ? "" : " -DNDEBUG ";

    if (fastbuild)
    {
        char flags[512];
        snprintf(flags, sizeof flags, "%s%s%s", CLANG_UNIX_FLAGS, clang_unix_config, test_flag);
        build_incremental("clang",
                          flags,
                          CAKE_SOURCE_FILES,
                          "",
                          "-o ",
                          "-o ",
                          "cake");
    }
    else
    {
        char cmd[512];
        snprintf(cmd, sizeof cmd, "clang %s%s%s -o cake %s",
                 CLANG_UNIX_FLAGS, clang_unix_config, test_flag, CAKE_SOURCE_FILES);
        execute_cmd(cmd);
    }

#if !defined CAKE_HEADERS
    if (!fastbuild)
        execute_cmd("./cake -autoconfig");
#endif

#if defined PLATFORM_MACOS
    //cake is not ready to parse headers on macos yet, so we skip this test for now
#else
    if (!fastbuild)
        execute_cmd("./cake -fanalyzer " CAKE_SOURCE_FILES);
#endif

#endif /* (PLATFORM_LINUX || PLATFORM_MACOS) && COMPILER_CLANG */

#if defined COMPILER_GCC && !defined COMPILER_TINYC

    const char* gcc_config = debug ? "" : " -DNDEBUG ";

    if (fastbuild)
    {
        char flags[512];
        snprintf(flags, sizeof flags, "%s %s %s", GCC_FLAGS, gcc_config, test_flag);
        build_incremental("gcc",
                          flags,
                          CAKE_SOURCE_FILES,
                          "",
                          "-o ",
                          "-o ",
                          "cake");
    }
    else
    {
        char cmd[512];
        snprintf(cmd, sizeof cmd, "gcc %s %s %s -o cake %s",
                 GCC_FLAGS, gcc_config, test_flag, CAKE_SOURCE_FILES);
        execute_cmd(cmd);
    }

#ifndef CAKE_HEADERS
    if (!fastbuild)
        execute_cmd("./cake -autoconfig");
#endif

    if (!fastbuild)
    {
        print_header("Run cake on its own source");
        execute_cmd("./cake -DTEST -style=cake " CAKE_SOURCE_FILES);

        
        print_header("Build cake89");

        echo_chdir("./x86_x64_gcc/");        
        char* cmd = calloc(2000, sizeof(char));
        snprintf(cmd, 2000, "gcc %s -o cake89 " CAKE_SOURCE_FILES, test_flag);
        execute_cmd(cmd);
        free(cmd);

        execute_cmd("cp cake89 ../cake89");
        echo_chdir("../");
    }

#endif /* COMPILER_GCC && !COMPILER_TINYC */
}

static void run_tests(void)
{
    print_header("Run tests");

    execute_cmd(RUN "cake -selftest");
    execute_cmd(RUN "cake -fdiagnostics-color=never ../tests/en-cpp-reference-c/*.c -wd20 -test-mode");
    execute_cmd(RUN "cake  -fdiagnostics-color=never -wd20 ../tests/unit-tests/*.c -test-mode");
    execute_cmd(RUN "cake  -fdiagnostics-color=never -wd20 ../tests/output-test/*.c -test-mode-in-out");
    execute_cmd(RUN "cake  -fdiagnostics-color=never -E ../tests/preprocessor/*.c -test-mode-in-out");

    print_header("Run tests (cake89)");

    execute_cmd(RUN "cake89 -selftest");
    execute_cmd(RUN "cake89 -fdiagnostics-color=never ../tests/en-cpp-reference-c/*.c -wd20 -test-mode");
    execute_cmd(RUN "cake89  -fdiagnostics-color=never -wd20 ../tests/unit-tests/*.c -test-mode");
    execute_cmd(RUN "cake89  -fdiagnostics-color=never -wd20 ../tests/output-test/*.c -test-mode-in-out");
    execute_cmd(RUN "cake89  -fdiagnostics-color=never -E ../tests/preprocessor/*.c -test-mode-in-out");

    printf("Other test cases:\n");
    printf("  cake ../tests/unit-tests/failing/*.c -test-mode\n");
}

int main(int argc, char* argv[])
{
    int fastbuild = 0;
    int test = 0;
    int debug = 0;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "fast") == 0) fastbuild = 1;
        if (strcmp(argv[i], "test") == 0) test = 1;
        if (strcmp(argv[i], "debug") == 0) debug = 1;
    }

    const char* test_flag = test ? " -DTEST " : "";

    if (!fastbuild)
    {
        build_tools();
        build_docs();
        build_inner_tests();
        build_embedded_files();
        build_amalgamation();
    }

    build_cake(fastbuild, debug, test_flag);


    if (test)
        run_tests();

    return 0;
}