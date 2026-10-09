/* Counts lines of the files given on the command line and prints the total. */
#include <stdio.h>

static long count_lines(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    if (file == NULL)
        return -1;

    long lines = 0;
    int last = '\n';
    int ch;
    while ((ch = fgetc(file)) != EOF)
    {
        if (ch == '\n')
            lines++;
        last = ch;
    }
    /* last line without a trailing newline */
    if (last != '\n')
        lines++;

    fclose(file);
    return lines;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("usage: loc file1 [file2 ...]\n");
        return 1;
    }

    long total = 0;
    int errors = 0;
    for (int i = 1; i < argc; i++)
    {
        long lines = count_lines(argv[i]);
        if (lines < 0)
        {
            fprintf(stderr, "cannot open '%s'\n", argv[i]);
            errors++;
            continue;
        }
        printf("%8ld %s\n", lines, argv[i]);
        total += lines;
    }
    printf("%8ld total\n", total);
    return errors ? 1 : 0;
}
