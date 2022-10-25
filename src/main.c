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
		"Options\n"
		"  -I .................. Adds a directory to the list of directories searched for include files.\n"
		"                        (On windows, if you run cake at the visual studio command prompt cake \n"
		"                        uses the same include files used by msvc.)\n"
		"\n"
		"  -D .................. Defines a preprocessing symbol for a source file.\n"
		"\n"
		"  -E .................. Copies preprocessor output to standard output.\n"
		"\n"
		"  -r .................. Remove all comments from the ouput file.\n"
		"\n"
		"  -rm ................. Outputs preprocessed code after compilation.\n"
		"\n"
		"  -target=standard .... Output target C standard (c89, c99, c11, c2x, cxx).\n"
		"                        C99 is the default and C89 (ANSI C) is the minimum target.\n"
		"\n"
		"  -std=standard ....... Assume that the input sources are for standard (c89, c99, c11, c2x, cxx).\n"
		"                        (not implented yet, input is considered C23)                    \n"
		"\n"
		"  -n .................. Check naming conventions (it is hardcoded for its own naming convention)\n"
		"\n"
		"  -fi ................. Format input (format before language convertion)\n"
		"\n"
		"  -fo ................. Format output (format after language convertion, result parsed again)\n"
		"\n"
		"  -default_nodiscard .. Makes [[nodiscard]] default implicitly.\n"
		"\n"
		"Samples\n"
		"\n"
		"  cake -DDEBUG source.c\n"
		"  cake -target=c99 source.c\n"
		;

	printf("Cake " __DATE__ "\n%s", options);
}

#ifndef TEST
int main(int argc, char** argv)
{
	enable_vt_mode();
	

	if (argc < 2)
	{
		help();
		return 1;
	}
	

	struct report report = { 0 };
	compile(argc, argv, &report);


	return report.error_count > 0;
}
#else
#include "unit_test.c"

int main(int argc, char** argv)
{

	enable_vt_mode();
	
	int r = test_main();
	printf("%d tests failed, %d tests passed\n", g_unit_test_error_count, g_unit_test_success_count);
	

}
#endif

