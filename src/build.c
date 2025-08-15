

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
    " visit_defer.c "     \
    " visit_il.c "        \
    " flow.c "            \
    " error.c "           \
    " type.c "

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
        "    <title>Cake Playground</title>\n"
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


int main()
{
    echo_chdir("./tools");

    execute_cmd(CC " -D_CRT_SECURE_NO_WARNINGS maketest.c " CC_OUTPUT("../maketest.exe") );
    execute_cmd(CC " -D_CRT_SECURE_NO_WARNINGS amalgamator.c " CC_OUTPUT("../amalgamator.exe"));
    execute_cmd(CC " -D_CRT_SECURE_NO_WARNINGS -I.. embed.c  ../fs.c ../error.c " CC_OUTPUT("../embed.exe"));

    echo_chdir("./hoedown");

    execute_cmd(CC HOEDOWN_SOURCE_FILES CC_OUTPUT("../../hoedown.exe"));

    echo_chdir("..");
    echo_chdir("..");

    generate_doc("../manual.md", "./web/manual.html");
    generate_doc("../README.md", "./web/index.html");
    generate_doc("../warnings.md", "./web/warnings.html");
    generate_doc("../ownership.md", "./web/ownership.html");
    generate_doc("../code.md", "./web/code.html");

    remove("hoedown.exe");

    execute_cmd(RUN "maketest.exe unit_test.c " CAKE_SOURCE_FILES);

    remove("maketest.exe");

    execute_cmd(RUN "embed.exe \"./include\" ");

    execute_cmd(RUN "amalgamator.exe -olib.c" CAKE_SOURCE_FILES);
    remove("amalgamator.exe");


#if defined COMPILER_MSVC

    execute_cmd(CC CAKE_SOURCE_FILES " main.c "

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

    //Runs cake on its own source
    execute_cmd("cake.exe -const-literal -sarif -sarif-path \"../vc/.sarif\" -ownership=enable -Wstyle -Wno-unused-parameter -Wno-unused-variable " " main.c " CAKE_SOURCE_FILES);

#ifndef TEST
    //compiling the generated code
    echo_chdir("../out/src");
    execute_cmd("cl main.c " CAKE_SOURCE_FILES);
#endif

#endif

#if defined PLATFORM_WINDOWS && defined COMPILER_CLANG

    execute_cmd("clang " CAKE_SOURCE_FILES " main.c "
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

#if defined COMPILER_HLC
    execute_cmd(CC CAKE_SOURCE_FILES " main.c "

#if defined DEBUG
              " /Od /MDd /RTC1 "
              " /Dstrdup=_strdup" /*nao linka em release*/
#else                              // RELEASE
              " /MT "
              " /DNDEBUG "

#endif
              " /D_CRT_NONSTDC_NO_WARNINGS "
              " /wd4996 "
              " /wd4100 " //unreferenced formal paramet
              " /wd4068 " //unknown pragma                              
              " /W4 "
#ifdef TEST
              "-DTEST"
#endif
              " /D_CRT_SECURE_NO_WARNINGS "
              " /link "
              " ucrt.lib "
              " Kernel32.lib User32.lib Advapi32.lib"
              " uuid.lib Ws2_32.lib Rpcrt4.lib Bcrypt.lib "
              " /out:cake.exe");

    //Runs cake on its own source
    execute_cmd("cake.exe -sarif -sarif-path \"../vc/.sarif\" -ownership=enable -Wstyle -Wno-unused-parameter -Wno-unused-variable " CAKE_SOURCE_FILES);

#endif


#if defined COMPILER_TINYC
    execute_cmd(CC CAKE_SOURCE_FILES " main.c "
             
#ifdef TEST
              "-DTEST"
#endif              
              " -o cake.exe");

    //Runs cake on its own source
    execute_cmd("cake.exe -sarif -sarif-path \"../vc/.sarif\" -ownership=enable -Wstyle -Wno-unused-parameter -Wno-unused-variable " CAKE_SOURCE_FILES);

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
           CAKE_SOURCE_FILES " main.c ");
#endif

#if defined COMPILER_GCC

    // #define GCC_ANALIZER  " -fanalyzer "
    execute_cmd("gcc "
           "  -Wall "
           " -Wno-multichar "
           " -Wno-unknown-pragmas "
           " -g  " CAKE_SOURCE_FILES " main.c "

#ifdef TEST
           " -DTEST"
#endif
           " -o cake");

    //Generates cakeconfig.h with the include dir used by gcc
    execute_cmd("./cake -autoconfig");

    //Uses previouly generated cakeconfig.h to find include dir
    execute_cmd("./cake "
               " -fanalyzer "
               CAKE_SOURCE_FILES);

    //run unit test if -DTEST


#endif

#ifdef TEST
    execute_cmd(RUN "cake ../tests/en-cpp-reference-c/*.c -Wno-unused-variable -Wno-array-size -Wno-array-indirection -Wno-div-by-zero -test-mode");
    execute_cmd(RUN "cake  -Wno-unused-variable ../tests/unit-tests/*.c -test-mode");

    printf("Other test cases\n");
    printf("cake ../tests/unit-tests/failing/*.c -test-mode\n");
#endif // TEST

    //cake ..\tests\sqlite\sqlite3.c -DSQLITE_OMIT_SEH -Wno-out-of-bounds

    return 0;
}
