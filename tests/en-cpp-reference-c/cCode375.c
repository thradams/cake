//en.cppreference.com/w/c/string/wide/iswblank.html
/* tcc headers (msvcrt) lack what this sample uses */
#ifndef __TINYC__
#include <locale.h>
#include <stdio.h>
#include <wchar.h>
#include <wctype.h>
 
int main(void)
{
    wchar_t c = L'\u3000'; // Ideographic space ('　')
    printf("In the default locale, iswblank(%#x) = %d\n", c, !!iswblank(c));
    setlocale(LC_ALL, "en_US.utf8");
    printf("In Unicode locale, iswblank(%#x) = %d\n", c, !!iswblank(c));
}
#endif
