//en.cppreference.com/w/c/string/byte/toupper.html
#include <stdio.h>
#include <ctype.h>
#include <locale.h>
#include <limits.h>
 
int main(void)
{
    /* In the default locale: */
    for (unsigned char l = 0; l < UCHAR_MAX; l++) {
        unsigned char u = toupper(l);
        if (u != l) printf("%c%c ", l, u);
    }
    printf("\n\n");
 
    unsigned char c = '\xb8'; // the character Ž in ISO-8859-15
                              // but ´ (acute accent) in ISO-8859-1 
    setlocale(LC_ALL, "en_US.iso88591");
    printf("in iso8859-1, toupper('0x%x') gives 0x%x\n", c, toupper(c));
    setlocale(LC_ALL, "en_US.iso885915");
    printf("in iso8859-15, toupper('0x%x') gives 0x%x\n", c, toupper(c));
}