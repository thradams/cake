//en.cppreference.com/w/c/string/wide/iswalpha.html
#include <locale.h>
#include <stdio.h>
#include <wchar.h>
#include <wctype.h>
 
int main(void)
{
    wchar_t c = L'\u0b83'; // Tamil sign Visarga ('ஃ')
    printf("in the default locale, iswalpha(%#x) = %d\n", c, !!iswalpha(c));
    setlocale(LC_ALL, "en_US.utf8");
    printf("in Unicode locale, iswalpha(%#x) = %d\n", c, !!iswalpha(c));
}