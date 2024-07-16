//en.cppreference.com/w/c/string/wide/wcstok.html
#include <wchar.h>
#include <stdio.h>
 
int main(void)
{
    wchar_t input[] = L"A bird came down the walk";
    printf("Parsing the input string '%ls'\n", input);
    wchar_t *buffer;
    wchar_t *token = wcstok(input, L" ", &buffer);
    while(token) {
        printf("%ls\n", token);
        token = wcstok(NULL, L" ", &buffer);
    }
 
    printf("Contents of the input string now: '");
    for(size_t n = 0; n < sizeof input / sizeof *input; ++n)
        input[n] ? printf("%lc", input[n]) : printf("\\0");
    puts("'");
}