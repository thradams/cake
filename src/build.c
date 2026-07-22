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

#define CAKE_IDE_SOURCE_FILES \
    CAKE_LIB_SOURCE_FILES \
    " ide_ui.c " \
    " ide.c " \
    

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
  * Output binary names
  *---------------------------------------------------------------------------*/

#define CKC_NAME    "cake"
#define CKC89_NAME  "cake89"
#define CAKE_NAME   "cakeide"


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

    printf("\n");
    printf("============================================================\n");
    printf(" %s\n", text);
    printf("============================================================\n");

}

static void generate_doc(const char* mdfilename, const char* outfile)
{
    static const char header[] =
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "  <meta charset=\"utf-8\">\n"
        "  <link rel=\"stylesheet\" href=\"default.min.css\">\n"
        "  <script src=\"highlight.min.js\"></script>\n"
        "  <script>hljs.highlightAll();</script>\n"
        "  <link rel=\"stylesheet\" href=\"style.css\" />\n"
        "  <title>Cake C Compiler</title>\n"
        "  <meta name=\"description\" content=\"Cake C Compiler\">\n"
        "  <link rel=\"icon\" type=\"image/x-icon\" href=\"favicon.ico\">\n"
        "\n"        
        "  <script>\n"
        "    // Playground launcher\n"
        "    function launchPlayground(sourceCode) {\n"
        "      var link = \"./playground.html?code=\" + encodeURIComponent(btoa(sourceCode))\n"
        "               + \"&to=\" + encodeURI(\"-2\")\n"
        "               + \"&options=-flow3\";\n"
        "      window.open(link, '_blank');\n"
        "    }\n"
        "\n"
        "    // Copy to clipboard\n"
        "    async function copyToClipboard(text, btnElement) {\n"
        "      try {\n"
        "        await navigator.clipboard.writeText(text);\n"
        "        const originalText = btnElement.innerText;\n"
        "        btnElement.innerText = \"Copied!\";\n"
        "        setTimeout(() => {\n"
        "          btnElement.innerText = originalText;\n"
        "        }, 1500);\n"
        "      } catch (err) {\n"
        "        btnElement.innerText = \"Error\";\n"
        "        setTimeout(() => {\n"
        "          btnElement.innerText = \"Copy\";\n"
        "        }, 1000);\n"
        "      }\n"
        "    }\n"
        "\n"
        "    // Check if a code block is marked as runnable\n"
        "    // Supports:\n"
        "    // - HTML comment <!-- runnable --> immediately before <pre>\n"
        "    // - data-runnable=\"true\" attribute on <pre>\n"
        "    // - class=\"runnable\" on <pre> (legacy)\n"
        "    function isRunnable(preElement) {\n"
        "      // Check attribute or class\n"
        "      if (preElement.hasAttribute('data-runnable') && preElement.getAttribute('data-runnable') === 'true') return true;\n"
        "      if (preElement.classList && preElement.classList.contains('runnable')) return true;\n"
        "      \n"
        "      // Check previous sibling for comment\n"
        "      let prev = preElement.previousSibling;\n"
        "      while (prev && prev.nodeType === Node.TEXT_NODE && prev.textContent.trim() === '') {\n"
        "        prev = prev.previousSibling;\n"
        "      }\n"
        "      if (prev && prev.nodeType === Node.COMMENT_NODE) {\n"
        "        const commentText = prev.textContent.trim().toLowerCase();\n"
        "        if (commentText === 'runnable' || commentText === ' runnable ') {\n"
        "          // Optionally remove the comment so it doesn't clutter the DOM\n"
        "          prev.remove();\n"
        "          return true;\n"
        "        }\n"
        "      }\n"
        "      return false;\n"
        "    }\n"
        "\n"
        "    // Wrap each code block in a frame, add Copy (always) and Run (if runnable)\n"
        "    function enhanceCodeSnippets() {\n"
        "      const codeBlocks = Array.from(document.querySelectorAll('pre code.language-c'));\n"
        "      \n"
        "      for (const codeElement of codeBlocks) {\n"
        "        const preElement = codeElement.parentElement;\n"
        "        if (!preElement) continue;\n"
        "        if (preElement.closest('.code-frame')) continue;\n"
        "        \n"
        "        const codeText = codeElement.innerText;\n"
        "        const runnable = isRunnable(preElement);\n"
        "        \n"
        "        // Find original \"try\" button to remove\n"
        "        let tryButton = null;\n"
        "        let tryButtonContainer = null;\n"
        "        let sibling = preElement.nextElementSibling;\n"
        "        while (sibling && !tryButton) {\n"
        "          if (sibling.tagName === 'BUTTON' && sibling.getAttribute('onclick')?.includes('Try(this)')) {\n"
        "            tryButton = sibling;\n"
        "            tryButtonContainer = sibling.parentElement;\n"
        "            break;\n"
        "          }\n"
        "          const btn = sibling.querySelector('button[onclick*=\"Try(this)\"]');\n"
        "          if (btn) {\n"
        "            tryButton = btn;\n"
        "            tryButtonContainer = sibling;\n"
        "            break;\n"
        "          }\n"
        "          sibling = sibling.nextElementSibling;\n"
        "        }\n"
        "        \n"
        "        // Save position before DOM changes\n"
        "        const parent = preElement.parentNode;\n"
        "        const nextSibling = preElement.nextSibling;\n"
        "        \n"
        "        // Create frame\n"
        "        const frameDiv = document.createElement('div');\n"
        "        frameDiv.className = 'code-frame';\n"
        "        \n"
        "        // Move pre element into frame\n"
        "        preElement.remove();\n"
        "        frameDiv.appendChild(preElement);\n"
        "        \n"
        "        // Create action buttons\n"
        "        const actionsDiv = document.createElement('div');\n"
        "        actionsDiv.className = 'code-actions';\n"
        "        \n"
        "        // Copy button (always)\n"
        "        const copyBtn = document.createElement('button');\n"
        "        copyBtn.innerText = 'Copy';\n"
        "        copyBtn.title = 'Copy code to clipboard';\n"
        "        copyBtn.addEventListener('click', (e) => {\n"
        "          e.stopPropagation();\n"
        "          copyToClipboard(codeText, copyBtn);\n"
        "        });\n"
        "        actionsDiv.appendChild(copyBtn);\n"
        "        \n"
        "        // Run button (only if marked runnable)\n"
        "        if (runnable) {\n"
        "          const runBtn = document.createElement('button');\n"
        "          runBtn.innerText = 'Run';\n"
        "          runBtn.title = 'Run in Playground';\n"
        "          runBtn.addEventListener('click', (e) => {\n"
        "            e.stopPropagation();\n"
        "            launchPlayground(codeText);\n"
        "          });\n"
        "          actionsDiv.appendChild(runBtn);\n"
        "        }\n"
        "        \n"
        "        frameDiv.appendChild(actionsDiv);\n"
        "        \n"
        "        // Insert frame\n"
        "        if (parent) {\n"
        "          parent.insertBefore(frameDiv, nextSibling);\n"
        "        } else {\n"
        "          const article = document.querySelector('article');\n"
        "          if (article) article.appendChild(frameDiv);\n"
        "        }\n"
        "        \n"
        "        // Remove original try button and empty container\n"
        "        if (tryButton) {\n"
        "          tryButton.remove();\n"
        "          if (tryButtonContainer && tryButtonContainer.children.length === 0 && !tryButtonContainer.innerText.trim()) {\n"
        "            tryButtonContainer.remove();\n"
        "          }\n"
        "        }\n"
        "      }\n"
        "    }\n"
        "    \n"
        "    document.addEventListener('DOMContentLoaded', () => {\n"
        "      enhanceCodeSnippets();\n"
        "      if (typeof hljs !== 'undefined' && hljs.highlightAll) {\n"
        "        hljs.highlightAll();\n"
        "      }\n"
        "    });\n"
        "    \n"        
        "  </script>\n"
        "\n"
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
             RUN EXE("hoedown") " --html-toc --toc-level 3 --autolink --fenced-code %s >> %s",
             mdfilename, outfile);
    execute_cmd(cmd);

    snprintf(cmd, sizeof cmd,
             RUN EXE("hoedown") " --toc-level 3 --autolink --fenced-code %s >> %s",
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
                CC_OUTPUT(EXE("install")));
