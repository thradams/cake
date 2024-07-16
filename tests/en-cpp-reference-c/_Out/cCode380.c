//en.cppreference.com/w/c/string/wide/iswlower.html
#include <locale.h>
#include <stdio.h>
#include <wchar.h>
#include <wctype.h>
 
int main(void)
{
    wchar_t c = L'\u0444'; // Cyrillic small letter ef ('ф')
    printf("In the default locale, iswlower(%#x) = %d\n", c, !!iswlower(c));
    setlocale(LC_ALL, "en_US.utf8");
    printf("In Unicode locale, iswlower(%#x) = %d\n", c, !!iswlower(c));
}