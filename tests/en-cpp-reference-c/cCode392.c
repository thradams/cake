//en.cppreference.com/w/c/string/wide/wcscoll.html
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
 
void try_compare(const wchar_t* p1, const wchar_t* p2)
{
    if(wcscoll(p1, p2) < 0)
        printf("%ls before %ls\n", p1, p2);
    else
        printf("%ls before %ls\n", p2, p1);
}
 
int main(void)
{
    setlocale(LC_ALL, "en_US.utf8");
    printf("In the American locale: ");
    try_compare(L"hrnec", L"chrt");
 
    setlocale(LC_COLLATE, "cs_CZ.utf8");
    printf("In the Czech locale: ");
    try_compare(L"hrnec", L"chrt");
 
    setlocale(LC_COLLATE, "en_US.utf8");
    printf("In the American locale: ");
    try_compare(L"år", L"ängel");
 
    setlocale(LC_COLLATE, "sv_SE.utf8");
    printf("In the Swedish locale: ");
    try_compare(L"år", L"ängel");
}