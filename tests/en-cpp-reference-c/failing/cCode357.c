//en.cppreference.com/w/c/string/multibyte/char32_t.html
#include <stdio.h>
#include <uchar.h>
 
int main(void)
{
    const char32_t wc[] = U"zß水🍌"; // or "z\u00df\u6c34\U0001f34c"
    const size_t wc_sz = sizeof wc / sizeof *wc;
    printf("%zu UTF-32 code units: [ ", wc_sz);
    for (size_t n = 0; n < wc_sz; ++n)
        printf("%#x ", wc[n]);
    printf("]\n");
}