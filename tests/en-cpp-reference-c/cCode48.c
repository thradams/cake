//en.cppreference.com/w/c/io/fscanf.html
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include <stddef.h>
#include <locale.h>
 
int main(void)
{
    int i, j;
    float x, y;
    char str1[10], str2[4];
    wchar_t warr[2];
    setlocale(LC_ALL, "en_US.utf8");
 
    char input[] = "25 54.32E-1 Thompson 56789 0123 56ß水";
    /* parse as follows:
       %d: an integer
       %f: a floating-point value
       %9s: a string of at most 9 non-whitespace characters
       %2d: two-digit integer (digits 5 and 6)
       %f:  a floating-point value (digits 7, 8, 9)
       %*d: an integer which isn't stored anywhere
       ' ': all consecutive whitespace
       %3[0-9]: a string of at most 3 decimal digits (digits 5 and 6)
       %2lc: two wide characters, using multibyte to wide conversion  */
    int ret = sscanf(input, "%d%f%9s%2d%f%*d %3[0-9]%2lc",
                     &i, &x, str1, &j, &y, str2, warr);
 
    printf("Converted %d fields:\n"
           "i = %d\n"
           "x = %f\n"
           "str1 = %s\n"
           "j = %d\n"
           "y = %f\n"
           "str2 = %s\n"
           "warr[0] = U+%x\n"
           "warr[1] = U+%x\n",
           ret, i, x, str1, j, y, str2, warr[0], warr[1]);
 
#ifdef __STDC_LIB_EXT1__
    int n = sscanf_s(input, "%d%f%s", &i, &x, str1, (rsize_t)sizeof str1);
    // writes 25 to i, 5.432 to x, the 9 bytes "Thompson\0" to str1, and 3 to n.
#endif
}