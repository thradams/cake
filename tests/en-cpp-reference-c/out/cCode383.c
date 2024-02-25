//en.cppreference.com/w/c/string/wide/iswspace.html
#include <locale.h>
#include <stdio.h>
#include <wchar.h>
#include <wctype.h>
 
int main(void)
{
    wchar_t c = L'\u2003'; // Unicode character 'EM SPACE'
    printf("In the default locale, iswspace(%#x) = %d\n", c, !!iswspace(c));
    setlocale(LC_ALL, "en_US.utf8");
    printf("In Unicode locale, iswspace(%#x) = %d\n", c, !!iswspace(c));
}