

/*
 WINDOWS
   cl -DTEST build.c && build

   Debugging on windows:
   cl /Zi build.c
   devenv /DebugExe  build.exe

 LINUX/MACOS
   gcc  build.c -o build && ./build
 */

#include "build.h"


#define CAKE_SOURCE_FILES \
    " token.c "           \
    " hashmap.c "         \
    " console.c "         \
    " tokenizer.c "       \
    " osstream.c "        \
    " fs.c "              \
    " options.c "         \
    " object.c "          \
    " expressions.c "     \
    " pre_expressions.c " \
    " parser.c "          \
    " compile.c "         \
    " visit_defer.c "     \
    " visit_il.c "        \
    " flow.c "            \
    " error.c "           \
    " target.c "          \
    " type.c "            \
    " main.c "

#define HOEDOWN_SOURCE_FILES \
 " autolink.c " \
 " buffer.c "   \
 " document.c " \
 " escape.c "   \
 " hoedown.c "  \
 " html.c "     \
 " html_blocks.c "\
 " html_smartypants.c "\
 " stack.c "\
 " version.c"

static void generate_doc(const char* mdfilename, const char* outfile)
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
        "    <title>Cake C Compiler</title>\n"
        "    <meta name=\"description\" content=\"Cake C Compiler\">\n"
        "    <link rel=\"icon\" type=\"image/x-icon\" href=\"favicon.ico\">\n"
        "    <script>\n"
        "    function Try(elm)\n"
        "    {\n"
        "        //collect the text previous sample\n"
        "        var source = elm.parentElement.previousElementSibling.innerText;\n"
        "\n"
        "        var link = \"./playground.html?code=\" + encodeURIComponent(btoa(source)) +\n"
        "            \"&to=\" + encodeURI(\"-2\") +\n"
        "            \"&options=\" + encodeURI(\"\");\n"
        "\n"
        "        window.open(link, '_blank');\n"
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
    else
    {
        printf("could not open index.html for writing\n");
        exit(1);
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
    else
    {
        printf("could not open index.html for writing\n");
        exit(1);
    }
}

static void HEADER(const char* text)
{
    printf("************************************************\n");
    printf(" %s\n", text);
    printf("************************************************\n");

}

