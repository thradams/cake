//en.cppreference.com/w/c/string/multibyte/char16_t.html
#include <stdio.h>
#include <uchar.h>
 
int main(void)
{
    const char16_t wcs[] = u"zß水🍌"; // or "z\u00df\u6c34\U0001f34c"
    const size_t wcs_sz = sizeof wcs / sizeof *wcs;
    printf("%zu UTF-16 code units: [ ", wcs_sz);
    for (size_t n = 0; n < wcs_sz; ++n)
        printf("%#x ", wcs[n]);
    printf("]\n");
}