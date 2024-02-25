//en.cppreference.com/w/c/string/wide/iswprint.html
#include <locale.h>
#include <stdio.h>
#include <wchar.h>
#include <wctype.h>
 
int main(void)
{
    wchar_t c = L'\u2002'; // Unicode character 'EN SPACE'
    printf("in the default locale, iswprint(%#x) = %d\n", c, !!iswprint(c));
    setlocale(LC_ALL, "en_US.utf8");
    printf("in Unicode locale, iswprint(%#x) = %d\n", c, !!iswprint(c));
    wchar_t c2 = L'\x82'; // break permitted
    printf("in Unicode locale, iswprint(%#x) = %d\n", c2, !!iswprint(c2));
}