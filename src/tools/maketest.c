
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <assert.h>
#include <errno.h>

/*this is the name used find #ifdef TEST_DEFINE_NAME*/
#define TEST_DEFINE_NAME "TEST"

/*this is the name of the function that calls all tests*/
#define TEST_MAIN "test_main"

struct test_entry
{
    /*represents a test function or file that contains test function*/
    char name[100];

    int bFileName; /*it is a file name*/
    bool bReturnInt;
    struct test_entry* next;
};

struct test_entry_list
{
    struct test_entry* head;
    struct test_entry* tail;
};

static void test_entry_list_destroy(struct test_entry_list* list)
{
    struct test_entry* pCurrent = list->head;
    while (pCurrent)
    {
        struct test_entry* next = pCurrent->next;
        free(pCurrent);
        pCurrent = next;
    }
}

static void test_entry_list_add(struct test_entry_list* list, struct test_entry* p)
{
    if (list->tail == NULL)
    {
        list->head = p;
        list->tail = p;
    }
    else
    {
        list->tail->next = p;
        list->tail = p;
    }
}

enum Token
{
    IDENTIFER,
    OTHER,
    PREPROCESSOR_LINE,
    STRING,
};

static enum Token match(FILE* f, char* dest, int destsize)
{
    dest[0] = 0;
    char ch = fgetc(f);
    for (;;)
    {
        if (ferror(f) || feof(f))
            return OTHER;
        /*ignore spaces*/
        if (ch == ' ' || ch == '\n' || ch == '\r')
        {
            ch = fgetc(f);
            continue;
        }
        /*comment or line comment*/
        if (ch == '/')
        {
            ch = fgetc(f);
            if (ch != '/' && ch != '*')
            {
                ungetc(ch, f);
                break;
            }
            if (ch == '/')
            {
                while (
                    ((ch = fgetc(f)) != EOF) &&
                    ch != '\r' &&
                    ch != '\n' &&
                    ch != '\0')
                {
                }
            }
            else if (ch == '*')
            {
                ch = fgetc(f);
                while (!feof(f))
                {
                    if (ch == '*')
                    {
                        ch = fgetc(f);
                        if (ch == '/')
                        {
                            break;
                        }
                    }
                    ch = fgetc(f);
                }
            }
            continue;
        }
        break;
    }
    /*identifier*/
    if ((ch >= 'a' && ch <= 'z') ||
        (ch >= 'A' && ch <= 'Z') ||
        ch == '_')
    {
        int count = 0;
        dest[count] = ch;
        count++;
        while (
            ((ch = fgetc(f)) != EOF) &&
            (ch >= 'a' && ch <= 'z') ||
            (ch >= 'A' && ch <= 'Z') ||
            (ch >= '0' && ch <= '9') ||
            ch == '_')
        {
            if (count < destsize - 1)
            {
                dest[count] = ch;
                count++;
            }
        }
        ungetc(ch, f);
        dest[count] = '\0';
        return IDENTIFER;
    }
    /*string or char constant*/
    if (ch == '\'' || ch == '"')
    {
        char type = ch;
        int count = 0;
        dest[count] = ch;
        count++;
        while (((ch = fgetc(f)) != EOF))
        {
            if (ch == '\\')
            {
                if (count < destsize - 1)
                {
                    dest[count] = ch;
                    count++;
                }
                ch = fgetc(f);
                if (count < destsize - 1)
                {
                    dest[count] = ch;
                    count++;
                }
            }
            else if (ch == type)
            {
                if (count < destsize - 1)
                {
                    dest[count] = ch;
                    count++;
                }
                break;
            }
            else
            {
                if (count < destsize - 1)
                {
                    dest[count] = ch;
                    count++;
                }
            }
        }
        dest[count] = '\0';
        return STRING;
    }
    /*preprocessor line*/
    if (ch == '#')
    {
        int count = 0;
        dest[count] = ch;
        count++;
        while (((ch = fgetc(f)) != EOF) && ch != '\r' && ch != '\n')
        {
            if (count < destsize - 1)
            {
                dest[count] = ch;
                count++;
            }
        }
        if (ch == '\r')
        {
            ch = fgetc(f); // \n
        }
        dest[count] = '\0';
        return PREPROCESSOR_LINE;
    }
    /*any other*/
    dest[0] = ch;
    dest[1] = '\0';
    return OTHER;
}