#else
    execute_cmd(CC " -D_CRT_SECURE_NO_WARNINGS install.c " CC_OUTPUT(EXE("install")));
#endif

    execute_cmd(CC " install.c " CC_OUTPUT(EXE("install")));

    echo_chdir("./tools");
    execute_cmd(CC " -D_CRT_SECURE_NO_WARNINGS maketest.c "           CC_OUTPUT("../" EXE("maketest")));
    execute_cmd(CC " -D_CRT_SECURE_NO_WARNINGS amalgamator.c "        CC_OUTPUT("../" EXE("amalgamator")));
    execute_cmd(CC " -D_CRT_SECURE_NO_WARNINGS -I.. embed.c ../fs.c ../error.c "
                CC_OUTPUT("../" EXE("embed")));

    echo_chdir("./hoedown");
    execute_cmd(CC HOEDOWN_SOURCE_FILES CC_OUTPUT("../../" EXE("hoedown")));

    echo_chdir("../..");
}

static void build_docs(void)
{
    print_header("Build docs");

    generate_doc("../manual.md", "./web/manual.html");
    generate_doc("../README.md", "./web/index.html");
    generate_doc("../warnings.md", "./web/warnings.html");
    generate_doc("../flow3.md", "./web/flow3.html");
    generate_doc("../ownership.md", "./web/ownership.html");

    remove(EXE("hoedown"));
}

