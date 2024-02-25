//en.cppreference.com/w/c/string/multibyte/wctomb.html
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
 
void demo(wchar_t wc)
{
    const char* dep = wctomb(NULL, wc) ? "Yes" : "No";
    printf("State-dependent encoding? %s.\n", dep);
 
    char mb[MB_CUR_MAX];
    int len = wctomb(mb, wc);
    printf("wide char '%lc' -> multibyte char [", wc);
    for (int idx = 0; idx < len; ++idx)
        printf("%s%#2x", idx ? " " : "", (unsigned char)mb[idx]);
    printf("]\n");
}
 
int main(void)
{
    setlocale(LC_ALL, "en_US.utf8");
    printf("MB_CUR_MAX = %zu\n", MB_CUR_MAX);
    demo(L'A');
    demo(L'\u00df');
    demo(L'\U0001d10b');
}