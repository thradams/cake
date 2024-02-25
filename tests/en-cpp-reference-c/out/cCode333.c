//en.cppreference.com/w/c/string/byte/strcspn.html
#include <string.h>
#include <stdio.h>
 
int main(void)
{
    const char *string = "abcde312$#@";
    const char *invalid = "*$#";
 
    size_t valid_len = strcspn(string, invalid);
    if(valid_len != strlen(string))
       printf("'%s' contains invalid chars starting at position %zu\n",
               string, valid_len);
}