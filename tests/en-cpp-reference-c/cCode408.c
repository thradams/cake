//en.cppreference.com/w/c/string/wide/wcsxfrm.html
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
 
int main(void)
{
    setlocale(LC_ALL, "sv_SE.utf8");
 
    const wchar_t *in1 = L"\u00e5r";
    wchar_t out1[1+wcsxfrm(NULL, in1, 0)];
    wcsxfrm(out1, in1, sizeof out1/sizeof *out1);
 
    const wchar_t *in2 = L"\u00e4ngel";
    wchar_t out2[1+wcsxfrm(NULL, in2, 0)];
    wcsxfrm(out2, in2, sizeof out2/sizeof *out2);
 
    printf("In the Swedish locale: ");
    if(wcscmp(out1, out2) < 0)
         printf("%ls before %ls\n", in1, in2);
    else
         printf("%ls before %ls\n", in2, in1);
 
    printf("In lexicographical comparison: ");
    if(wcscmp(in1, in2) < 0)
         printf("%ls before %ls\n", in1, in2);
    else
         printf("%ls before %ls\n", in2, in1);
}