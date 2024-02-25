//en.cppreference.com/w/c/string/byte/isprint.html
#include <stdio.h>
#include <ctype.h>
#include <locale.h>
 
int main(void)
{
    unsigned char c = '\xa0'; // the non-breaking space in ISO-8859-1
    printf("In the default C locale, \\xa0 is %sprintable\n", isprint(c)?"":"not ");
    setlocale(LC_ALL, "en_GB.iso88591");
    printf("In ISO-8859-1 locale, \\xa0 is %sprintable\n", isprint(c)?"":"not ");
}