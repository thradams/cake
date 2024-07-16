//en.cppreference.com/w/c/string/multibyte/wcsrtombs.html
#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <wchar.h>
 
void print_wide(const wchar_t* wstr)
{
    mbstate_t state;
    memset(&state, 0, sizeof state);
    size_t len = 1 + wcsrtombs(NULL, &wstr, 0, &state);
    char mbstr[len];
    wcsrtombs(mbstr, &wstr, len, &state);
    printf("Multibyte string: %s\n", mbstr);
    printf("Length, including '\\0': %zu\n", len);
}
 
int main(void)
{
    setlocale(LC_ALL, "en_US.utf8");
    print_wide(L"z\u00df\u6c34\U0001f34c"); // or L"zß水🍌"
}