static void build_inner_tests(void)
{
    print_header("Build inner tests");
    execute_cmd(RUN EXE("maketest") " unit_test.c " CAKE_SOURCE_FILES);
    remove(EXE("maketest"));
}

static void build_embedded_files(void)
{
    print_header("Build embedded files");
    execute_cmd(RUN EXE("embed") " ./include/" );
}

static void build_amalgamation(void)
{
    print_header("Build amalgamated file");
    execute_cmd(RUN EXE("amalgamator") " -olib.c " CAKE_LIB_SOURCE_FILES);
    remove(EXE("amalgamator"));
}

static time_t get_mtime(const char* path)
{
    struct stat st;
    if (stat(path, &st) != 0)
        return 0;
    return st.st_mtime;
}

/*
 * main.c does `#include "unit_test.c"` when TEST is defined, so main.o's
 * real dependencies include unit_test.c even though build_incremental only
 * looks at main.c's own timestamp. When building with the test flag,
 * force main.c to be recompiled if unit_test.c changed more recently than
 * main.o, otherwise incremental builds can silently keep stale test code.
 */
static void refresh_test_dependency(int test)
{
    if (!test)
        return;

    if (get_mtime("unit_test.c") > get_mtime("main.o"))
        remove("main.o");
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

    const int test = (*test_flag != '\0');

#if defined COMPILER_MSVC

    const char* msvc_config = debug ? MSVC_DEBUG_CONFIG_FLAGS : MSVC_RELEASE_CONFIG_FLAGS;
    const char* msvc_link = debug ? MSVC_DEBUG_LINK_FLAGS : MSVC_RELEASE_LINK_FLAGS;

    if (fastbuild)
    {
        char flags[512];
        char ide_flags[512];
        /* The IDE (cake) never links unit_test.c, so it must never be
         * compiled with -DTEST - doing so pulls in TEST-guarded test
         * functions (via the assert() macro in unit_test.h) that reference
         * g_unit_test_error_count/g_unit_test_success_count, which are only
         * defined when main.c's include of unit_test.c is compiled in.
         * Linking those into cake.exe fails with unresolved externals. */
        snprintf(flags, sizeof flags, "%s %s %s", msvc_config, MSVC_COMMON_FLAGS, test_flag);
        snprintf(ide_flags, sizeof ide_flags, "%s %s", msvc_config, MSVC_COMMON_FLAGS);
        refresh_test_dependency(test);
        build_incremental("cl ",
                          flags,
                          CAKE_SOURCE_FILES,
                          msvc_link,
                          " /Fo ",
                          " -o ",
                          EXE(CKC_NAME));
        /* Also incrementally build the IDE when doing a fast build. */
        build_incremental("cl ",
                          ide_flags,
                          CAKE_IDE_SOURCE_FILES,
                          msvc_link,
                          " ../vc/ide/ide.res /Fo ",
                          " -o ",
                          EXE(CAKE_NAME));
    }
    else
    {
        char* cmd = calloc(2000, sizeof(char));

        snprintf(cmd, 2000, "cl %s%s%s -o " EXE(CKC_NAME) CAKE_SOURCE_FILES "%s ",
                 msvc_config,
                 MSVC_COMMON_FLAGS,
                 test_flag,
                 msvc_link);

        execute_cmd(cmd);


        print_header("Build cake IDE");


        execute_cmd("rc ../vc/ide/ide.rc");
        /* No test_flag here: the IDE doesn't link unit_test.c, so building
         * it with -DTEST leaves g_unit_test_error_count/success_count
         * unresolved at link time (see comment in the fastbuild branch). */
        snprintf(cmd, 2000, "cl %s%s  -o " EXE(CAKE_NAME) " ide_win32.c ../vc/ide/ide.res  %s",
                 MSVC_COMMON_FLAGS, msvc_config, CAKE_IDE_SOURCE_FILES);


        execute_cmd(cmd);

        free(cmd);
    }

#ifndef CAKE_HEADERS
    execute_cmd(EXE(CKC_NAME) " -autoconfig");
#endif

    if (!fastbuild)
    {
        print_header("Run cake on its own source");


        execute_cmd(EXE(CKC_NAME) " -DTEST -const-literal -style=cake " CAKE_SOURCE_FILES);

        print_header("Build cake89");

#ifdef _WIN64
        echo_chdir("./x64_msvc/");
#else
        echo_chdir("./x86_msvc/");
#endif


        char* cmd = calloc(2000, sizeof(char));
        snprintf(cmd, 2000, "cl %s -o " EXE(CKC89_NAME) " " CAKE_SOURCE_FILES, test_flag);
        execute_cmd(cmd);
        free(cmd);

        copy_file(EXE(CKC89_NAME), "../../src/" EXE(CKC89_NAME));
        echo_chdir("../../src");
    }

#endif /* COMPILER_MSVC */

#if defined PLATFORM_WINDOWS && defined COMPILER_CLANG

    const char* clang_win_config = debug ? CLANG_WIN_DEBUG_FLAGS : CLANG_WIN_RELEASE_FLAGS;

    if (fastbuild)
    {
        char flags[512];
        char ide_flags[512];
        /* No test_flag for the IDE build - see comment in the MSVC branch. */
        snprintf(flags, sizeof flags, "%s%s%s", clang_win_config, CLANG_WIN_FLAGS, test_flag);
        snprintf(ide_flags, sizeof ide_flags, "%s%s", clang_win_config, CLANG_WIN_FLAGS);
        refresh_test_dependency(test);
        build_incremental("clang",
                          flags,
                          CAKE_SOURCE_FILES,
                          "",
                          "-o ",
                          "-o ",
                          EXE(CKC_NAME));
        /* Also build the IDE incrementally on fast builds. */
        build_incremental("clang",
                          ide_flags,
                          CAKE_IDE_SOURCE_FILES,
                          "",
                          "-o ",
                          "-o ",
                          EXE(CAKE_NAME));
    }
    else
    {
        char cmd[512];
        snprintf(cmd, sizeof cmd, "clang %s%s%s -o " EXE(CKC_NAME) " %s",
                 clang_win_config, CLANG_WIN_FLAGS, test_flag, CAKE_SOURCE_FILES);
        execute_cmd(cmd);
    }

    print_header("Run cake on its own source");
    execute_cmd(EXE(CKC_NAME) " -DTEST -style=cake " CAKE_SOURCE_FILES);

#endif /* PLATFORM_WINDOWS && COMPILER_CLANG */

#if (defined PLATFORM_LINUX || defined PLATFORM_MACOS) && defined COMPILER_CLANG

    const char* clang_unix_config = debug ? "" : " -DNDEBUG ";

    if (fastbuild)
    {
        char flags[512];
        char ide_flags[512];
        /* No test_flag for the IDE build - see comment in the MSVC branch. */
        snprintf(flags, sizeof flags, "%s%s%s", CLANG_UNIX_FLAGS, clang_unix_config, test_flag);
        snprintf(ide_flags, sizeof ide_flags, "%s%s", CLANG_UNIX_FLAGS, clang_unix_config);
        refresh_test_dependency(test);
        build_incremental("clang",
                          flags,
                          CAKE_SOURCE_FILES,
                          "",
                          "-o ",
                          "-o ",
                          CKC_NAME);
        /* Also incrementally build the IDE on fast builds using the
         * platform-specific frontend file. */
#if defined PLATFORM_MACOS
        build_incremental("clang",
                          ide_flags,
                          "ide_cocoa.c " CAKE_IDE_SOURCE_FILES,
                          " -framework Cocoa -framework CoreText -framework CoreGraphics -lobjc ",
                          "-o ",
                          "-o ",
                          EXE(CAKE_NAME));
#else
        /* Xft.h pulls in <ft2build.h>, which on Debian/Ubuntu lives under
         * /usr/include/freetype2 rather than directly on the default
         * include path, so it must be added explicitly. */
        char ide_flags_x11[560];
        snprintf(ide_flags_x11, sizeof ide_flags_x11, "%s -I/usr/include/freetype2 ", ide_flags);
        build_incremental("clang",
                          ide_flags_x11,
                          "ide_x11.c " CAKE_IDE_SOURCE_FILES,
                          " -lX11 -lXft -lXrender -lfreetype ",
                          "-o ",
                          "-o ",
                          EXE(CAKE_NAME));
#endif
    }
    else
    {
        char cmd[512];
        snprintf(cmd, sizeof cmd, "clang %s%s%s -o " CKC_NAME " %s",
                 CLANG_UNIX_FLAGS, clang_unix_config, test_flag, CAKE_SOURCE_FILES);
        execute_cmd(cmd);

        /* Build IDE: use Cocoa frontend on macOS, X11 frontend on Linux.
         * No test_flag here - see comment in the MSVC branch. */
    #if defined PLATFORM_MACOS
        snprintf(cmd, sizeof cmd, "clang %s%s  ide_cocoa.c  -framework Cocoa -framework CoreText -framework CoreGraphics -lobjc -o " EXE(CAKE_NAME) " %s",
             CLANG_UNIX_FLAGS, clang_unix_config, CAKE_IDE_SOURCE_FILES);
        execute_cmd(cmd);
    #else
        /* Xft.h pulls in <ft2build.h>, which on Debian/Ubuntu lives under
         * /usr/include/freetype2 rather than directly on the default
         * include path, so it must be added explicitly. */
        snprintf(cmd, sizeof cmd, "clang %s%s -I/usr/include/freetype2  ide_x11.c %s -o " EXE(CAKE_NAME) " %s",
             CLANG_UNIX_FLAGS, clang_unix_config, "-lX11 -lXft -lXrender -lfreetype", CAKE_IDE_SOURCE_FILES);
        execute_cmd(cmd);
    #endif
    }

#if !defined CAKE_HEADERS
    if (!fastbuild)
    {
        print_header("Cake auto-config");
        execute_cmd("./" CKC_NAME " -autoconfig");
    }
#endif

    /* Run cake on its own source (self-analysis) on Linux and macOS. */
    if (!fastbuild)
    {
        print_header("Running Cake on its own source");
        execute_cmd("./" CKC_NAME " -fanalyzer " CAKE_SOURCE_FILES);
    }
#endif /* (PLATFORM_LINUX || PLATFORM_MACOS) && COMPILER_CLANG */

#if defined COMPILER_GCC && !defined COMPILER_TINYC

    const char* gcc_config = debug ? "" : " -DNDEBUG ";

    if (fastbuild)
    {
        char flags[512];
        char ide_flags[512];
        /* No test_flag for the IDE build - see comment in the MSVC branch. */
        snprintf(flags, sizeof flags, "%s %s %s", GCC_FLAGS, gcc_config, test_flag);
        snprintf(ide_flags, sizeof ide_flags, "%s %s", GCC_FLAGS, gcc_config);
        refresh_test_dependency(test);
        build_incremental("gcc",
                          flags,
                          CAKE_SOURCE_FILES,
                          "",
                          "-o ",
                          "-o ",
                          CKC_NAME);
        /* Also incrementally build the IDE when doing a fast build. */
    #if defined PLATFORM_MACOS
        build_incremental("gcc",
                  ide_flags,
                  "ide_cocoa.c " CAKE_IDE_SOURCE_FILES,
                  " -framework Cocoa -framework CoreText -framework CoreGraphics -lobjc ",
                  "-o ",
                  "-o ",
                  EXE(CAKE_NAME));
    #else
        /* Xft.h pulls in <ft2build.h>, which on Debian/Ubuntu lives under
         * /usr/include/freetype2 rather than directly on the default
         * include path, so it must be added explicitly. */
        char ide_flags_x11[560];
        snprintf(ide_flags_x11, sizeof ide_flags_x11, "%s -I/usr/include/freetype2 ", ide_flags);
        build_incremental("gcc",
                  ide_flags_x11,
                  "ide_x11.c " CAKE_IDE_SOURCE_FILES,
                  " -lX11 -lXft -lXrender -lfreetype ",
                  "-o ",
                  "-o ",
                  EXE(CAKE_NAME));
    #endif
    }
    else
    {
        char cmd[512];
        snprintf(cmd, sizeof cmd, "gcc %s %s %s -o " CKC_NAME " %s",
                 GCC_FLAGS, gcc_config, test_flag, CAKE_SOURCE_FILES);
        execute_cmd(cmd);

        /* Build IDE: use Cocoa frontend on macOS, X11 frontend on Linux.
         * No test_flag here - see comment in the MSVC branch. */
        print_header("Build cake IDE");
    #if defined PLATFORM_MACOS
        snprintf(cmd, sizeof cmd, "gcc %s %s  ide_cocoa.c  -framework Cocoa -framework CoreText -framework CoreGraphics -lobjc -o " EXE(CAKE_NAME) " %s",
             GCC_FLAGS, gcc_config, CAKE_IDE_SOURCE_FILES);
        execute_cmd(cmd);
    #else
        /* Xft.h pulls in <ft2build.h>, which on Debian/Ubuntu lives under
         * /usr/include/freetype2 rather than directly on the default
         * include path, so it must be added explicitly. */
        snprintf(cmd, sizeof cmd, "gcc %s %s -I/usr/include/freetype2  ide_x11.c %s -o " EXE(CAKE_NAME) " %s",
             GCC_FLAGS, gcc_config, "-lX11 -lXft -lXrender -lfreetype", CAKE_IDE_SOURCE_FILES);
        execute_cmd(cmd);
    #endif
    }

#ifndef CAKE_HEADERS
    if (!fastbuild)
        execute_cmd("./" CKC_NAME " -autoconfig");
#endif

    if (!fastbuild)
    {
        print_header("Run cake on its own source");
        execute_cmd("./" CKC_NAME " -DTEST -style=cake " CAKE_SOURCE_FILES);


        print_header("Build cake89");

        echo_chdir("./x86_x64_gcc/");
        char* cmd = calloc(2000, sizeof(char));
        snprintf(cmd, 2000, "gcc %s -o " CKC89_NAME " " CAKE_SOURCE_FILES, test_flag);
        execute_cmd(cmd);
        free(cmd);

        execute_cmd("cp " CKC89_NAME " ../" CKC89_NAME);
        echo_chdir("../");
    }

#endif /* COMPILER_GCC && !COMPILER_TINYC */
}

