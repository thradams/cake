//en.cppreference.com/w/c/string/wide/wcscat.html
#include <wchar.h> 
#include <stdio.h>
#include <locale.h>
 
int main(void) 
{
    wchar_t str[50] = L"Земля, прощай.";
    wcscat(str, L" ");
    wcscat(str, L"В добрый путь.");
    setlocale(LC_ALL, "en_US.utf8");
    printf("%ls", str);
}