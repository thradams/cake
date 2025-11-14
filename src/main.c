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

    printf("Cake " CAKE_VERSION " (%s)\n", get_platform(CAKE_COMPILE_TIME_SELECTED_TARGET)->name);

    if (argc < 2)
    {
        print_help();
        return 1;
    }


    if (argc > 1 && strcmp(argv[1], "-selftest") == 0)    
    {
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

        return g_unit_test_error_count > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
#else
        printf("Error: self-tests not included. To run the self-tests, compile with -DTEST and try again.\n");
        return EXIT_FAILURE;
#endif
    }


    struct report report = { 0 };
    int result = compile(argc, (const char**)argv, &report);

    return result;
}