static void run_tests(void)
{
    print_header("Run tests");

    execute_cmd(RUN CKC_NAME " -selftest");

    execute_cmd(RUN CKC_NAME " -fdiagnostics-color=never ../tests/en-cpp-reference-c/*.c -wd20 -test-mode");
    execute_cmd(RUN CKC_NAME "  -fdiagnostics-color=never -wd20 ../tests/unit-tests/*.c -test-mode");
    execute_cmd(RUN CKC_NAME "  -fdiagnostics-color=never -wd20 ../tests/output-test/*.c -test-mode-in-out");
    execute_cmd(RUN CKC_NAME "  -fdiagnostics-color=never -E ../tests/preprocessor/*.c -test-mode-in-out");

#if !PLATFORM_MACOS
    /* cake89 (C89) is not built in the macOS/clang branch, so its tests
       only run where it is produced (MSVC / GCC). */
    print_header("Run tests (cake89)");

    execute_cmd(RUN CKC89_NAME " -selftest");
    execute_cmd(RUN CKC89_NAME " -fdiagnostics-color=never ../tests/en-cpp-reference-c/*.c -wd20 -test-mode");
    execute_cmd(RUN CKC89_NAME "  -fdiagnostics-color=never -wd20 ../tests/unit-tests/*.c -test-mode");
    execute_cmd(RUN CKC89_NAME "  -fdiagnostics-color=never -wd20 ../tests/output-test/*.c -test-mode-in-out");
    execute_cmd(RUN CKC89_NAME "  -fdiagnostics-color=never -E ../tests/preprocessor/*.c -test-mode-in-out");
#endif

    printf("Other test cases:\n");
    printf("  " CKC_NAME " ../tests/unit-tests/failing/*.c -test-mode\n");
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
