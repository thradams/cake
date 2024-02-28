//en.cppreference.com/w/c/string/byte/strerror.html
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <locale.h>
 
int main(void)
{
    FILE *fp = fopen(tmpnam((char[L_tmpnam]){0}), "r");
    if(fp==NULL) {
        printf("File opening error: %s\n", strerror(errno));
        setlocale(LC_MESSAGES, "de_DE.utf8");
        printf("Now in German: %s\n", strerror(errno));
#ifdef __STDC_LIB_EXT1__
        setlocale(LC_ALL, "ja_JP.utf8"); // printf needs CTYPE for multibyte output
        size_t errmsglen = strerrorlen_s(errno) + 1;
        char errmsg[errmsglen]; 
        strerror_s(errmsg, errmsglen, errno);
        printf("Now in Japanese: %s\n", errmsg);
#endif
    }
}