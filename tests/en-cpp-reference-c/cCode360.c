//en.cppreference.com/w/c/string/multibyte/mbrlen.html
#include <locale.h>
#include <string.h>
#include <stdio.h>
#include <wchar.h>
 
int main(void)
{   
    // allow mbrlen() to work with UTF-8 multibyte encoding
    setlocale(LC_ALL, "en_US.utf8");
    // UTF-8 narrow multibyte encoding
    const char* str = u8"水";
    size_t sz = strlen(str);
 
    mbstate_t mb;
    memset(&mb, 0, sizeof mb);
    int len1 = mbrlen(str, 1, &mb);
    if(len1 == -2) 
        printf("The first 1 byte of %s is an incomplete multibyte char"
               " (mbrlen returns -2)\n", str);
 
    int len2 = mbrlen(str+1, sz-1, &mb);
    printf("The remaining %zu  bytes of %s hold %d bytes of the multibyte"
           " character\n", sz-1, str, len2);
 
    printf("Attempting to call mbrlen() in the middle of %s while in initial"
           " shift state returns %zd\n", str, mbrlen(str+1, sz-1, &mb));
}