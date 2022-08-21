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
		"usage: cake [options] source1.c source2.c ...\n"
		"OPTIONS:\n"
		"  --help                   Display this information.\n"
		"  -E                       Preprocess only; do not compile, assemble or link.\n"
		"  -o <file>                Place the output into <file>.\n"
		"  -od <directory>          Output directory.\n"
		"  -B <directory>           Add <directory> to the compiler's search paths.\n"
		"  -r                       Remove comments\n"
		"  -std=<standard>          input language: use -std=c99 -std=c11 -std=c2x\n"
		"  -target-std=<standard>   ouput language: use -std=c99 -std=c11 -std=c2x\n"
	);
}

#ifndef TEST
int main(int argc, char** argv)
{
	EnableVTMode();
	

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

	EnableVTMode();
	
	int r = test_main();
	printf("%d tests failed, %d tests passed\n", g_unit_test_error_count, g_unit_test_success_count);
	

}
#endif

