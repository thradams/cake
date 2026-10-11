/*
    This file is part of cake compiler
    https://github.com/thradams/cake

    Joins C files and the local headers they include into one file.
*/
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
 
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
#define strdup _strdup
#define realpath(a,b) _fullpath(b, a, 250)
#else
#include <unistd.h>
#endif 

int strcicmp(char const* a, char const* b)
{
    for (;; a++, b++)
    {
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0 || !*a)
            return d;
    }
}


struct strlist_node
{
    char* fileName;
    struct strlist_node* pNext;
};

struct strlist
{
    struct strlist_node* head;
};


void strlist_append(const char* filename, struct strlist_node** s_included)
{

    struct strlist_node* pNew = malloc(sizeof * pNew);
    pNew->fileName = strdup(filename);
    pNew->pNext = 0;

    if (*s_included == NULL)
    {
        *s_included = pNew;
    }
    else
    {
        pNew->pNext = *s_included;
        *s_included = pNew;
    }
}


void mark_as_included(const char* filename0, struct strlist_node** s_included)
{
    char filename[200];
    realpath(filename0, filename);
    //printf("filename %s \n", filename);
    strlist_append(filename, s_included);
}

bool strlist_has(const char* filename0, struct strlist_node** s_included)
{
    char filename[200];
    if(realpath(filename0, filename) == NULL)
    {
        return false;
    }

    bool result = false;
    struct strlist_node* pCurrent = *s_included;
    while (pCurrent)
    {
        if (strcicmp(pCurrent->fileName, filename) == 0)
        {
            result = true;
            break;
        }
        pCurrent = pCurrent->pNext;
    }
    return result;
}

void strlist_free(struct strlist_node** s_included)
{
    struct strlist_node* pCurrent = *s_included;
    while (pCurrent)
    {
        struct strlist_node* pNext = pCurrent->pNext;
        free(pCurrent->fileName);
        free(pCurrent);
        pCurrent = pNext;
    }

}

int str_ends_with(const char* str, const char* suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix > lenstr)
        return 0;
    return strcicmp(str + lenstr - lensuffix, suffix) == 0;
}

/*
    Returns whether a block comment is still open at the end of the line.
*/
static bool scan_comments(const char* p, bool in_comment)
{
    while (*p)
    {
        if (in_comment)
        {
            if (p[0] == '*' && p[1] == '/')
            {
                in_comment = false;
                p++;
            }
        }
        else if (p[0] == '/' && p[1] == '*')
        {
            in_comment = true;
            p++;
        }
        else if (p[0] == '/' && p[1] == '/')
        {
            break;
        }
        else if (*p == '"' || *p == '\'')
        {
            const char q = *p++;
            while (*p && *p != q)
            {
                if (*p == '\\' && p[1])
                    p++;
                p++;
            }
            if (*p == '\0')
                break;
        }
        p++;
    }
    return in_comment;
}