static int collect_tests_from_this_file(FILE* f, struct test_entry_list* list)
{
    int num_tests = 0;

    int ifdefcount = 0;
    int b_inside_test_if_group = 0;
    while (!feof(f))
    {
        char preprocessor_line_text[100] = { 0 };
        enum Token tk = match(f, preprocessor_line_text, sizeof preprocessor_line_text);
        

        if (tk == PREPROCESSOR_LINE)
        {

            if (strcmp(preprocessor_line_text, "#ifdef " TEST_DEFINE_NAME) == 0)
            {
                
                ifdefcount++;
                b_inside_test_if_group = 1;
            }
            else if (strncmp(preprocessor_line_text, "#if", 3) == 0)
            {
                
                ifdefcount++;
            }
            else if (strcmp(preprocessor_line_text, "#endif") == 0)
            {
                

                assert(ifdefcount > 0);
                ifdefcount--;
                if (ifdefcount == 0)
                {
                    /*we are leaving the #ifdef TEST*/
                    if (b_inside_test_if_group)
                        b_inside_test_if_group = 0;
                }
            }
        }
        else if (tk == IDENTIFER && b_inside_test_if_group == 1)
        {
            // Pattern
            // void | int functionName ( optional void ) {
            do
            {
                int bReturnInt = 0;
                if (strcmp(preprocessor_line_text, "void") != 0)
                {
                    if (strcmp(preprocessor_line_text, "int") == 0)
                    {
                        bReturnInt = 1;
                    }
                    else
                        break;
                }


                tk = match(f, preprocessor_line_text, sizeof preprocessor_line_text);
                if (tk != IDENTIFER) break;
                char functionName[200] = { 0 };
                strncpy(functionName, preprocessor_line_text, sizeof functionName);
                tk = match(f, preprocessor_line_text, sizeof preprocessor_line_text);
                if (strcmp(preprocessor_line_text, "(") != 0) break;
                tk = match(f, preprocessor_line_text, sizeof preprocessor_line_text);
                if (strcmp(preprocessor_line_text, "void") == 0)
                {
                    /*optional*/
                    tk = match(f, preprocessor_line_text, sizeof preprocessor_line_text);
                }
                if (strcmp(preprocessor_line_text, ")") != 0) break;
                tk = match(f, preprocessor_line_text, sizeof preprocessor_line_text);
                if (strcmp(preprocessor_line_text, "{") != 0) break;
                //Pattern match!
                // we have void functionName ( void ) {
                //inside #if TEST group
                struct test_entry* p = calloc(1, sizeof * p);
                if (p)
                {
                    p->bFileName = 0;
                    p->bReturnInt = bReturnInt;
                    strcpy(p->name, functionName);
                    //printf("  %s\n", p->name);
                    test_entry_list_add(list, p);
                    num_tests++;
                }
            }
            while (0);
        }
    }
    return num_tests;
}

int open_file_and_collect_tests(const char* file, struct test_entry_list* list)
{
    int num_tests = 0;
    FILE* f = fopen(file, "r");
    if (f)
    {
        struct test_entry* p = calloc(1, sizeof * p);
        if (p)
        {
            p->bFileName = 1;
            strcpy(p->name, file);
            test_entry_list_add(list, p);
        }
        
        num_tests = collect_tests_from_this_file(f, list);
        //printf("%-50s %d\n", file, num_tests);


        fclose(f);
    }
    return num_tests;
}

static int generate(const char* output, struct test_entry_list* list)
{
    int error = 0;
    FILE* fout = fopen(output, "w");
    if (fout)
    {
        fprintf(fout, "/*do not edit this file*/\n\n");
        fprintf(fout, "#include <stdio.h>\n\n");
        fprintf(fout, "#ifdef " TEST_DEFINE_NAME "\n");
        fprintf(fout, "#define TESTCODE\n");
        fprintf(fout, "#endif\n");
        
        fprintf(fout, "int g_unit_test_error_count = 0;\n");
        fprintf(fout, "int g_unit_test_success_count = 0;\n");
        
        fprintf(fout, "#ifdef TESTCODE\n\n");
        fprintf(fout, "/*forward declarations*/\n");
        struct test_entry* pCurrent = list->head;
        while (pCurrent)
        {
            if (pCurrent->bFileName)
            {
                if (pCurrent->next != NULL && pCurrent->next->bFileName == 0)
                {
                    fprintf(fout, "\n");
                    fprintf(fout, "/* tests from %s*/\n", pCurrent->name);
                }
            }
            else
            {
                if (pCurrent->bReturnInt)
                  fprintf(fout, "int %s(void);\n", pCurrent->name);
                else
                    fprintf(fout, "void %s(void);\n", pCurrent->name);
            }
            pCurrent = pCurrent->next;
        }

        fprintf(fout, "\n");
        fprintf(fout, "/*end of forward declarations*/\n\n");

        

        fprintf(fout, "int " TEST_MAIN "(void)\n{\n");
        
        
        fprintf(fout, "g_unit_test_error_count = 0;\n");
        fprintf(fout, "g_unit_test_success_count = 0;\n");

        pCurrent = list->head;
        while (pCurrent)
        {
            if (pCurrent->bFileName)
            {                
            }
            else
            {                
                fprintf(fout, "    %s();\n", pCurrent->name);                                    
            }

            pCurrent = pCurrent->next;
        }
        fprintf(fout, "return g_unit_test_error_count;\n");
        
        
        fprintf(fout, "\n");
        fprintf(fout, "}\n");
        fprintf(fout, "#undef TESTCODE\n");
        fprintf(fout, "#endif /*" TEST_DEFINE_NAME "*/\n");

        fclose(fout);

        //printf("file '%s' was updated\n", output);
    }
    else
    {
        error = errno;        
    }
    return error;
}


int main(int argc, char** argv)
{
    
    printf("Maketest\n");
    if (argc < 3)
    {
        printf("usage: outputFileName.c file1.c file2.c ...\n");
        return EXIT_FAILURE;
    }
    int count = 0;
    char* outputFileName = argv[1]; //first file is the output name
    struct test_entry_list list = { 0 };
    for (int i = 2; i < (int)argc; i++)
    {
        count += open_file_and_collect_tests(argv[i], &list);
    }
    
    if (generate(outputFileName, &list) == 0)
    {
        printf("%d test functions generated at %s\n", count, outputFileName);
    }
    else
    {
        printf("error\n");
    }

    test_entry_list_destroy(&list);
    
}

