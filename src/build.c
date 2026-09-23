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
 *   ./build full             (build everything with -DTEST, but do not run tests)
 *   ./build test             (same as full, and run the tests afterwards)
 *   ./build debug            (debug build: no optimization, debug runtime)
 *   ./build fast debug test  (flags combine freely)
 *   ./build test -cake-headers (cake runs with its bundled headers: self
 *                             analysis, cake89 bootstrap and the test suites)
 */

#include "build.h"
#include "version.h"
#include <sys/stat.h>
#include <ctype.h>


 /*---------------------------------------------------------------------------
  * Source file lists
  *---------------------------------------------------------------------------*/

#define CAKE_LIB_SOURCE_FILES \
    " json.c "                \
    " token.c "               \
    " fp_to_string.c "        \
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
    " flow_alternative.c "    \
    " flow_branch.c "            \
    " flow.c "               \
    " error.c "               \
    " target.c "              \
    " type.c "

#define CAKE_SOURCE_FILES \
    CAKE_LIB_SOURCE_FILES \
    " main.c "

#define CAKE_IDE_SOURCE_FILES \
    CAKE_LIB_SOURCE_FILES \
    " ide_lsp.c " \
    " ide_ui.c " \
    " ide_debug.c " \
    " ide.c " \
    " tinycthread.c " \


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
    const int header_width = 80;
    char upper[256];
    size_t length = 0;
    int left_padding = 0;
    int i = 0;

    for (; text[length] != '\0' && length < sizeof(upper) - 1; length++)
    {
        upper[length] = (char)toupper((unsigned char)text[length]);
    }
    upper[length] = '\0';

    if ((int)length < header_width)
    {
        left_padding = (header_width - (int)length) / 2;
    }

    printf("\n");
    for (i = 0; i < header_width; i++)
    {
        printf("=");
    }
    printf("\n");
    printf("%*s%s\n", left_padding, "", upper);
    for (i = 0; i < header_width; i++)
    {
        printf("=");
    }
    printf("\n\n");

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
        "               + \"&options=\";\n"
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
        " | <a href=\"flow.html\">Static Analysis</a>"
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
             RUN EXE("hoedown") " --html-toc --toc-level 3 --tables --autolink --fenced-code %s >> %s",
             mdfilename, outfile);
    execute_cmd(cmd);

    snprintf(cmd, sizeof cmd,
             RUN EXE("hoedown") " --toc-level 3 --tables --autolink --fenced-code %s >> %s",
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
    generate_doc("../idemanual.md", "./web/idemanual.html");
    generate_doc("../README.md", "./web/index.html");
    generate_doc("../diagnostics.md", "./web/diagnostics.html");
    generate_doc("../flow.md", "./web/flow.html");

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
    execute_cmd(RUN EXE("embed") " ./include" );
    execute_cmd(RUN EXE("embed") " ./include/builtins" );
}

static void build_amalgamation(void)
{
    print_header("Build amalgamated file (lib.c)");
    execute_cmd(RUN EXE("amalgamator") " -olib.c " CAKE_LIB_SOURCE_FILES);
    remove(EXE("amalgamator"));
}

