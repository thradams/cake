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
	printf("Cake " __DATE__ "\n"
		"\n"
		"cake [options] source1.c source2.c ...\n"
		"\n"
		"Options\n"
		"  -I               Adds a directory to the list of directories searched for include files.\n"
		"  -D               Defines a preprocessing symbol for a source file.\n"
		"  -E               Copies preprocessor output to standard output.\n"
		"  -r               Remove all comments from the ouput file.\n"
		"  -rm              Outputs preprocessed code after compilation.\n"
		"  -target=standard Output target C standard (c99, c11, c2x, cxx).\n"
		"  -std=standard    Assume that the input sources are for standard (c99, c11, c2x, cxx).\n"
	);
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
	

	struct error error = { 0 };
	compile(argc, argv, &error);



	if (error.code)
	{
		printf("\n%s\n", error.message);
#ifdef _WIN32
		OutputDebugStringA(error.message);
#endif
	}

	return error.code;
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

