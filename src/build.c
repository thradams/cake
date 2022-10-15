
/*
 WINDOWS
 cl -DTEST build.c && build
 LINUX
 gcc  build.c -o build && ./build
 */

#include "build.h"

#ifdef BUILD_WINDOWS
#define OUTPUT "cake.exe"
#else
#define OUTPUT "cake"
#endif


#define HEADER_FILES \
 " console.h "\
 " tokenizer.h " \
 " parser.h " \
 " error.h "\
 " fs.h "\
 " grammar.h "\
 " hash.h "\
 " hashmap.h "\
 " osstream.h "\
 " options.h " \
 " token.h "\
 " type.h " \
 " pre_expressions.h " \
 " expressions.h " \
 " visit.h " \
 " formatvisit.h "

#define SOURCE_FILES \
" token.c " \
" hash.c " \
" hashmap.c " \
" console.c "\
" tokenizer.c " \
" osstream.c " \
" fs.c " \
" options.c " \
" expressions.c " \
" pre_expressions.c " \
" type.c " \
" parser.c " \
" visit.c " \
" error.c " \
" formatvisit.c "


void compile_cake()
{

#ifdef BUILD_WINDOWS_MSC
    if (system("cl "
        SOURCE_FILES  " main.c "
#if defined DEBUG
        " /Od /MDd /RTC1 "
        " /Dstrdup=_strdup" /*nao linka em release*/
#else //RELEASE
        " /GL /Gy /O2 /MD /Ot"
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
        " /out:" OUTPUT ) != 0) exit(1);



#endif

#ifdef BUILD_WINDOWS_CLANG

    system("clang "
           SOURCE_FILES " main.c "
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
           " -o " OUTPUT );
#endif

#ifdef BUILD_LINUX_CLANG
    system("clang "
           SOURCE_FILES " main.c "
#ifdef TEST
           "-DTEST"
#endif
           " -std=c17 "
           " -Wall "
           " -o " OUTPUT);
#endif

#ifdef BUILD_LINUX_GCC
    //#define GCC_ANALIZER  " -fanalyzer "
    system(GCC
           " -Wall "
           " -Wno-multichar "
           " -Wno-switch "
           " -g "
           SOURCE_FILES " main.c "

#if defined DEBUG
           " -D_DEBUG"
#else
           " -D_NDEBUG"
#endif

#ifdef TEST
           " -DTEST"
#endif
           " -DNDEBUG"
           " -o " OUTPUT
    );
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
        "     <link rel=\"stylesheet\" href=\"style.css\" />\n"
        "</head>\n"
        "<body>\n"
        "<p><a href=\"index.html\">Home</a> | <a href=\"manual.html\">Manual</a> | <a href=\"playground.html\">Playground</a></p>\n"
        "<h1>Cake</h1>\n";

    FILE* f2 = fopen(outfile /*"./web/index.html"*/, "w");
    if (f2)
    {
        fwrite(header, 1, strlen(header), f2);
        fclose(f2);
    }
    char cmd[200];
    snprintf(cmd, sizeof cmd, RUN "hoedown.exe --html-toc --toc-level 2 --autolink --fenced-code %s >> %s", mdfilename, outfile);
    if (system(cmd) != 0) exit(1);

    snprintf(cmd, sizeof cmd, RUN "hoedown.exe  --toc-level 2 --autolink --fenced-code %s >> %s", mdfilename, outfile);
    if (system(cmd) != 0) exit(1);

    FILE* f3 = fopen(outfile /*"./web/index.html"*/, "a");
    if (f3)
    {
        fwrite("</body></html>", 1, strlen("</body></html>"), f3);
        fclose(f3);
    }
}

int build_main()
{

    printf("Building tools-------------------------------------------\n");
    chdir("./tools");
    
    if (system(COMPILER_NAME " -D_CRT_SECURE_NO_WARNINGS maketest.c -o ../maketest.exe") != 0) exit(1);
    if (system(COMPILER_NAME " -D_CRT_SECURE_NO_WARNINGS amalgamator.c -o ../amalgamator.exe") != 0) exit(1);
    if (system(COMPILER_NAME " -D_CRT_SECURE_NO_WARNINGS embed.c -o ../embed.exe") != 0) exit(1);

    chdir("./hoedown");

#define HOEDOWN_SRC " autolink.c buffer.c document.c escape.c hoedown.c html.c html_blocks.c html_smartypants.c stack.c version.c"

    if (system(COMPILER_NAME HOEDOWN_SRC  " -o ../../hoedown.exe") != 0) exit(1);

    chdir("..");
    chdir("..");
    
   /*
    * Generates documentation from md files
    */

    generate_doc("../manual.md", "./web/manual.html");
    generate_doc("../README.md", "./web/index.html");

    remove("hoedown.exe");

    if (system(RUN "maketest.exe unit_test.c " SOURCE_FILES) != 0) exit(1);
    remove("maketest.exe");

    if (system(RUN "amalgamator.exe -olib.c" SOURCE_FILES) != 0) exit(1);
    remove("amalgamator.exe");

#define EMBED_SRC \
    " ./web_include/stdio.h " \
    " ./web_include/stdlib.h " \
    " ./web_include/math.h " \
    " ./web_include/errno.h " \
    " ./web_include/string.h " \
    " ./web_include/assert.h " \
    " ./web_include/annotations.h "
    
    if (system(RUN "embed.exe " EMBED_SRC) != 0) exit(1);
    remove("embed.exe");


    compile_cake();

#ifndef TEST
#ifdef BUILD_WINDOWS
    /*compila usando ele mesmo*/
    if (system(RUN OUTPUT  " -n " HEADER_FILES SOURCE_FILES) != 0) exit(1);
#endif
#endif

#ifdef TEST
    int test_result = system(RUN OUTPUT);
    if (test_result)
    {
        printf("\n");
        printf(RED "TESTS FAILED "  OUTPUT " exit code %d\n", test_result);
        printf(RESET);
    }
#endif


}
