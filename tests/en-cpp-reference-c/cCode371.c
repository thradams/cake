//en.cppreference.com/w/c/string/multibyte/wctob.html
#include <locale.h>
#include <wchar.h>
#include <stdio.h>
#include <assert.h>
 
void try_narrowing(wchar_t c)
{
    int cn = wctob(c);
    if(cn != EOF)
        printf("%#x narrowed to %#x\n", c, cn);
    else
        printf("%#x could not be narrowed\n", c);
}
 
int main(void)
{
    char* utf_locale_present = setlocale(LC_ALL, "th_TH.utf8");
    assert(utf_locale_present);
    puts("In Thai UTF-8 locale:");
    try_narrowing(L'a');
    try_narrowing(L'๛');
 
    char* tis_locale_present = setlocale(LC_ALL, "th_TH.tis620");
    assert(tis_locale_present);
    puts("In Thai TIS-620 locale:");
    try_narrowing(L'a');
    try_narrowing(L'๛');
}