
/*
 WINDOWS
 cl -DTEST build.c && build
 LINUX/MACOS
 gcc  build.c -o build && ./build
 */

#include "build.h"

static void execute_cmd(const char* cmd)
{
    printf("%s\n", cmd);
    fflush(stdout);
    if (system_like(cmd) != 0)
    {
        exit(1);
    }
}

static int echo_sytem(const char* cmd)
{
    printf("%s\n", cmd);
    fflush(stdout);
    return system_like(cmd);
}


static int mychdir(const char* path)
{
    printf("chdir: %s\n", path);
    fflush(stdout);
    return chdir(path);
}

#ifdef BUILD_WINDOWS
#define OUTPUT "cake.exe"
#else
#define OUTPUT "cake"
#endif

#define HEADER_FILES      \
    " console.h "         \
    " tokenizer.h "       \
    " parser.h "          \
    " error.h "           \
    " fs.h "              \
    " hash.h "            \
    " flow_object.h "     \
    " hashmap.h "         \
    " osstream.h "        \
    " options.h "         \
    " token.h "           \
    " type.h "            \
    " pre_expressions.h " \
    " expressions.h "     \
    " visit.h "           \
    " format_visit.h "


#define SOURCE_FILES      \
    " token.c "           \
    " hash.c "            \
    " hashmap.c "         \
    " console.c "         \
    " tokenizer.c "       \
    " osstream.c "        \
    " fs.c "              \
    " options.c "         \
    " expressions.c "     \
    " pre_expressions.c " \
    " type.c "            \
    " flow_object.c "     \
    " parser.c "          \
    " visit.c "           \
    " flow_visit.c " \
    " error.c "           \
    " format_visit.c "


void compile_cake()
{
    int result = 0;

#ifdef BUILD_WINDOWS_MSC
    execute_cmd("cl  " SOURCE_FILES " main.c "

#ifdef DISABLE_COLORS
               " /DDISABLE_COLORS "
#endif

#if defined DEBUG
               " /Od /MDd /RTC1 "
               " /Dstrdup=_strdup" /*nao linka em release*/
#else                              // RELEASE
               " /GL /Gy /O2 /MT /Ot"
               " /DNDEBUG "
               " /Dstrdup=_strdup" /*nao linka em release*/
#endif
               " /D_CRT_NONSTDC_NO_WARNINGS "
               " /wd4996 "
               " /permissive- "
               " /GS "
               " /Zc:preprocessor- "
               " /std:c17 "
               " /utf-8 "
               " /W4 "
               " /Zi "
               " /Gm- "
               " /sdl "
               " /Zc:inline "
               //" /WX " //Treats all compiler warnings as errors.
               " /Gd "
               " /Oy- "
               " /FC "
               " /EHsc "
#ifdef TEST
               "-DTEST"
#endif
               " /D_CRT_SECURE_NO_WARNINGS "

               " /link "
               " /NODEFAULTLIB "
               " ucrt.lib vcruntime.lib msvcrt.lib "
               " Kernel32.lib User32.lib Advapi32.lib"
               " uuid.lib Ws2_32.lib Rpcrt4.lib Bcrypt.lib "
               " /out:" OUTPUT);

#endif

#ifdef BUILD_WINDOWS_CLANG

    execute_cmd("clang " SOURCE_FILES " main.c "
#if defined DEBUG
           " -D_DEBUG"
#else
           " -D_NDEBUG"
#endif

#ifdef TEST
           " -DTEST"
#endif
           " -Wno-switch"
           " -DWIN32"
           " -D_CRT_SECURE_NO_WARNINGS "
           " -std=c17 -Wno-multichar "
           " -D_MT "
           " -Xlinker /NODEFAULTLIB "
           " -lucrt.lib -lvcruntime.lib -lmsvcrt.lib "
           " -lKernel32.lib -lUser32.lib -lAdvapi32.lib"
           " -luuid.lib -lWs2_32.lib -lRpcrt4.lib -lBcrypt.lib "
           " -o " OUTPUT);
#endif

#if defined BUILD_LINUX_CLANG || defined BUILD_MACOS_CLANG
    execute_cmd("clang " SOURCE_FILES " main.c "
#ifdef TEST
           "-DTEST"
#endif
           " -std=c17 "
           " -Wall "
           " -o " OUTPUT);
#endif

#if defined BUILD_LINUX_GCC || defined BUILD_WINDOWS_GCC  || defined BUILD_MACOS_GCC

    // #define GCC_ANALIZER  " -fanalyzer "
    execute_cmd("gcc "
           "  -Wall "
           " -Wno-multichar "
           " -g  " SOURCE_FILES " main.c "

#ifdef TEST
           " -DTEST"
#endif
           " -o " OUTPUT);
#endif


}

