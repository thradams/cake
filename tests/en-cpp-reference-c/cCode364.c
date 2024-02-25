//en.cppreference.com/w/c/string/multibyte/mbsinit.html
#include <locale.h>
#include <string.h>
#include <stdio.h>
#include <wchar.h>
 
int main(void)
{
    // allow mbrlen() to work with UTF-8 multibyte encoding
    setlocale(LC_ALL, "en_US.utf8");
    // UTF-8 narrow multibyte encoding
    const char* str = u8"水"; // or u8"\u6c34" or "\xe6\xb0\xb4"
    static mbstate_t mb; // zero-initialize
    (void)mbrlen(&str[0], 1, &mb);
    if (!mbsinit(&mb)) {
        printf("After processing the first 1 byte of %s,\n"
               "the conversion state is not initial\n\n", str);
    }
 
    (void)mbrlen(&str[1], strlen(str), &mb);
    if (mbsinit(&mb)) {
        printf("After processing the remaining 2 bytes of %s,\n"
               "the conversion state is initial conversion state\n", str);
    }
}