static void build_web_samples(void)
{
    /* Keep web samples in sync with the samples folder */
    print_header("Build web samples (samples.js)");

    echo_chdir("./tools");
    execute_cmd(CC " -D_CRT_SECURE_NO_WARNINGS makesamples.c " CC_OUTPUT("../" EXE("makesamples")));
    echo_chdir("..");
    execute_cmd(RUN EXE("makesamples") " ./samples");
#ifdef _WIN32
    execute_cmd("copy samples.js web\\samples.js");
#else
    execute_cmd("cp samples.js web/samples.js");
#endif
    remove(EXE("makesamples"));
    remove("samples.js");
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
    int  any_changed = 0;

    /* On the heap, not the stack: the object list holds every .o name in the
     * build and the link command holds all of that PLUS the surrounding
     * flags, so together they are far too big to park in a stack frame.
     * cmd must stay larger than obj_list - the link line below is
     * "compiler out_flag output <obj_list> link_flags", so if it were the
     * smaller of the two a long object list would be silently truncated
     * into a broken link command (which is what -Wformat-truncation was
     * warning about when cmd was 2048 and obj_list 4096). */
    enum { OBJ_LIST_SIZE = 8192, CMD_SIZE = OBJ_LIST_SIZE * 2 };
    char* obj_list = malloc(OBJ_LIST_SIZE);
    char* cmd = malloc(CMD_SIZE);
    if (!obj_list || !cmd)
    {
        printf("out of memory\n");
        free(obj_list);
        free(cmd);
        return;
    }

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

        /* derive .o name: "flow.c" -> "flow.o" */
        snprintf(obj, sizeof obj, "%s", src);
        char* dot = strrchr(obj, '.');
        if (dot) strcpy(dot, ".o");
        else      strcat(obj, ".o");

        /* append to object list (bounded - obj_list is a plain pointer
         * now, so there is no sizeof to lean on) */
        size_t used = strlen(obj_list);
        snprintf(obj_list + used, OBJ_LIST_SIZE - used, " %s", obj);

        /* compile if .o missing or .c newer */
        if (get_mtime(src) > get_mtime(obj))
        {
            printf("compiling: %s\n", src);
            snprintf(cmd, CMD_SIZE, "%s %s -c %s %s%s",
                     compiler, compile_flags, src, obj_flag, obj);
            execute_cmd(cmd);
            any_changed = 1;
        }
    }

    /* link if anything changed or output missing */
    if (any_changed || get_mtime(output) == 0)
    {
        printf("linking: %s\n", output);
        snprintf(cmd, CMD_SIZE, "%s %s%s %s %s",
                 compiler, out_flag, output, obj_list, link_flags);
        execute_cmd(cmd);
    }
    else
    {
        printf("No sources changed, skipping.\n");
    }

    free(obj_list);
    free(cmd);
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
        snprintf(flags, sizeof flags, "%s %s %s", msvc_config, MSVC_COMMON_FLAGS, test_flag);
        refresh_test_dependency(test);
        build_incremental("cl ",
                          flags,
                          CAKE_SOURCE_FILES,
                          msvc_link,
                          " /Fo ",
                          " -o ",
                          EXE(CKC_NAME));
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

        free(cmd);
    }

#endif /* COMPILER_MSVC */

#if defined PLATFORM_WINDOWS && defined COMPILER_CLANG

    const char* clang_win_config = debug ? CLANG_WIN_DEBUG_FLAGS : CLANG_WIN_RELEASE_FLAGS;

    if (fastbuild)
    {
        char flags[512];
        snprintf(flags, sizeof flags, "%s%s%s", clang_win_config, CLANG_WIN_FLAGS, test_flag);
        refresh_test_dependency(test);
        build_incremental("clang",
                          flags,
                          CAKE_SOURCE_FILES,
                          "",
                          "-o ",
                          "-o ",
                          EXE(CKC_NAME));
    }
    else
    {
        char cmd[512];
        snprintf(cmd, sizeof cmd, "clang %s%s%s -o " EXE(CKC_NAME) " %s",
                 clang_win_config, CLANG_WIN_FLAGS, test_flag, CAKE_SOURCE_FILES);
        execute_cmd(cmd);
    }

#endif /* PLATFORM_WINDOWS && COMPILER_CLANG */

#if (defined PLATFORM_LINUX || defined PLATFORM_MACOS) && defined COMPILER_CLANG

    const char* clang_unix_config = debug ? "" : " -DNDEBUG -O2 ";

    if (fastbuild)
    {
        char flags[512];
        snprintf(flags, sizeof flags, "%s%s%s", CLANG_UNIX_FLAGS, clang_unix_config, test_flag);
        refresh_test_dependency(test);
        build_incremental("clang",
                          flags,
                          CAKE_SOURCE_FILES,
                          "",
                          "-o ",
                          "-o ",
                          CKC_NAME);
    }
    else
    {
        char cmd[512];
        snprintf(cmd, sizeof cmd, "clang %s%s%s -o " CKC_NAME " %s",
                 CLANG_UNIX_FLAGS, clang_unix_config, test_flag, CAKE_SOURCE_FILES);
        execute_cmd(cmd);
    }

