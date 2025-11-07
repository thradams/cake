/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma safety enable

#include <assert.h>
#include <stdio.h>
#include <string.h>
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



static void print_report(struct report* report)
{
    if (report->test_mode ||
        report->error_count != 0 ||
        report->warnings_count != 0 ||
        report->info_count != 0)
    {

        printf("\n");
        printf("%d"   " errors ", report->error_count);
        printf("%d"  " warnings ", report->warnings_count);
        printf("%d"     " notes ", report->info_count);
        printf("\n");
        printf("%d files in %.2f seconds", report->no_files, report->cpu_time_used_sec);

        if (report->test_mode)
        {
            if (report->test_failed > 0)
                printf(RED " - TEST FAILED" COLOR_RESET);
            else
                printf(GREEN " - TEST SUCCEEDED" COLOR_RESET);

        }
        printf("\n");

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


    if (argc > 1 && strcmp(argv[1], "-selftest") == 0)    
    {
        /*
          cake -selftest
          cake must be compiled with -DTEST
        */

        printf("*** SELF TEST ***.\n");

#ifdef TEST


        clock_t begin_clock = clock();
        
        test_main();
        
        struct report report = { 0 }; 
        report.test_mode = true;
        report.test_failed = g_unit_test_error_count;
        report.test_succeeded = g_unit_test_success_count;

        clock_t end_clock = clock();
        double cpu_time_used = ((double)(end_clock - begin_clock)) / CLOCKS_PER_SEC;
        report.no_files = g_unit_test_error_count + g_unit_test_success_count;
        report.cpu_time_used_sec = cpu_time_used;

        print_report(&report);

        if (g_unit_test_error_count > 0)
        {
            return EXIT_FAILURE;
        }
#else
        printf("cake must be compiled with -DTEST to include self tests.\n");
        return EXIT_FAILURE;
#endif
    }
    


    struct report report = { 0 };
    compile(argc, (const char**)argv, &report);

    if (!report.ignore_this_report)
    {
        print_report(&report);
    }

    if (report.test_mode)
    {
        return report.test_failed ? EXIT_FAILURE : EXIT_SUCCESS;
    }


    return report.error_count > 0 ? EXIT_FAILURE : EXIT_SUCCESS;

}


