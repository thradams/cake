
/*
 WINDOWS
 cl -DTEST build.c && build
 LINUX
 gcc  build.c -o build && ./build
 */

#define OUTPUT "cake"

#include "build.h"

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
 " visit.h "

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
" error.c " 


void compile_cprime()
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
        " /out:" OUTPUT ".exe") != 0) exit(1);



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
           " -o " OUTPUT ".exe");
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

    if (system(RUN "hoedown.exe --autolink --fenced-code ../manual.md > ./web/manual.html ") != 0) exit(1);
    if (system(RUN "hoedown.exe --autolink --fenced-code ../README.md > ./web/index.html ") != 0) exit(1);
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
    " ./web_include/string.h " 
    
        if (system(RUN "embed.exe " EMBED_SRC) != 0) exit(1);
    remove("embed.exe");


    compile_cprime();
#ifndef TEST
    /*compila usando ele mesmo*/
    if (system(RUN OUTPUT ".exe " HEADER_FILES SOURCE_FILES) != 0) exit(1);
#endif

}
