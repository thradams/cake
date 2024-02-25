//en.cppreference.com/w/c/string/multibyte/mbrtoc32.html
#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <uchar.h>
#include <assert.h>
 
int main(void)
{
    setlocale(LC_ALL, "en_US.utf8");
    char in[] = u8"zß水🍌"; // or "z\u00df\u6c34\U0001F34C"
    const size_t in_size = sizeof in / sizeof *in;
 
    printf("Processing %zu UTF-8 code units: [ ", in_size);
    for (size_t i = 0; i < in_size; ++i)
        printf("0x%02x ", (unsigned char)in[i]);
 
    puts("]");
 
    char32_t out[in_size];
    char32_t *p_out = out;
    char *p_in = in, *end = in + in_size;
    mbstate_t state;
    memset(&state, 0, sizeof(state));
    size_t rc;
    while ((rc = mbrtoc32(p_out, p_in, end - p_in, &state)))
    {
        assert(rc != (size_t)-3); // no surrogate pairs in UTF-32
        if (rc == (size_t)-1) break; // invalid input
        if (rc == (size_t)-2) break; // truncated input
        p_in += rc;
        ++p_out;
    }
 
    size_t out_size = p_out+1 - out;
    printf("into %zu UTF-32 code units: [ ", out_size);
    for (size_t i = 0; i < out_size; ++i)
        printf("0x%08X ", out[i]);
 
    puts("]");
}