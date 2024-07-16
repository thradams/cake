//en.cppreference.com/w/c/io/fputwc.html
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
 
int main(void)
{
    setlocale(LC_ALL, "en_US.utf8");
 
    errno = 0;
    if (fputwc(L'🍌', stdout) == WEOF)
    {
        if (errno == EILSEQ)
            puts("Encoding error in fputwc.");
        else
            puts("I/O error in fputwc.");
        return EXIT_FAILURE;
    }
}