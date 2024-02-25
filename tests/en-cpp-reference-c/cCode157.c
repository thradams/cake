//en.cppreference.com/w/c/locale/LC_categories.html
#include <locale.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
 
int main(void)
{
    setlocale(LC_ALL, "en_US.UTF-8"); // the C locale will be the UTF-8 enabled English
    setlocale(LC_NUMERIC, "de_DE.utf8"); // decimal dot will be German
    setlocale(LC_TIME, "ja_JP.utf8");    // date/time formatting will be Japanese
    wchar_t str[100];
    time_t t = time(NULL);
    wcsftime(str, 100, L"%A %c", localtime(&t));
    wprintf(L"Number: %.2f\nDate: %Ls\n", 3.14, str);
}