#endif /* (PLATFORM_LINUX || PLATFORM_MACOS) && COMPILER_CLANG */

#if defined COMPILER_GCC && !defined COMPILER_TINYC

    const char* gcc_config = debug ? "" : " -DNDEBUG -O2 ";

    if (fastbuild)
    {
        char flags[512];
        snprintf(flags, sizeof flags, "%s %s %s", GCC_FLAGS, gcc_config, test_flag);
        refresh_test_dependency(test);
        build_incremental("gcc",
                          flags,
                          CAKE_SOURCE_FILES,
                          "",
                          "-o ",
                          "-o ",
                          CKC_NAME);
    }
    else
    {
        char cmd[512];
        snprintf(cmd, sizeof cmd, "gcc %s %s %s -o " CKC_NAME " %s",
                 GCC_FLAGS, gcc_config, test_flag, CAKE_SOURCE_FILES);
        execute_cmd(cmd);
    }

#endif /* COMPILER_GCC && !COMPILER_TINYC */
}

/*
 * The IDE never links unit_test.c, so it must never be compiled with -DTEST -
 * doing so pulls in TEST-guarded test functions (via the assert() macro in
 * unit_test.h) that reference g_unit_test_error_count/g_unit_test_success_count,
 * which are only defined when main.c's include of unit_test.c is compiled in.
 * Linking those into the IDE fails with unresolved externals.
 * That is why this function takes no test_flag.
 */
