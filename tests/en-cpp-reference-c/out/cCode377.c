//en.cppreference.com/w/c/string/wide/iswctype.html
#include <locale.h>
#include <stdio.h>
#include <wchar.h>
#include <wctype.h>
 
const char* classify(wchar_t wc, const char* cat)
{
    return iswctype(wc, wctype(cat)) ? "true" : "false";
}
 
int main(void)
{
    setlocale(LC_ALL, "ja_JP.UTF-8");
    puts("The character \u6c34 is...");
    const char* cats[] = {"digit", "alpha", "space", "cntrl", "jkanji"};
    for (int n = 0; n < 5; ++n)
        printf("%s?\t%s\n", cats[n], classify(L'\u6c34', cats[n]));
}