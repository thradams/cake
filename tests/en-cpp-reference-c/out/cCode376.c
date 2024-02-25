//en.cppreference.com/w/c/string/wide/iswcntrl.html
#include <locale.h>
#include <stdio.h>
#include <wchar.h>
#include <wctype.h>
 
int main(void)
{
    wchar_t c = L'\u2028'; // the Unicode character "line separator"
    printf("In the default locale, iswcntrl(%#x) = %d\n", c, !!iswcntrl(c));
    setlocale(LC_ALL, "en_US.utf8");
    printf("In Unicode locale, iswcntrl(%#x) = %d\n", c, !!iswcntrl(c));
}