static void build_cake_ide(int fastbuild, int debug)
{
    print_header("Build cake IDE");

#if defined COMPILER_MSVC

    const char* msvc_config = debug ? MSVC_DEBUG_CONFIG_FLAGS : MSVC_RELEASE_CONFIG_FLAGS;
    const char* msvc_link = debug ? MSVC_DEBUG_LINK_FLAGS : MSVC_RELEASE_LINK_FLAGS;

    if (fastbuild)
    {
        char ide_flags[512];
        snprintf(ide_flags, sizeof ide_flags, "%s %s", msvc_config, MSVC_COMMON_FLAGS);
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
        execute_cmd("rc ../vc/ide/ide.rc");

        char* cmd = calloc(2000, sizeof(char));
        snprintf(cmd, 2000, "cl %s%s  -o " EXE(CAKE_NAME) " ide_win32.c ../vc/ide/ide.res  %s",
                 MSVC_COMMON_FLAGS, msvc_config, CAKE_IDE_SOURCE_FILES);
        execute_cmd(cmd);
        free(cmd);
    }

#endif /* COMPILER_MSVC */

#if defined PLATFORM_WINDOWS && defined COMPILER_CLANG

    const char* clang_win_config = debug ? CLANG_WIN_DEBUG_FLAGS : CLANG_WIN_RELEASE_FLAGS;

    /* With clang on Windows the IDE is only built incrementally. */
    if (fastbuild)
    {
        char ide_flags[512];
        snprintf(ide_flags, sizeof ide_flags, "%s%s", clang_win_config, CLANG_WIN_FLAGS);
        build_incremental("clang",
                          ide_flags,
                          CAKE_IDE_SOURCE_FILES,
                          "",
                          "-o ",
                          "-o ",
                          EXE(CAKE_NAME));
    }

#endif /* PLATFORM_WINDOWS && COMPILER_CLANG */

#if (defined PLATFORM_LINUX || defined PLATFORM_MACOS) && defined COMPILER_CLANG

    const char* clang_unix_config = debug ? "" : " -DNDEBUG -O2 ";

    if (fastbuild)
    {
        char ide_flags[512];
        snprintf(ide_flags, sizeof ide_flags, "%s%s", CLANG_UNIX_FLAGS, clang_unix_config);
        /* Use the platform-specific frontend file. */
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
        /* -lpthread: tinycthread.c (the compile now runs on a worker thread -
         * see compile_stream_start in ide.c) is pthreads underneath on
         * POSIX. glibc 2.34+ folded pthread into libc so this links without
         * it on new distros, but older ones still need it explicitly. */
        build_incremental("clang",
                          ide_flags_x11,
                          "ide_x11.c " CAKE_IDE_SOURCE_FILES,
                          " -lX11 -lXft -lXrender -lfreetype -lpthread ",
                          "-o ",
                          "-o ",
                          EXE(CAKE_NAME));
#endif
    }
    else
    {
        char cmd[512];
        /* Use Cocoa frontend on macOS, X11 frontend on Linux. */
    #if defined PLATFORM_MACOS
        snprintf(cmd, sizeof cmd, "clang %s%s  ide_cocoa.c  -framework Cocoa -framework CoreText -framework CoreGraphics -lobjc -o " EXE(CAKE_NAME) " %s",
             CLANG_UNIX_FLAGS, clang_unix_config, CAKE_IDE_SOURCE_FILES);
        execute_cmd(cmd);
    #else
        /* Xft.h pulls in <ft2build.h>, which on Debian/Ubuntu lives under
         * /usr/include/freetype2 rather than directly on the default
         * include path, so it must be added explicitly. */
        snprintf(cmd, sizeof cmd, "clang %s%s -I/usr/include/freetype2  ide_x11.c %s -o " EXE(CAKE_NAME) " %s",
             CLANG_UNIX_FLAGS, clang_unix_config, "-lX11 -lXft -lXrender -lfreetype -lpthread", CAKE_IDE_SOURCE_FILES);
        execute_cmd(cmd);
    #endif
    }

#endif /* (PLATFORM_LINUX || PLATFORM_MACOS) && COMPILER_CLANG */

#if defined COMPILER_GCC && !defined COMPILER_TINYC

    const char* gcc_config = debug ? "" : " -DNDEBUG -O2 ";

    if (fastbuild)
    {
        char ide_flags[512];
        snprintf(ide_flags, sizeof ide_flags, "%s %s", GCC_FLAGS, gcc_config);
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
                  " -lX11 -lXft -lXrender -lfreetype -lpthread ",
                  "-o ",
                  "-o ",
                  EXE(CAKE_NAME));
    #endif
    }
    else
    {
        char cmd[512];
        /* Use Cocoa frontend on macOS, X11 frontend on Linux. */
    #if defined PLATFORM_MACOS
        snprintf(cmd, sizeof cmd, "gcc %s %s  ide_cocoa.c  -framework Cocoa -framework CoreText -framework CoreGraphics -lobjc -o " EXE(CAKE_NAME) " %s",
             GCC_FLAGS, gcc_config, CAKE_IDE_SOURCE_FILES);
        execute_cmd(cmd);
    #else
        /* Xft.h pulls in <ft2build.h>, which on Debian/Ubuntu lives under
         * /usr/include/freetype2 rather than directly on the default
         * include path, so it must be added explicitly. */
        snprintf(cmd, sizeof cmd, "gcc %s %s -I/usr/include/freetype2  ide_x11.c %s -o " EXE(CAKE_NAME) " %s",
             GCC_FLAGS, gcc_config, "-lX11 -lXft -lXrender -lfreetype -lpthread", CAKE_IDE_SOURCE_FILES);
        execute_cmd(cmd);
    #endif
    }

#endif /* COMPILER_GCC && !COMPILER_TINYC */
}

static void generate_config(void)
{
    print_header("Generate config (cake.json)");
    execute_cmd(RUN EXE(CKC_NAME) " -autoconfig");
}

/*
 * Windows: cake-<version>-setup.exe (files listed in tools/win_installer.h)
 * Linux / macOS: cake-<version>-<os>.tar.gz with tools/unix_install.sh
 */
