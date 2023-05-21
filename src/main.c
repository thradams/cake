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

void help()
{
	const char* options =
		"cake [options] source1.c source2.c ...\n"
		"\n"
		WHITE "SAMPLES\n" RESET
		      "\n"
		WHITE "    cake source.c\n" RESET
		      "    Compiles source.c and ouputs /out/source.c\n"
		      "\n"
		WHITE "    cake -target=C11 source.c\n" RESET
		      "    Compiles source.c and ouputs C11 code at /out/source.c\n"
		      "\n"
		WHITE "OPTIONS\n" RESET
		      "\n"
		WHITE "  -I                   " RESET " Adds a directory to the list of directories searched for include files \n"
		      "                        (On windows, if you run cake at the visual studio command prompt cake \n"
		      "                        uses the same include files used by msvc )\n"
		      "\n"
		WHITE "  -no-output            " RESET "Cake will not generate ouput\n"
		      "\n"
		WHITE "  -D                    " RESET "Defines a preprocessing symbol for a source file \n"
		      "\n"
		WHITE "  -E                    " RESET "Copies preprocessor output to standard output \n"
		      "\n"
		WHITE "  -r                    " RESET "Remove all comments from the ouput file \n"
		      "\n"
		WHITE "  -rm                   " RESET "Outputs preprocessed code after compilation \n"
		      "\n"
		WHITE "  -target=standard      " RESET "Output target C standard (c89, c99, c11, c2x, cxx) \n"
		      "                        C99 is the default and C89 (ANSI C) is the minimum target \n"
		      "\n"
		WHITE "  -std=standard         " RESET "Assume that the input sources are for standard (c89, c99, c11, c2x, cxx) \n"
		      "                        (not implented yet, input is considered C23)                    \n"
		      "\n"
		WHITE "  -n                    " RESET "Check naming conventions (it is hardcoded for its own naming convention)\n"
		      "\n"
		WHITE "  -fi                   " RESET "Format input (format before language convertion)\n"
		      "\n"
		WHITE "  -fo                   " RESET "Format output (format after language convertion, result parsed again)\n"
		      "\n"
		WHITE "  -default_nodiscard    " RESET "Makes [[nodiscard]] default implicitly \n"
		      "\n"
		      "\n"
	          "More details at http://thradams.com/cake/manual.html\n"
		;

	printf("%s", options);
}

#ifndef TEST
int main(int argc, char** argv)
{
	enable_vt_mode();
	
	printf(WHITE "Cake " CAKE_VERSION RESET "\n");	
	printf("\n\n");
	

	if (argc < 2)
	{
		help();
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

