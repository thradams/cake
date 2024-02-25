//en.cppreference.com/w/c/string/byte/strstr.html
#include <string.h>
#include <stdio.h>
 
void find_str(char const *str, char const *substr)
{
    char *pos = strstr(str, substr);
    pos ? printf("found the string '%s' in '%s' at position %td\n",
                 substr, str, pos - str)
        : printf("the string '%s' was not found in '%s'\n",
                 substr, str);
}
 
int main(void)
{
    char *str = "one two three";
    find_str(str, "two");
    find_str(str, "");
    find_str(str, "nine");
    find_str(str, "n");
 
    return 0;
}