static void build_installer(void)
{
#if defined COMPILER_MSVC
    print_header("Build installer (cake-" CAKE_VERSION "-setup.exe)");

    echo_chdir("./tools");
    /* not named *install*: Windows asks elevation for 32-bit exes with that name */
    execute_cmd(CC " -nologo win_installer.c -Fe:win_pack.exe");
    execute_cmd(RUN "win_pack.exe ..\\cake-" CAKE_VERSION "-setup.exe");
    remove("win_installer.obj");
    remove("win_pack.exe");
    echo_chdir("..");
#endif

#if defined PLATFORM_LINUX || defined PLATFORM_MACOS
    /*
     * cake-<version>-<os>.tar.gz with the files and install.sh (tools/unix_install.sh):
     *   tar xzf cake-<version>-<os>.tar.gz
     *   cd cake-<version>-<os> && sudo ./install.sh
     */
#if defined PLATFORM_MACOS
#define PACKAGE_NAME "cake-" CAKE_VERSION "-macos"
#else
#define PACKAGE_NAME "cake-" CAKE_VERSION "-linux"
#endif
    print_header("Build installer (" PACKAGE_NAME ".tar.gz)");

    execute_cmd("rm -rf " PACKAGE_NAME " && mkdir -p " PACKAGE_NAME "/include");
    execute_cmd("cp -r " CKC_NAME " " CAKE_NAME " cake.json samples web " PACKAGE_NAME "/");
    execute_cmd("cp include/*.h " PACKAGE_NAME "/include/");
    execute_cmd("cp tools/unix_install.sh " PACKAGE_NAME "/install.sh && chmod +x " PACKAGE_NAME "/install.sh");
    execute_cmd("echo " CAKE_VERSION " > " PACKAGE_NAME "/VERSION");
    execute_cmd("tar czf " PACKAGE_NAME ".tar.gz " PACKAGE_NAME);
    execute_cmd("rm -rf " PACKAGE_NAME);
#endif
}

static void run_cake_on_own_source(const char* cake_flags)
{
    print_header("Run cake on its own source");

    /*
       Warnings that are off by default but that cake's own source is kept
       clean of, so dogfooding catches a regression the moment it lands:

         06  unreferenced formal parameter -- remove it, or (when it is
             used only under some #ifdef) mark it used in the other branch;
             a //lint cannot work here because it is per-configuration.
         82  parameter could point to const
         83  parameter set but not used
         84  variable set but not used

       Named explicitly rather than relying on the defaults, so this stays
       enforced whichever way fill_options is configured.
    */
    char* self = calloc(2000, sizeof(char));

#if defined COMPILER_MSVC
    snprintf(self, 2000, EXE(CKC_NAME) " -DTEST -const-literal %s " CAKE_SOURCE_FILES, cake_flags);
#elif defined PLATFORM_WINDOWS && defined COMPILER_CLANG
    snprintf(self, 2000, EXE(CKC_NAME) " -DTEST -w06 -w082 -w083 -w084 %s " CAKE_SOURCE_FILES, cake_flags);
#elif (defined PLATFORM_LINUX || defined PLATFORM_MACOS) && defined COMPILER_CLANG
    snprintf(self, 2000, "./" EXE(CKC_NAME) " -fanalyzer -w06 -w082 -w083 -w084 %s " CAKE_SOURCE_FILES, cake_flags);
#elif defined COMPILER_GCC && !defined COMPILER_TINYC
    snprintf(self, 2000, "./" CKC_NAME " -DTEST -w06 -w082 -w083 -w084 " CAKE_SOURCE_FILES);
#endif

    execute_cmd(self);
    free(self);
}

