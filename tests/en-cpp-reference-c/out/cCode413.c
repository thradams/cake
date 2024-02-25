//en.cppreference.com/w/c/string/wide/wmemset.html
#include <locale.h>
#include <stdio.h>
#include <wchar.h>
 
int main(void)
{
    wchar_t ar[10] = L"1234567890"; // no trailing null in the array
    wmemset(ar, L'\U0001f34c', 5); // replaces [12345] with the 🍌 bananas
    wmemset(ar + 5, L'蕉', 5); // replaces [67890] with the 蕉 bananas
 
    setlocale(LC_ALL, "en_US.utf8");
    for (size_t n = 0; n < sizeof ar / sizeof *ar; ++n)
        putwchar(ar[n]);
    putwchar(L'\n');
}