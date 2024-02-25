//en.cppreference.com/w/c/string/byte/isupper.html
#include <stdio.h>
#include <ctype.h>
#include <locale.h>
 
int main(void)
{
    unsigned char c = '\xc6'; // letter Æ in ISO-8859-1
    printf("In the default C locale, \\xc6 is %suppercase\n",
           isupper(c) ? "" : "not " );
    setlocale(LC_ALL, "en_GB.iso88591");
    printf("In ISO-8859-1 locale, \\xc6 is %suppercase\n",
           isupper(c) ? "" : "not " );
}