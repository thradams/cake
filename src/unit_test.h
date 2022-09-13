#pragma once

extern int g_unit_test_error_count;
extern int g_unit_test_success_count;
static void assert_func(int condition, const char* func, const char* file, int line, const char* message)
{
    if (!condition)
    {
        const char* pos = file;
        const char* p = file;
        while (*p)
        {
            if (*p == '/' || *p == '\\')
                pos = p;
            p++;
        }
        
        if (*pos == '/' || *pos == '\\')
            pos++;

        g_unit_test_error_count++;
        printf("\x1b[97m" "%s:%d:0:" "\x1b[91m" " test failed:" "\x1b[0m" " function '%s'\n", pos, line, func);
        
        char buffer[20] = { 0 };
        int n = snprintf(buffer, sizeof buffer, "%d", line);        
        printf(" %s |", buffer);
        printf("    assert(%s);\n", message);
        printf(" %*s |\n", n, " ");
    }
    else
    {
        g_unit_test_success_count++;
        //printf("\x1b[97m" "%s:%d:0" "\x1b[92m" " OK" "\x1b[0m" " at '%s'\n", file, line, func);        
    }
}

#undef assert
#define assert(expression) assert_func(expression, __func__, __FILE__, __LINE__, #expression)

