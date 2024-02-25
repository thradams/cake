//en.cppreference.com/w/c/io/clearerr.html
#include <stdio.h>
#include <assert.h>
 
int main(void)
{
    FILE* tmpf = tmpfile();
    fputs("cppreference.com\n", tmpf);
    rewind(tmpf);
 
    for (int ch; (ch = fgetc(tmpf)) != EOF; putchar(ch)) { }
 
    assert(feof(tmpf)); // the loop is expected to terminate by EOF
    puts("End of file reached");
 
    clearerr(tmpf); // clear EOF
 
    puts(feof(tmpf) ? "EOF indicator set" 
                    : "EOF indicator cleared");
}