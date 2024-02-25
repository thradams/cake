//en.cppreference.com/w/c/string/multibyte/c32rtomb.html
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <uchar.h>
 
mbstate_t state;
 
int main(void)
{
    setlocale(LC_ALL, "en_US.utf8");
    const char32_t in[] = U"zß水🍌"; // or "z\u00df\u6c34\U0001F34C"
    size_t in_sz = sizeof in / sizeof *in;
 
    printf("Processing %zu UTF-32 code units: [ ", in_sz);
    for (size_t n = 0; n < in_sz; ++n)
        printf("%#x ", in[n]);
    puts("]");
 
    char out[MB_CUR_MAX * in_sz];
    char* p = out;
    for (size_t n = 0; n < in_sz; ++n)
    {
        size_t rc = c32rtomb(p, in[n], &state);
        if(rc == (size_t)-1) break;
        p += rc;
    }
 
    size_t out_sz = p - out;
    printf("into %zu UTF-8 code units: [ ", out_sz);
    for (size_t x = 0; x < out_sz; ++x)
        printf("%#x ", +(unsigned char)out[x]);
    puts("]");
}