static void build_cake89(const char* test_flag)
{
    print_header("Build cake89");

#if defined COMPILER_MSVC

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

#endif /* COMPILER_MSVC */

#if (defined PLATFORM_LINUX || defined PLATFORM_MACOS) && defined COMPILER_CLANG

    char* cmd89 = calloc(2000, sizeof(char));
    snprintf(cmd89, 2000, "clang -Wno-multichar %s -o " CKC89_NAME " " CAKE_SOURCE_FILES, test_flag);
    execute_cmd(cmd89);
    free(cmd89);

#endif /* (PLATFORM_LINUX || PLATFORM_MACOS) && COMPILER_CLANG */

#if defined COMPILER_GCC && !defined COMPILER_TINYC

    echo_chdir("./x86_x64_gcc/");
    char* cmd = calloc(2000, sizeof(char));
    snprintf(cmd, 2000, "gcc %s -o " CKC89_NAME " " CAKE_SOURCE_FILES, test_flag);
    execute_cmd(cmd);
    free(cmd);

    execute_cmd("cp " CKC89_NAME " ../" CKC89_NAME);
    echo_chdir("../");

#endif /* COMPILER_GCC && !COMPILER_TINYC */
}

/*
 * run_generated_tests() - behavior tests of the generated C.
 *
 * Each C file under ../tests/run-tests is transpiled with `cake_exe`, the output is
 * compiled with the host compiler (CC) and executed. A test passes when cake
 * reports nothing and the program exits with 0 (tests end with
 * `return failures;`), so these check what the generated code DOES and need
 * no expected-output file per platform.
 */
static void run_generated_tests(const char* title, const char* cake_exe, const char* cake_flags)
{
    print_header(title);

    const char* dir = "../tests/run-tests";
    const char* out_dir = "../tests/run-tests/out";

    mkdir(out_dir, 0777);

    int count = 0;

#ifdef PLATFORM_WINDOWS
    WIN32_FIND_DATAA data;
    HANDLE h = FindFirstFileA("..\\tests\\run-tests\\*.c", &data);
    if (h == INVALID_HANDLE_VALUE)
    {
        printf("no tests found in %s\n", dir);
        exit(1);
    }
    do
    {
        const char* name = data.cFileName;
#else
    DIR* d = opendir(dir);
    if (d == NULL)
    {
        printf("cannot open %s\n", dir);
        exit(1);
    }
    struct dirent* entry;
    while ((entry = readdir(d)) != NULL)
    {
        const char* name = entry->d_name;
        const size_t len = strlen(name);
        if (len < 3 || strcmp(name + len - 2, ".c") != 0)
        {
            continue;
        }
#endif
        char base[512] = { 0 };
        snprintf(base, sizeof base, "%s", name);
        base[strlen(base) - 2] = '\0'; /* drop .c */

        char cmd[4096] = { 0 };

        /* 1. cake: source -> generated C */
        snprintf(cmd, sizeof cmd,
                 RUN "%s %s -fdiagnostics-color=never -wd20 -wd85 -test-mode -o %s/%s.c %s/%s",
                 cake_exe, cake_flags, out_dir, base, dir, name);
        execute_cmd(cmd);

        /* 2. host compiler: generated C -> executable */
#ifdef COMPILER_MSVC
        snprintf(cmd, sizeof cmd, CC " /nologo /w %s/%s.c /Fe:%s/%s.exe", out_dir, base, out_dir, base);
#else
        snprintf(cmd, sizeof cmd, CC " -w %s/%s.c -o %s/%s", out_dir, base, out_dir, base);
#endif
        execute_cmd(cmd);

        /* 3. run it; non-zero exit is the test's failure count */
        snprintf(cmd, sizeof cmd, "%s/%s" EXE(""), out_dir, base);
#ifdef PLATFORM_WINDOWS
        for (char* p = cmd; *p; p++)
        {
            if (*p == '/')
            {
                *p = '\\';
            }
        }
#endif
        printf("%s\n", cmd);
        fflush(stdout);
        const int result = system_like(cmd);
        if (result != 0)
        {
            printf("TEST FAILED: %s/%s exited with %d\n", dir, name, result);
            printf("generated file: %s/%s.c\n", out_dir, base);
            exit(1);
        }
        count++;
#ifdef PLATFORM_WINDOWS
    }
    while (FindNextFileA(h, &data));
    FindClose(h);
#else
    }
    closedir(d);
#endif

    printf("%d generated-code tests passed\n", count);
}

