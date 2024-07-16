//en.cppreference.com/w/c/io/putwchar.html
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
 
int main()
{
    setlocale(LC_ALL, "en_US.utf8");
 
    const wchar_t data[] =
    {
        L'\u2200', // Unicode name: "FOR ALL"
        L'∀',
        L'\n',
    };
 
    for (size_t t = 0; t != (sizeof data / sizeof(wchar_t)); ++t)
    {
        if (putwchar(data[t]) == WEOF)
        {
            puts("I/O error in putwchar");
            return EXIT_FAILURE;
        }
    }
 
    return EXIT_SUCCESS;
}