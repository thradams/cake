//en.cppreference.com/w/c/string/wide/iswalnum.html
#include <stdio.h>
#include <locale.h>
#include <wchar.h>
#include <wctype.h>
 
int main(void)
{
    wchar_t c = L'\u13ad'; // the Cherokee letter HA ('Ꭽ')
    printf("in the default locale, iswalnum(%#x) = %d\n", c, !!iswalnum(c));
    setlocale(LC_ALL, "en_US.utf8");
    printf("in Unicode locale, iswalnum(%#x) = %d\n", c, !!iswalnum(c));
}