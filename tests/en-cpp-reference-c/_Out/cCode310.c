//en.cppreference.com/w/c/string/byte/isalnum.html
#include <stdio.h>
#include <ctype.h>
#include <locale.h>
 
int main(void)
{
    unsigned char c = '\xdf'; // German letter ß in ISO-8859-1
 
    printf("isalnum('\\xdf') in default C locale returned %d\n", !!isalnum(c));
 
    if(setlocale(LC_CTYPE, "de_DE.iso88591"))
        printf("isalnum('\\xdf') in ISO-8859-1 locale returned %d\n", !!isalnum(c));
}