static void run_test_suites(const char* title, const char* cake_exe, const char* cake_flags)
{
    print_header(title);

    const char* suites[] = {
        " -fdiagnostics-color=never ../tests/en-cpp-reference-c/*.c -wd20 -wd44 -wd74 -wd85 -wd88 -test-mode",
        " -fdiagnostics-color=never -wd20 -wd85 ../tests/unit-tests/*.c -test-mode",
        " -fdiagnostics-color=never -wd20 -wd82 -wd85 ../tests/unit-tests/flow3/*.c -test-mode",
    };

    char cmd[1024];
    snprintf(cmd, sizeof cmd, RUN "%s -selftest", cake_exe);
    execute_cmd(cmd);

    for (int i = 0; i < 3; i++)
    {
        snprintf(cmd, sizeof cmd, RUN "%s %s %s", cake_exe, cake_flags, suites[i]);
        execute_cmd(cmd);
    }
}

static void run_tests(const char* cake_flags)
{
    run_test_suites("Run tests", EXE(CKC_NAME), cake_flags);
    run_generated_tests("Run generated-code tests", EXE(CKC_NAME), cake_flags);

    run_test_suites("Run tests (cake89)", EXE(CKC89_NAME), cake_flags);
    run_generated_tests("Run generated-code tests (cake89)", EXE(CKC89_NAME), cake_flags);

    printf("Other test cases:\n");
    printf("  " CKC_NAME " ../tests/unit-tests/failing/*.c -test-mode\n");
}

int main(int argc, char* argv[])
{
    print_header("Cake Build " CAKE_VERSION);

    int fastbuild = 0;
    int full = 0;
    int run_test_suite = 0;
    int debug = 0;
    int cake_headers = 0;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "fast") == 0)
        {
            fastbuild = 1;
        }
        else if (strcmp(argv[i], "full") == 0)
        {
            full = 1;
        }
        else if (strcmp(argv[i], "test") == 0)
        {
            /* test is full plus actually running the test suite */
            full = 1;
            run_test_suite = 1;
        }
        else if (strcmp(argv[i], "debug") == 0)
        {
            debug = 1;
        }
        else if (strcmp(argv[i], "-cake-headers") == 0)
        {
            /* every cake run (self-analysis, cake89, test suites) uses the
               bundled headers instead of the system ones */
            cake_headers = 1;
        }
        else
        {
            printf("unrecognized option: %s\n", argv[i]);
            printf("usage: %s [fast] [full] [test] [debug] [-cake-headers]\n", argv[0]);
            printf("  fast  - incremental build, skips tools/docs/inner-tests/amalgamation\n");
            printf("  full  - build everything with -DTEST, but do not run the test suite\n");
            printf("  test  - same as full, and run the test suite afterwards\n");
            printf("  debug - build without optimizations/-DNDEBUG\n");
            printf("  -cake-headers - run cake with its bundled headers (cake89 and tests)\n");
            return 1;
        }
    }

    /* a full build is the opposite of an incremental one */
    if (full)
    {
        fastbuild = 0;
    }

    const char* test_flag = full ? " -DTEST " : "";
    const char* cake_flags = cake_headers ? " -cake-headers " : "";

    if (!fastbuild)
    {
        build_tools();
        build_docs();
        build_web_samples();
        build_inner_tests();
        build_embedded_files();
        build_amalgamation();
    }

    build_cake(fastbuild, debug, test_flag);
    build_cake_ide(fastbuild, debug);

#ifndef CAKE_HEADERS
    if (!fastbuild)
    {
        generate_config();
        build_installer(); /* needs cake.json */
    }
#endif

    if (full)
    {
        run_cake_on_own_source(cake_flags);
        build_cake89(test_flag);
    }


    if (run_test_suite)
    {
        run_tests(cake_flags);
    }

    print_header("Build succeeded");

    return 0;
}