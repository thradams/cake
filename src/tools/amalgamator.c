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
    for (;; a++, b++) {
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
    realpath(filename0, filename);

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

bool Write(const char* name, bool bHeaderMode, FILE* out, struct strlist_node** s_included)
{

    bool cppfile = str_ends_with(name, ".c");

    fputs("\n", out);
    bool found = false;

    assert(!strlist_has(name, s_included));
    mark_as_included(name, s_included);

    struct strlist_node* defineList = 0;


    char previous = '\0';

    FILE* input = fopen(name, "r");

    if (input == 0)
    {
        //char name2[200];
        //snprintf(name2, 200, "./openssl/Lib141s/Include/%s", name);
        //input = fopen(name2, "r");
    }

    if (input)
    {
        found = true;
        char c = '\0';

        bool bInclude = bHeaderMode ? false : true;

        while (!feof(input))
        {
            previous = c;
            c = fgetc(input);
            if ((previous == '\0' || previous == '\n') && c == '#')
            {
                c = fgetc(input);
                char linebuffer[500];
                int i = 0;
                linebuffer[i] = '#';
                i++;
                while (c != '\n' && c != EOF)
                {
                    linebuffer[i] = c;
                    c = fgetc(input);
                    i++;
                }
                linebuffer[i] = '\0';
                const char* pChar = linebuffer;

                pChar++; //skip #

                while (*pChar == ' ') //skip spaces
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

                        if (!strlist_has(fileName, s_included))
                        {
                            fputs("\n", out);
                            if (!Write(fileName, bHeaderMode, out, s_included))
                            {
                                fputs(linebuffer, out);
                                fputs("\n", out);
                            }
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
            else
            {
                if (c != EOF && bInclude)
                {
                    fputc(c, out);
                }
            }
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
        printf(" [options] file1.c file2.c ...\n");
        printf(" options:\n");
        printf(" -o output name\n");
        printf(" -h header mode\n");
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
            strcpy(file_name_out, argv[i]+2);
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