bool Write(const char* name, bool bHeaderMode, FILE* out, struct strlist_node** s_included)
{

    bool cppfile = str_ends_with(name, ".c");

    bool found = false;

    struct strlist_node* defineList = 0;


    FILE* input = fopen(name, "r");

    if (input == 0)
    {
        //char name2[200];
        //snprintf(name2, 200, "./openssl/Lib141s/Include/%s", name);
        //input = fopen(name2, "r");
    }

    if (input)
    {
        fputs("\n", out);
        assert(!strlist_has(name, s_included));
        mark_as_included(name, s_included);
        found = true;
        bool bInclude = bHeaderMode ? false : true;
        bool first_line = true;
        bool in_comment = false;
        char line[8192];
        int line_number = 0;
        /*
            #line is emitted before the next line written whenever the output
            stopped matching the source lines: at the start of the file, after an
            #include and after lines skipped in header mode.
        */
        bool need_line_directive = true;

        while (fgets(line, sizeof line, input))
        {
            char* linebuffer = line;
            if (first_line && strncmp(linebuffer, "\xEF\xBB\xBF", 3) == 0)
                linebuffer += 3; //skip UTF-8 BOM
            first_line = false;
            line_number++;

            if (need_line_directive && bInclude)
            {
                fputs("#line ", out);
                fprintf(out, "%d \"", line_number);
                for (const char* p = name; *p; p++)
                    fputc(*p == '\\' ? '/' : *p, out);
                fputs("\"\n", out);
                need_line_directive = false;
            }

            const char* pChar = linebuffer;
            while (*pChar == ' ' || *pChar == '\t')
                pChar++;

            const bool line_starts_in_comment = in_comment;
            in_comment = scan_comments(linebuffer, in_comment);

            if (*pChar == '#' && !line_starts_in_comment)
            {
                linebuffer[strcspn(linebuffer, "\r\n")] = '\0';

                pChar++; //skip #

                while (*pChar == ' ' || *pChar == '\t') //skip spaces
                    pChar++;

                if (strncmp(pChar, "//BEGIN_EXPORT", sizeof("//BEGIN_EXPORT") - 1) == 0)
                {
                    //fputs(linebuffer, out);
                    fputs("\n", out);
                    if (bHeaderMode)
                    {
                        bInclude = true;
                    }
                }
                else if (strncmp(pChar, "//END_EXPORT", sizeof("//END_EXPORT") - 1) == 0)
                {
                    //fputs(linebuffer, out);
                    fputs("\n", out);
                    if (bHeaderMode)
                    {
                        bInclude = false;
                    }
                }
                else if (bInclude && strncmp(pChar, "define", sizeof("define") - 1) == 0)
                {



                    fputs(linebuffer, out);
                    fputs("\n", out);

                    if (cppfile)
                    {
                        //guardar todos defines feitos em ccpp e fazer undef
                        pChar += sizeof("define") - 1; //skip pragma

                        while (*pChar == ' ') //skip spaces
                            pChar++;

                        char defineName[200] = { 0 };
                        int k = 0;
                        while (*pChar != '\0' && *pChar != ' ' && *pChar != '(' && *pChar != '\n')
                        {
                            defineName[k] = *pChar;
                            k++;
                            pChar++;
                        }
                        strlist_append(defineName, &defineList);
                    }
                }
                else if (bInclude && strncmp(pChar, "pragma", sizeof("pragma") - 1) == 0)
                {
                    pChar += sizeof("pragma") - 1; //skip pragma

                    while (*pChar == ' ') //skip spaces
                        pChar++;

                    if (strncmp(pChar, "once", sizeof("once") - 1) == 0)
                    {
                        fputs("//", out);
                        fputs(linebuffer, out);
                        fputs("\n", out);
                    }
                    else
                    {
                        fputs(linebuffer, out);
                        fputs("\n", out);
                    }
                }
                else if (bInclude && strncmp(pChar, "include", sizeof("include") - 1) == 0)
                {
                    pChar += sizeof("include") - 1; //match include

                    while (*pChar == ' ') //skip spaces          
                        pChar++;

                    if (*pChar == '"' || *pChar == '<')
                    {
                        pChar++;//match " or <

                        char fileName[200] = { 0 };
                        int k = 0;
                        while (*pChar)
                        {
                            if (*pChar == '\"' || *pChar == '>')
                            {
                                break;
                            }
                            fileName[k] = *pChar;
                            pChar++;
                            k++;
                        }

                        if (strlist_has(fileName, s_included))
                        {
                            fputs("\n", out);
                        }
                        else if (Write(fileName, bHeaderMode, out, s_included))
                        {
                            need_line_directive = true;
                        }
                        else
                        {
                            fputs(linebuffer, out);
                            fputs("\n", out);
                        }
                    }
                    else
                    {
                        fputs(linebuffer, out);
                        fputs("\n", out);
                    }
                }
                else
                {
                    if (bInclude)
                    {
                        fputs(linebuffer, out);
                        fputs("\n", out);
                    }
                }
            }
            else if (bInclude)
            {
                fputs(linebuffer, out);
            }

            if (!bInclude)
                need_line_directive = true;
        }

        fprintf(out, "\n");
        struct strlist_node* pCurrent = defineList;
        while (pCurrent)
        {
            struct strlist_node* pNext = pCurrent->pNext;
            //fprintf(out, "#undef %s \n", pCurrent->fileName);


            free(pCurrent->fileName);
            free(pCurrent);
            pCurrent = pNext;
        }

        fclose(input);
    }


    return found;
}



int main(int argc, char** argv)
{
    if (argc <= 1)
    {
        printf("usage: amalgamator [options] file1.c [file2.c ...]\n");
        printf("options:\n");
        printf("  -o<file>  output file (default amalgamated.c)\n");
        printf("  -h        header mode: only the //BEGIN_EXPORT .. //END_EXPORT parts\n");
        return 1;
    }

    char file_name_out[200] = "amalgamated.c";
    bool bHeaderMode = false;

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
            continue;

        if (argv[i][1] == 'o')
        {
            strcpy(file_name_out, argv[i] + 2);
        }
        else if (argv[i][1] == 'h')
        {
            bHeaderMode = true;
        }

    }

    struct strlist_node* s_included = 0;


    FILE* out = fopen(file_name_out, "w");
    if (out)
    {
        fprintf(out, "/*\n");
        fprintf(out, " * THIS FILE WAS AUTOMATICALLY GENERATED BY THE AMALGAMATOR TOOL.\n");
        fprintf(out, " * DO NOT EDIT THIS FILE DIRECTLY - ANY CHANGES WILL BE LOST.\n");
        fprintf(out, " * Edit the original source files instead and re-run the amalgamator.\n");
        fprintf(out, " */\n");

        for (int i = 1; i < argc; i++)
        {
            if (argv[i][0] == '-')
                continue;

            if (!strlist_has(argv[i], &s_included))
            {
                Write(argv[i], bHeaderMode, out, &s_included);
            }

        }

        fclose(out);
        printf("Amalgamated file %s generated.\n", file_name_out);
    }
    else
    {
        printf("error\n");
    }

    strlist_free(&s_included);

    return 0;
}
