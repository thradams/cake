#include <assert.h>
#include <stdio.h>
#include "parser.h"
#include "console.h"
#include "visit.h"
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <crtdbg.h>
#include <Windows.h>
#include <debugapi.h>
#undef assert
#define assert _ASSERTE
#endif


#ifndef TEST
int main(int argc, char** argv)
{
	enable_vt_mode();
	
	printf(WHITE "Cake " CAKE_VERSION RESET "\n");	
	printf("\n\n");
	

	if (argc < 2)
	{
		print_help();
		return 1;
	}
	

	struct report report = { 0 };
	compile(argc, (const char**) argv, &report);


 	return report.error_count > 0;
}
#else
#include "unit_test.c"

int main(int argc, char** argv)
{

	enable_vt_mode();
	
	test_main();
	printf("%d tests failed, %d tests passed\n", g_unit_test_error_count, g_unit_test_success_count);
	

}
#endif

