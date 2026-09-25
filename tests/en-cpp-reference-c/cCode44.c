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
#ifdef _WIN32 /* 16-bit wchar_t cannot hold U+1F34C */
    if (fputwc(L'a', stdout) == WEOF)
#else
    if (fputwc(L'🍌', stdout) == WEOF)
#endif
    {
        if (errno == EILSEQ)
            puts("Encoding error in fputwc.");
        else
            puts("I/O error in fputwc.");
        return EXIT_FAILURE;
    }
}