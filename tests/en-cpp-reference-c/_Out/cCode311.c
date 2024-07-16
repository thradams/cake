//en.cppreference.com/w/c/string/byte/isalpha.html
#include <ctype.h>
#include <stdio.h>
#include <locale.h>
 
int main(void)
{
    unsigned char c = '\xdf'; // German letter ß in ISO-8859-1
 
    printf("isalpha('\\xdf') in default C locale returned %d\n", !!isalpha(c));
 
    setlocale(LC_CTYPE, "de_DE.iso88591");
    printf("isalpha('\\xdf') in ISO-8859-1 locale returned %d\n", !!isalpha(c));
}