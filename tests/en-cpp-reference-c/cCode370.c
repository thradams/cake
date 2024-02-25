//en.cppreference.com/w/c/string/multibyte/wcstombs.html
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
 
int main(void)
{
    // 4 wide characters
    const wchar_t src[] = L"z\u00df\u6c34\U0001f34c";
    // they occupy 10 bytes in UTF-8
    char dst[11];
 
    setlocale(LC_ALL, "en_US.utf8");
    printf("wide-character string: '%ls'\n",src);
    for (size_t ndx=0; ndx < sizeof src/sizeof src[0]; ++ndx)
        printf("   src[%2zu] = %#8x\n", ndx, src[ndx]);
 
    int rtn_val = wcstombs(dst, src, sizeof dst);
    printf("rtn_val = %d\n", rtn_val);
    if (rtn_val > 0)
        printf("multibyte string:  '%s'\n",dst);
    for (size_t ndx=0; ndx<sizeof dst; ++ndx)
        printf("   dst[%2zu] = %#2x\n", ndx, (unsigned char)dst[ndx]);
}