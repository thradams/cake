/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake 
*/

#pragma safety enable

#include <assert.h>
#include <stdio.h>
#include "parser.h"
#include "console.h"
#include <stdlib.h>
#include <time.h>
#include "version.h"

#ifdef _WIN32
#include <crtdbg.h>
#include <Windows.h>
#undef assert
#define assert _ASSERTE
#endif



static void print_report(struct report* report, bool no_colors)
{
    printf("\n");
    printf(" %d files in %.2f seconds\n", report->no_files, report->cpu_time_used_sec);
    if (no_colors)
    {
        printf(" %d"   " errors ", report->error_count);
        printf("%d"  " warnings ", report->warnings_count);
        printf("%d"     " notes ", report->info_count);
    }
    else
    {
        printf(" %d" LIGHTRED  " errors " RESET, report->error_count);
        printf("%d" LIGHTMAGENTA " warnings " RESET, report->warnings_count);
        printf("%d" LIGHTCYAN    " notes " RESET, report->info_count);
    }

    if (report->test_mode)
    {
        printf("\n");
        printf(" %d tests of %d failed\n", report->test_failed, (report->test_failed + report->test_succeeded));
    }

    printf("\n");
}

#ifdef TEST
#include "unit_test.c"
#endif

int main(int argc, char** argv)
{
    enable_vt_mode();

    

#if 0
    //Help debug emscript on desktop 
    //MOCKFILES needs to be defined
    const char* source = 
        "#include <stdio.h>\n";

    char* _Owner _Opt r =
        CompileText("", source);
    return;
#endif

    //-msvc-output 

    /*
      /nologo ?
    */
    printf("Cake " CAKE_VERSION " ");
#if defined _MSC_VER 
    printf("(MSVC ");
#ifdef _WIN64
    printf("x64");
#else
    printf("x86");
#endif
    printf(")");

#elif defined __linux__ && defined __GNUC__ 
    printf("(GCC)");
#endif

    printf("\n");

    if (argc < 2)
    {
        print_help();
        return 1;
    }

#ifdef TEST
    test_main();
    printf("--------------------------------------\n");
    printf("Tests embedded within #ifdef TEST blocks.\n");
    printf("%d tests failed, %d tests passed\n", g_unit_test_error_count, g_unit_test_success_count);
    printf("--------------------------------------\n");
    if (g_unit_test_error_count > 0)
    {
        return EXIT_FAILURE;
    }
#endif

    struct report report = { 0 };
    compile(argc, (const char**)argv, &report);

    if (!report.ignore_this_report)
    {
        print_report(&report, true);
    }

    if (report.test_mode)
    {
        return report.test_failed ? EXIT_FAILURE : EXIT_SUCCESS;
    }


    return report.error_count > 0 ? EXIT_FAILURE : EXIT_SUCCESS;

}


