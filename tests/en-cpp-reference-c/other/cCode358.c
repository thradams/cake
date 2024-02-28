//en.cppreference.com/w/c/string/multibyte/char8_t.html
#include <uchar.h>
#include <stdio.h>
 
int main(void)
{
    char8_t str[] = u8"zß水🍌"; // or "z\u00df\u6c34\U0001f34c"
    size_t str_sz = sizeof str; // sizeof *str == 1 by definition
    printf("%zu UTF-8 code units: [ ", str_sz);
    for (size_t n = 0; n < str_sz; ++n)
        printf("%02X ", str[n]);
    printf("]\n");
}