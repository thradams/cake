#include <assert.h>
#include <stdio.h>
#include "parser.h"
#include "console.h"
#include "visit.h"
#include <stdlib.h>
#include <time.h>
#include "version.h"

#ifdef _WIN32
#include <crtdbg.h>
#include <Windows.h>
#include <debugapi.h>
#undef assert
#define assert _ASSERTE
#endif


#ifndef TEST

static void print_report(struct report* report, bool msvc)
{
    printf("\n");
    printf(" %d files in %.2f seconds\n", report->no_files, report->cpu_time_used_sec);
    if (msvc)
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


    printf("\n");
}

int main(int argc, char** argv)
{
    enable_vt_mode();

    //-msvc-output 

    printf("Cake " CAKE_VERSION "\n");
    printf("\n\n");


    if (argc < 2)
    {
        print_help();
        return 1;
    }

    struct report report = {0};
    compile(argc, (const char**) argv, &report);
    print_report(&report, true);

    return report.error_count > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
    
}
#else
#include "unit_test.c"
#define CURRENT_NUMBER_OF_FAILING_TEST 2
int main(int argc, char** argv)
{

    enable_vt_mode();

    test_main();
    printf("%d tests failed, %d tests passed\n", g_unit_test_error_count, g_unit_test_success_count);
    printf("%d expected to fail - waiting fixes\n", CURRENT_NUMBER_OF_FAILING_TEST);

    return g_unit_test_error_count > CURRENT_NUMBER_OF_FAILING_TEST ? EXIT_FAILURE : EXIT_SUCCESS;
}
#endif

