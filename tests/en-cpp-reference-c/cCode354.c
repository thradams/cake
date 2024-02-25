//en.cppreference.com/w/c/string/multibyte/c16rtomb.html
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <uchar.h>
 
mbstate_t state;
 
int main(void)
{
    setlocale(LC_ALL, "en_US.utf8");
    const char16_t in[] = u"zß水🍌"; // or "z\u00df\u6c34\U0001F34C"
    const size_t in_sz = sizeof in / sizeof *in;
 
    printf("Processing %zu UTF-16 code units: [ ", in_sz);
    for (size_t n = 0; n < in_sz; ++n)
        printf("%#x ", in[n]);
    puts("]");
 
    char out[MB_CUR_MAX * in_sz];
    char *p = out;
    for (size_t n = 0; n < in_sz; ++n)
    {
        size_t rc = c16rtomb(p, in[n], &state);
        if (rc == (size_t)-1)
            break;
        p += rc;
    }
 
    size_t out_sz = p - out;
    printf("into %zu UTF-8 code units: [ ", out_sz);
    for (size_t x = 0; x < out_sz; ++x)
        printf("%#x ", +(unsigned char)out[x]);
    puts("]");
}