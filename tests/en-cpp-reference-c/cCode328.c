//en.cppreference.com/w/c/string/byte/strcat.html
#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h> 
#include <stdio.h>
#include <stdlib.h>
 
int main(void) 
{
    char str[50] = "Hello ";
    char str2[50] = "World!";
    strcat(str, str2);
    strcat(str, " ...");
    strcat(str, " Goodbye World!");
    puts(str);
 
#ifdef __STDC_LIB_EXT1__
    set_constraint_handler_s(ignore_handler_s);
    int r = strcat_s(str, sizeof str, " ... ");
    printf("str = \"%s\", r = %d\n", str, r);
    r = strcat_s(str, sizeof str, " and this is too much");
    printf("str = \"%s\", r = %d\n", str, r);
#endif
}