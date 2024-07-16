//en.cppreference.com/w/c/io/fwprintf.html
#include <locale.h>
#include <wchar.h>
 
int main(void)
{
    char narrow_str[] = "z\u00df\u6c34\U0001f34c";
                    // or "zß水🍌"
                    // or "\x7a\xc3\x9f\xe6\xb0\xb4\xf0\x9f\x8d\x8c";
    wchar_t warr[29]; // the expected string is 28 characters plus 1 null terminator
    setlocale(LC_ALL, "en_US.utf8");
    swprintf(warr, sizeof warr/sizeof *warr,
              L"Converted from UTF-8: '%s'", narrow_str);
    wprintf(L"%ls\n", warr);
}