int main()
{

    HEADER("Build tools");

    echo_chdir("./tools");

    execute_cmd(CC " -D_CRT_SECURE_NO_WARNINGS maketest.c " CC_OUTPUT("../maketest.exe"));
    execute_cmd(CC " -D_CRT_SECURE_NO_WARNINGS amalgamator.c " CC_OUTPUT("../amalgamator.exe"));
    execute_cmd(CC " -D_CRT_SECURE_NO_WARNINGS -I.. embed.c  ../fs.c ../error.c " CC_OUTPUT("../embed.exe"));

    echo_chdir("./hoedown");

    execute_cmd(CC HOEDOWN_SOURCE_FILES CC_OUTPUT("../../hoedown.exe"));


    HEADER("Build docs");

    echo_chdir("..");
    echo_chdir("..");

    generate_doc("../manual.md", "./web/manual.html");
    generate_doc("../README.md", "./web/index.html");
    generate_doc("../warnings.md", "./web/warnings.html");
    generate_doc("../ownership.md", "./web/ownership.html");
    generate_doc("../code.md", "./web/code.html");

    remove("hoedown.exe");


    HEADER("Build inner tests");

    execute_cmd(RUN "maketest.exe unit_test.c " CAKE_SOURCE_FILES);

    remove("maketest.exe");

    HEADER("Build embedded files");



    execute_cmd(RUN "embed.exe \"./include\" ");


    HEADER("Build amalgamated file");


    execute_cmd(RUN "amalgamator.exe -olib.c" CAKE_SOURCE_FILES);
    remove("amalgamator.exe");



    HEADER("Build cake");


#if defined COMPILER_MSVC

    execute_cmd(CC CAKE_SOURCE_FILES

#if defined DEBUG
               " /Od /MDd /RTC1 "
               " /Dstrdup=_strdup" /*nao linka em release*/
#else                              // RELEASE
               " /GL /Gy /O2 /MT /Ot"
               " /DNDEBUG "
               " /Dstrdup=_strdup"
#endif
               " /D_CRT_NONSTDC_NO_WARNINGS "
               " /wd4996 "
               " /wd4100 " //unreferenced formal paramet
               " /wd4068 " //unknown pragma
               " /permissive- "
               " /GS "
               " /Zc:preprocessor- "
               " /std:c17 "
               " /utf-8 "
               " /W4 "
               " /Zi "
               " /Gm- "
               " /std:clatest "
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
               " /out:cake.exe ");

#ifdef CAKE_HEADERS
    //uses cakeconfig
#else
    //Generates cakeconfig.h with the include dir used by gcc
    execute_cmd("cake.exe -autoconfig");
#endif

    //Runs cake on its own source
    
    HEADER("Runs cake on its own source");
    

    execute_cmd("cake.exe -DTEST -const-literal -sarif -sarif-path \"../vc/.sarif\" -ownership=enable -w11 " CAKE_SOURCE_FILES);

#ifdef _WIN64
    echo_chdir("../x64_msvc/src");
#else
    echo_chdir("../x86_msvc/src");
#endif

    execute_cmd("cl  -o cake89.exe" CAKE_SOURCE_FILES);
    copy_file("cake89.exe", "../../src/cake89.exe");
    echo_chdir("../../src");

#endif

#if defined PLATFORM_WINDOWS && defined COMPILER_CLANG

    execute_cmd("clang " CAKE_SOURCE_FILES
#if defined DEBUG
           " -D_DEBUG"
#else
           " -D_NDEBUG"
#endif

#ifdef TEST
           " -DTEST"
#endif
           " -Dstrdup=_strdup "
           " -Wno-switch"
           " -DWIN32"
           " -D_CRT_SECURE_NO_WARNINGS "
           " -std=c17 -Wno-multichar "
           " -D_MT "
           " -Xlinker /NODEFAULTLIB "
           " -lucrt.lib -lvcruntime.lib -lmsvcrt.lib "
           " -lKernel32.lib -lUser32.lib -lAdvapi32.lib"
           " -luuid.lib -lWs2_32.lib -lRpcrt4.lib -lBcrypt.lib "
           " -o cake.exe");

    //Runs cake on its own source
    execute_cmd("cake.exe -ownership=enable -Wstyle -fanalyzer -Wno-unused-parameter -Wno-unused-variable " CAKE_SOURCE_FILES);

#endif


#if (defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)) && defined COMPILER_CLANG
    execute_cmd("clang "

#ifdef TEST
           "-DTEST"
#endif
           " -Wall "
           " -D_DEFAULT_SOURCE "
           " -Wno-unknown-pragmas "
           " -Wno-multichar "
           " -std=c17 "

           " -o cake "
           CAKE_SOURCE_FILES);


#ifdef CAKE_HEADERS
    //uses cakeconfig
#else
    //Generates cakeconfig.h with the include dir used by gcc
    execute_cmd("./cake  -autoconfig");
#endif

    //Uses previouly generated cakeconfig.h to find include dir
    execute_cmd("./cake "
               " -fanalyzer "
               CAKE_SOURCE_FILES);

    //run unit test if -DTEST
#endif

#if defined COMPILER_GCC && !defined(COMPILER_TINYC)

    // #define GCC_ANALIZER  " -fanalyzer "
    execute_cmd("gcc "
           "  -Wall "
           " -Wno-multichar "
           " -Wno-unknown-pragmas "
           " -g  " CAKE_SOURCE_FILES

#ifdef TEST
           " -DTEST "
#endif
           " -o cake");


#ifdef CAKE_HEADERS
    //uses cakeconfig
#else
    //Generates cakeconfig.h with the include dir used by gcc
    execute_cmd("./cake  -autoconfig");
#endif

    
    HEADER("Runs cake on its own source");
    

    //Uses previouly generated cakeconfig.h to find include dir
    execute_cmd("./cake -DTEST -fanalyzer " CAKE_SOURCE_FILES);


    echo_chdir("./x86_x64_gcc/");

    execute_cmd("gcc  -o cake89 " CAKE_SOURCE_FILES);
    //execute_cmd("cp "cake"")
    execute_cmd("cp cake89 ../cake89");
    echo_chdir("../");

#endif

#ifdef TEST
    
    HEADER("Runs tests");
    

    execute_cmd(RUN "cake -selftest");
    execute_cmd(RUN "cake -fdiagnostics-color=never ../tests/en-cpp-reference-c/*.c -wd20 -test-mode");
    execute_cmd(RUN "cake  -fdiagnostics-color=never -wd20 ../tests/unit-tests/*.c -test-mode");

    execute_cmd(RUN "cake  -fdiagnostics-color=never -wd20 ../tests/output-test/*.c -test-mode-in-out");
    execute_cmd(RUN "cake  -fdiagnostics-color=never -E ../tests/preprocessor/*.c -test-mode-in-out");


    
    HEADER("Runs tests 89");
    

    execute_cmd(RUN "cake89 -selftest");
    execute_cmd(RUN "cake89 -fdiagnostics-color=never ../tests/en-cpp-reference-c/*.c -wd20 -test-mode");
    execute_cmd(RUN "cake89  -fdiagnostics-color=never -wd20 ../tests/unit-tests/*.c -test-mode");

    execute_cmd(RUN "cake89  -fdiagnostics-color=never -wd20 ../tests/output-test/*.c -test-mode-in-out");
    execute_cmd(RUN "cake89  -fdiagnostics-color=never -E ../tests/preprocessor/*.c -test-mode-in-out");

    printf("Other test cases\n");
    printf("cake ../tests/unit-tests/failing/*.c -test-mode\n");


#endif // TEST

    //cake ..\tests\sqlite\sqlite3.c -DSQLITE_OMIT_SEH -Wno-out-of-bounds

    return 0;
}
