//en.cppreference.com/w/c/string/wide/wcsncat.html
#include <wchar.h> 
#include <stdio.h>
#include <locale.h>
 
int main(void) 
{
    wchar_t str[50] = L"Земля, прощай.";
    wcsncat(str, L" ", 1);
    wcsncat(str, L"В добрый путь.", 8); // only append the first 8 wide chars
    setlocale(LC_ALL, "en_US.utf8");
    printf("%ls", str);
}