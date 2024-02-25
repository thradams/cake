//en.cppreference.com/w/c/string/wide/iswupper.html
#include <locale.h>
#include <stdio.h>
#include <wchar.h>
#include <wctype.h>
 
int main(void)
{
    wchar_t c = L'\u053d'; // Armenian capital letter xeh ('Խ')
    printf("In the default locale, iswupper(%#x) = %d\n", c, !!iswupper(c));
    setlocale(LC_ALL, "en_US.utf8");
    printf("In Unicode locale, iswupper(%#x) = %d\n", c, !!iswupper(c));
}