//en.cppreference.com/w/c/string/wide/wcsstr.html
#include <stdio.h>
#include <locale.h>
#include <wchar.h>
 
int main(void)
{
    setlocale(LC_ALL, "ru_RU.UTF-8");
 
    wchar_t str[5][64] = {
        L"Строка, где есть подстрока 'но'.",
        L"Строка, где такой подстроки нет.",
        L"Он здесь.",
        L"Здесь он.",
        L"Его нет."
    };
 
    for (size_t i = 0; i < 5; ++i) {
        if (wcsstr(str[i], L"но")) {
            wprintf(L"%ls\n", str[i]);
        }
    }
}