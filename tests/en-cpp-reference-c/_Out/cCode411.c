//en.cppreference.com/w/c/string/wide/wmemcpy.html
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
 
int main(void)
{
    wchar_t from1[] = L"नमस्ते";
    size_t sz1 = sizeof from1 / sizeof *from1;
    wchar_t from2[] = L"Բարև";
    size_t sz2 = sizeof from2 / sizeof *from2;
    wchar_t to[sz1 + sz2];
    wmemcpy(to, from1, sz1); // copy from1, along with its null terminator
    wmemcpy(to + sz1, from2, sz2); // append from2, along with its null terminator
 
    setlocale(LC_ALL, "en_US.utf8");
    printf("Wide array contains: ");
    for(size_t n = 0; n < sizeof to / sizeof *to; ++n)
        if(to[n])
            printf("%lc", to[n]);
        else
            printf("\\0");
    printf("\n");
}