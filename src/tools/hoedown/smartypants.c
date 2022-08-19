#include "html.h"

#include "common.h"
/*#include <time.h>*/


/* FEATURES INFO / DEFAULTS */

#define DEF_IUNIT 1024
#define DEF_OUNIT 64


/* PRINT HELP */

void
print_help(const char *basename)
{
	/* usage */
	printf("Usage: %s [OPTION]... [FILE]\n\n", basename);

	/* description */
	printf("Apply SmartyPants smart punctuation to the HTML in FILE (or standard input), and output the resulting HTML to standard output.\n\n");

	/* main options */
	printf("Main options:\n");
	print_option('T', "time", "Show time spent in SmartyPants processing.");
	print_option('i', "input-unit=N", "Reading block size. Default is " str(DEF_IUNIT) ".");
	print_option('o', "output-unit=N", "Writing block size. Default is " str(DEF_OUNIT) ".");
	print_option('h', "help", "Print this help text.");
	print_option('v', "version", "Print Hoedown version.");
	printf("\n");

	/* ending */
	printf("Options are processed in order, so in case of contradictory options the last specified stands.\n\n");

	printf("When FILE is '-', read standard input. If no FILE was given, read standard input. Use '--' to signal end of option parsing. "
	       "Exit status is 0 if no errors occurred, 1 with option parsing errors, 4 with memory allocation errors or 5 with I/O errors.\n\n");
}


/* OPTION PARSING */

struct option_data {
	char *basename;
	int done;

	/* time reporting */
	int show_time;

	/* I/O */
	size_t iunit;
	size_t ounit;
	const char *filename;
};

int
parse_short_option(char opt, char *next, void *opaque)
{
	struct option_data *data = opaque;
	long int num;
	int isNum = next ? parseint(next, &num) : 0;

	if (opt == 'h') {
		print_help(data->basename);
		data->done = 1;
		return 0;
	}

	if (opt == 'v') {
		print_version();
		data->done = 1;
		return 0;
	}

	if (opt == 'T') {
		data->show_time = 1;
		return 1;
	}

	/* options requiring value */
	/* FIXME: add validation */

	if (opt == 'i' && isNum) {
		data->iunit = num;
		return 2;
	}

	if (opt == 'o' && isNum) {
		data->ounit = num;
		return 2;
	}

	fprintf(stderr, "Wrong option '-%c' found.\n", opt);
	return 0;
}

int
parse_long_option(char *opt, char *next, void *opaque)
{
	struct option_data *data = opaque;
	long int num;
	int isNum = next ? parseint(next, &num) : 0;

	if (strcmp(opt, "help")==0) {
		print_help(data->basename);
		data->done = 1;
		return 0;
	}

	if (strcmp(opt, "version")==0) {
		print_version();
		data->done = 1;
		return 0;
	}

	if (strcmp(opt, "time")==0) {
		data->show_time = 1;
		return 1;
	}

	/* FIXME: validation */

	if (strcmp(opt, "input-unit")==0 && isNum) {
		data->iunit = num;
		return 2;
	}
	if (strcmp(opt, "output-unit")==0 && isNum) {
		data->ounit = num;
		return 2;
	}

	fprintf(stderr, "Wrong option '--%s' found.\n", opt);
	return 0;
}

int
parse_argument(int argn, char *arg, int is_forced, void *opaque)
{
	struct option_data *data = opaque;

	if (argn == 0) {
		/* Input file */
		if (strcmp(arg, "-")!=0 || is_forced) data->filename = arg;
		return 1;
	}

	fprintf(stderr, "Too many arguments.\n");
	return 0;
}


/* MAIN LOGIC */

int
main(int argc, char **argv)
{
	struct option_data data;
	/*struct timespec start, end;*/
	FILE *file = stdin;
	hoedown_buffer *ib, *ob;

	/* Parse options */
	data.basename = argv[0];
	data.done = 0;
	data.show_time = 0;
	data.iunit = DEF_IUNIT;
	data.ounit = DEF_OUNIT;
	data.filename = NULL;

	argc = parse_options(argc, argv, parse_short_option, parse_long_option, parse_argument, &data);
	if (data.done) return 0;
	if (!argc) return 1;

	/* Open input file, if needed */
	if (data.filename) {
		file = fopen(data.filename, "r");
		if (!file) {
			fprintf(stderr, "Unable to open input file \"%s\": %s\n", data.filename, strerror(errno));
			return 5;
		}
	}

	/* Read everything */
	ib = hoedown_buffer_new(data.iunit);

	while (!feof(file)) {
		if (ferror(file)) {
			fprintf(stderr, "I/O errors found while reading input.\n");
			return 5;
		}
		hoedown_buffer_grow(ib, ib->size + data.iunit);
		ib->size += fread(ib->data + ib->size, 1, data.iunit, file);
	}

	if (file != stdin) fclose(file);

	/* Perform SmartyPants processing */
	ob = hoedown_buffer_new(data.ounit);

	/*clock_gettime(CLOCK_MONOTONIC, &start);*/
	hoedown_html_smartypants(ob, ib->data, ib->size);
	/*clock_gettime(CLOCK_MONOTONIC, &end);*/

	/* Write the result to stdout */
	(void)fwrite(ob->data, 1, ob->size, stdout);

	/* Show rendering time */
	if (data.show_time) {
		/*TODO: enable this
		long long elapsed = (end.tv_sec - start.tv_sec)*1e9 + (end.tv_nsec - start.tv_nsec);
		if (elapsed < 1e9)
			fprintf(stderr, "Time spent on rendering: %.2f ms.\n", ((double)elapsed)/1e6);
		else
			fprintf(stderr, "Time spent on rendering: %.3f s.\n", ((double)elapsed)/1e9);
		*/
	}

	/* Cleanup */
	hoedown_buffer_free(ib);
	hoedown_buffer_free(ob);

	if (ferror(stdout)) {
		fprintf(stderr, "I/O errors found while writing output.\n");
		return 5;
	}

	return 0;
}
