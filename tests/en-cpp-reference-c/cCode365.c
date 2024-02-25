//en.cppreference.com/w/c/string/multibyte/mbsrtowcs.html
#include <stdio.h>
#include <locale.h>
#include <wchar.h>
#include <string.h>
 
void print_as_wide(const char* mbstr)
{
    mbstate_t state;
    memset(&state, 0, sizeof state);
    size_t len = 1 + mbsrtowcs(NULL, &mbstr, 0, &state);
    wchar_t wstr[len];
    mbsrtowcs(&wstr[0], &mbstr, len, &state);
    wprintf(L"Wide string: %ls \n", wstr);
    wprintf(L"The length, including L'\\0': %zu\n", len);
}
 
int main(void)
{
    setlocale(LC_ALL, "en_US.utf8");
    print_as_wide(u8"z\u00df\u6c34\U0001f34c"); // u8"zß水🍌"
}