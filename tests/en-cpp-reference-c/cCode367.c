//en.cppreference.com/w/c/string/multibyte/mbtowc.html
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
 
// print multibyte string to wide-oriented stdout
// equivalent to wprintf(L"%s\n", ptr);
void print_mb(const char* ptr)
{
    mbtowc(NULL, 0, 0); // reset the conversion state
    const char* end = ptr + strlen(ptr);
    int ret = 0;
    for (wchar_t wc; (ret = mbtowc(&wc, ptr, end - ptr)) > 0; ptr += ret)
        wprintf(L"%lc", wc);
    wprintf(L"\n");
}
 
int main(void)
{
    setlocale(LC_ALL, "en_US.utf8");
    // UTF-8 narrow multibyte encoding
    print_mb(u8"z\u00df\u6c34\U0001F34C"); // or u8"zß水🍌"
}