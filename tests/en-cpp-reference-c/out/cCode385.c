//en.cppreference.com/w/c/string/wide/iswxdigit.html
#include <locale.h>
#include <stdio.h>
#include <wchar.h>
#include <wctype.h>
 
void test(wchar_t a3, wchar_t u3, wchar_t j3)
{
    printf("\t  '%lc'  '%lc' '%lc'\n", a3, u3, j3);
    printf("iswxdigit: %d    %d    %d\n",
           !!iswxdigit(a3),
           !!iswxdigit(u3),
           !!iswxdigit(j3));
    printf("jdigit:    %d    %d    %d\n",
           !!iswctype(a3, wctype("jdigit")),
           !!iswctype(u3, wctype("jdigit")),
           !!iswctype(j3, wctype("jdigit")));
}
 
int main(void)
{
    wchar_t a3 = L'9';  // the ASCII digit 9
    wchar_t u3 = L'〩'; // the CJK numeral 9
    wchar_t j3 = L'９'; // the full-width digit 9
 
    setlocale(LC_ALL, "en_US.utf8");
    puts("In American locale:");
    test(a3, u3, j3);
 
    setlocale(LC_ALL, "ja_JP.utf8");
    puts("\nIn Japanese locale:");
    test(a3, u3, j3);
}