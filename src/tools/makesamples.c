/*
    make_samples.c

    Usage:

        make_samples samples

    Generates:

        samples.js

    Directory layout:

        samples/
            C89/
                bit-fields.c
                enums.c
            C99/
                compound-literal.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

static FILE *out;

struct category {
    char name[256];
};

static struct category categories[256];
static int category_count = 0;

static int category_exists(const char *name)
{
    for (int i = 0; i < category_count; i++) {
        if (strcmp(categories[i].name, name) == 0)
            return 1;
    }
    return 0;
}

/* Write a string as a double-quoted JS string literal, escaping
   everything that could break out of the literal (backslash, quote,
   control characters). Used for object keys such as category and
   sample names, which are not run through write_file_contents. */
static void write_js_string_literal(const char *s)
{
    fputc('"', out);

    for (const unsigned char *p = (const unsigned char *)s; *p; p++)
    {
        unsigned char c = *p;

        switch (c)
        {
        case '\\':
            fputs("\\\\", out);
            break;

        case '"':
            fputs("\\\"", out);
            break;

        case '\n':
            fputs("\\n", out);
            break;

        case '\r':
            fputs("\\r", out);
            break;

        case '\t':
            fputs("\\t", out);
            break;

        default:
            if (c < 0x20)
                fprintf(out, "\\x%02X", c);
            else
                fputc(c, out);
        }
    }

    fputc('"', out);
}

/* Directories that hold compiler output rather than samples. */
static int is_output_dir(const char *name)
{
    if (strcmp(name, "macos_arm64") == 0 ||
        strcmp(name, "x64_msvc") == 0 ||
        strcmp(name, "x86_msvc") == 0 ||
        strcmp(name, "x86_x64_gcc") == 0)
    {
        return 1;
    }
    return 0;
}

static void add_category(const char *name)
{
    if (category_exists(name))
        return;

    strcpy(categories[category_count++].name, name);

    fprintf(out, "sample[");
    write_js_string_literal(name);
    fprintf(out, "] = [];\n\n");
}

static void write_file_contents(FILE *f)
{
    int c;

    while ((c = fgetc(f)) != EOF)
    {
        switch (c)
        {
        case '\\':
            fputs("\\\\", out);
            break;

        case '"':
            fputs("\\\"", out);
            break;

        case '`':
            /* would otherwise terminate the JS template literal */
            fputs("\\`", out);
            break;

        case '$':
            /* would otherwise start a ${...} interpolation in the template literal */
            fputs("\\$", out);
            break;

        case '\n':
            fputs("\\n", out);
            break;

        case '\r':
            /* ignore CR */
            break;

        case '\t':
            fputs("\\t", out);
            break;

        default:
            if ((unsigned char)c < 0x20)
            {
                fprintf(out, "\\x%02X", (unsigned char)c);
            }
            else
            {
                fputc(c, out);
            }
        }
    }
}


static void process_file(const char *root, const char *path)
{
    char relative[PATH_MAX];

    strcpy(relative, path + strlen(root));

    if (relative[0] == '/' || relative[0] == '\\')
        memmove(relative, relative + 1, strlen(relative));

    /* Split off just the first path component as the category. Any
       further separators in "rest" (e.g. a nested sub-directory like
       flow3\x86_x64_gcc\zz-try4.c) are kept as-is and become part of
       the sample name/key rather than being split further -- they
       just need to be escaped correctly for JS, which
       write_js_string_literal now handles. */
    char *slash = strpbrk(relative, "/\\");
    if (!slash)
        return;

    *slash = '\0';

    const char *category = relative;
    const char *rest = slash + 1;

    const char *dot = strrchr(rest, '.');
    if (!dot)
        return;

    if (strcmp(dot, ".c") != 0)
        return;

    add_category(category);

    char sample_name[256];
    size_t len = (size_t)(dot - rest);

    if (len >= sizeof(sample_name))
        len = sizeof(sample_name) - 1;

    memcpy(sample_name, rest, len);
    sample_name[len] = '\0';

    FILE *f = fopen(path, "rb");
    if (!f) {
        perror(path);
        return;
    }

    fprintf(out, "sample[");
    write_js_string_literal(category);
    fprintf(out, "][");
    write_js_string_literal(sample_name);
    fprintf(out, "] =\n`\n");

    write_file_contents(f);

    fprintf(out,
        "\n`;\n\n");

    fclose(f);
}

#ifdef _WIN32

static void walk(const char *root, const char *dir)
{
    char pattern[PATH_MAX];
    WIN32_FIND_DATAA fd;

    snprintf(pattern, sizeof(pattern), "%s\\*", dir);

    HANDLE h = FindFirstFileA(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE)
        return;

    do {

        if (strcmp(fd.cFileName, ".") == 0 ||
            strcmp(fd.cFileName, "..") == 0)
            continue;

        char path[PATH_MAX];

        snprintf(path,
                 sizeof(path),
                 "%s\\%s",
                 dir,
                 fd.cFileName);

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (!is_output_dir(fd.cFileName))
            {
                walk(root, path);
            }
        }
        else
            process_file(root, path);

    } while (FindNextFileA(h, &fd));

    FindClose(h);
}

#else

static void walk(const char *root, const char *dir)
{
    DIR *d = opendir(dir);
    if (!d)
        return;

    struct dirent *entry;

    while ((entry = readdir(d)) != NULL) {

        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;

        char path[PATH_MAX];

        snprintf(path,
                 sizeof(path),
                 "%s/%s",
                 dir,
                 entry->d_name);

        struct stat st;

        if (stat(path, &st) != 0)
            continue;

        if (S_ISDIR(st.st_mode))
        {
            if (!is_output_dir(entry->d_name))
            {
                walk(root, path);
            }
        }
        else if (S_ISREG(st.st_mode))
            process_file(root, path);
    }

    closedir(d);
}

#endif

int main(int argc, char **argv)
{
    const char *root = ".";

    if (argc > 1)
        root = argv[1];

    out = fopen("samples.js", "wb");
    if (!out) {
        perror("samples.js");
        return EXIT_FAILURE;
    }

    fprintf(out, "var sample = {};\n\n");

    walk(root, root);

    fclose(out);

    printf("Generated samples.js\n");

    return EXIT_SUCCESS;
}