void generate_doc(const char* mdfilename, const char* outfile)
{
    const char* header =
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "  \n"
        "    <link rel=\"stylesheet\" href=\"default.min.css\">\n"
        "    <script src=\"highlight.min.js\"></script>\n"
        "    <script>hljs.highlightAll();</script>\n"
        "    <link rel=\"stylesheet\" href=\"style.css\" />\n"
        "    <title>Cake Playground</title>\n"
        "    <link rel=\"icon\" type=\"image/x-icon\" href=\"favicon.ico\">\n"
        "    <script>\n"
        "    function Try(elm)\n"
        "    {\n"
        "        //collect the text previous sample\n"
        "        var source = elm.parentElement.previousElementSibling.innerText;\n"
        "\n"
        "        var link = \"./playground.html?code=\" + encodeURIComponent(btoa(source)) +\n"
        "            \"&to=\" + encodeURI(\"1\") +\n"
        "            \"&options=\" + encodeURI(\"\");\n"
        "\n"
        "        window.open(link, 'popup','width=800,height=600');\n"
        "    }\n"
        "// find-replace for this\n"
        "// <button onclick=\"Try(this)\">try</button> \n"
        "</script>"
        "</head>\n"
        "<body>\n"
        "    <article style=\"max-width: 40em; margin:auto\">\n"
        "<p><a href=\"index.html\">Home</a> | <a href=\"manual.html\">Manual</a> | <a href=\"playground.html\">Playground</a></p>\n"
        "<article>\n"
        "<h1>Cake - C23 and Beyond</h1>\n";

    FILE* f2 = fopen(outfile /*"./web/index.html"*/, "w");
    if (f2)
    {
        fwrite(header, 1, strlen(header), f2);
        fclose(f2);
    }
    char cmd[200];
    snprintf(cmd, sizeof cmd, RUN "hoedown.exe --html-toc --toc-level 3 --autolink --fenced-code %s >> %s", mdfilename, outfile);
    execute_cmd(cmd);

    snprintf(cmd, sizeof cmd, RUN "hoedown.exe  --toc-level 3 --autolink --fenced-code %s >> %s", mdfilename, outfile);
    execute_cmd(cmd);

    FILE* f3 = fopen(outfile /*"./web/index.html"*/, "a");
    if (f3)
    {
        fwrite("</article></body></html>", 1, strlen("</article></body></html>"), f3);
        fclose(f3);
    }
}

int main()
{
    printf(CC_DESCRIPTION "\n");
#if RELEASE
    printf("RELEASE\n");
#elif defined DEBUG
    printf("DEBUG\n");
#endif

    printf("Building tools-------------------------------------------\n");
    mychdir("./tools");

    execute_cmd(CC " -D_CRT_SECURE_NO_WARNINGS maketest.c " OUT_OPT "../maketest.exe");
    execute_cmd(CC " -D_CRT_SECURE_NO_WARNINGS amalgamator.c " OUT_OPT "../amalgamator.exe");

    mychdir("./hoedown");

#define HOEDOWN_SRC " autolink.c buffer.c document.c escape.c hoedown.c html.c html_blocks.c html_smartypants.c stack.c version.c"

    execute_cmd(CC HOEDOWN_SRC OUT_OPT "../../hoedown.exe");

    mychdir("..");
    mychdir("..");

    /*
     * Generates documentation from md files
     */

    generate_doc("../manual.md", "./web/manual.html");
    generate_doc("../README.md", "./web/index.html");
    generate_doc("../warnings.md", "./web/warnings.html");
    generate_doc("../ownership.md", "./web/ownership.html");

    remove("hoedown.exe");

    execute_cmd(RUN "maketest.exe unit_test.c " SOURCE_FILES);

    remove("maketest.exe");

    execute_cmd(RUN "amalgamator.exe -olib.c" SOURCE_FILES);
    remove("amalgamator.exe");

    compile_cake();


    /*run cake on it�s own source*/
#ifdef BUILD_WINDOWS_MSC

    /*
      running cake on its own source code
    */

    execute_cmd("cake.exe  -ownership=enable -Wstyle -fanalyzer -Wno-unused-parameter -Wno-unused-variable -sarif " HEADER_FILES SOURCE_FILES);
    printf("\n");
    printf("To run unit test use:\n");
    printf("cake ../tests/unit-tests/*.c -test-mode\n");

#ifdef TEST
    execute_cmd("cake.exe ../tests/unit-tests/*.c -test-mode\n");
#endif // TEST



#endif

#if defined BUILD_LINUX_GCC

    /*
       running cake on its own source code
    */

    //Generates cakeconfig.h with the include dir used by gcc
    execute_cmd("./cake -autoconfig");

    //Uses previouly generated cakeconfig.h to find include dir
    execute_cmd("./cake "
               " -D__x86_64__ "
               " -fanalyzer "
               HEADER_FILES
               SOURCE_FILES);

#ifdef TEST
    execute_cmd("./cake ../tests/unit-tests/*.c -test-mode\n");
#endif // TEST

#endif

    return 0;
}
