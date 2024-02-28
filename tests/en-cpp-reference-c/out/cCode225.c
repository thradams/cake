//en.cppreference.com/w/c/numeric/math/div.html
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
 
void reverse(char* first, char* last)
{
    for (--last; first < last; ++first, --last)
    {
        char c = *last;
        *last = *first;
        *first = c;
    }
}
 
// returns empty buffer in case of buffer overflow
char* my_itoa(int n, int base, char* buf, size_t buf_size)
{
    assert(2 <= base && base <= 16 && buf && buf_size);
    div_t dv = {.quot = n};
    char* p = buf;
    do
    {
        if (!--buf_size)
            return (*buf = '\0'), buf;
        dv = div(dv.quot, base);
        *p++ = "0123456789abcdef"[abs(dv.rem)];
    }
    while(dv.quot);
    if (n < 0)
        *p++ = '-';
    *p = '\0';
    reverse(buf, p);
    return buf;
}
 
int main(void)
{
    char buf[16];
    printf("%s\n", my_itoa(0, 2, buf, sizeof buf));
    printf("%s\n", my_itoa(007, 3, buf, sizeof buf));
    printf("%s\n", my_itoa(12346, 10, buf, sizeof buf));
    printf("%s\n", my_itoa(-12346, 10, buf, sizeof buf));
    printf("%s\n", my_itoa(-42, 2, buf, sizeof buf));
    printf("%s\n", my_itoa(INT_MAX, 16, buf, sizeof buf));
    printf("%s\n", my_itoa(INT_MIN, 16, buf, sizeof buf));
}