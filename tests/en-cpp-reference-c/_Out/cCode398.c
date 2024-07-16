//en.cppreference.com/w/c/string/wide/wcsncpy.html
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
 
int main(void)
{
    const wchar_t src[] = L"わゐ";
    wchar_t dest[6] = {L'あ', L'い', L'う', L'え', L'お'};
 
    wcsncpy(dest, src, 4); // this will copy わゐ and repeat L'\0' two times
 
    puts("The contents of dest are: ");
    setlocale(LC_ALL, "en_US.utf8");
 
    const long dest_size = sizeof dest / sizeof *dest;
    for(wchar_t* p = dest; p-dest != dest_size; ++p) {
        *p ? printf("%lc ", *p)
           : printf("\\0 ");
    }
}