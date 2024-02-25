//en.cppreference.com/w/c/io/ferror.html
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
 
int main(void)
{
    char* fname = tmpnam(NULL);
    FILE* f = fopen(fname, "wb");
    fputs("\xff\xff\n", f); // not a valid UTF-8 character sequence
    fclose(f);
 
    setlocale(LC_ALL, "en_US.utf8");
    f = fopen(fname, "rb");
    wint_t ch;
    while ((ch=fgetwc(f)) != WEOF) // attempt to read as UTF-8 fails
          printf("%#x ", ch);
 
    if (feof(f))
        puts("EOF indicator set");
    if (ferror(f))
        puts("Error indicator set");
}