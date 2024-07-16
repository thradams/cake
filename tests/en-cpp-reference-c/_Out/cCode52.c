//en.cppreference.com/w/c/io/fwide.html
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
 
void show_orientation(int n)
{
    n < 0 ? puts("\tnarrow orientation"):
    n > 0 ? puts("\twide orientation"):
            puts("\tno orientation");
}
 
void try_read(FILE* fp)
{
    int c = fgetc(fp);
    if(c == EOF)
        puts("\tnarrow character read failed");
    else
        printf("\tnarrow character read '%c'\n", c);
 
    wint_t wc = fgetwc(fp);
    if(wc == WEOF)
        puts("\twide character read failed");
    else
        printf("\twide character read '%lc'\n", wc);
}
 
int main(void)
{
    enum fwide_orientation { narrow = -1, query, wide };
 
    FILE* fp = fopen("main.cpp", "r");
    if (!fp)
    {
        perror("fopen() failed");
        return EXIT_FAILURE;
    }
 
    puts("1) A newly opened stream has no orientation.");
    show_orientation(fwide(fp, query));
 
    puts("2) Establish byte orientation.");
    show_orientation(fwide(fp, narrow));
    try_read(fp);
 
    puts("3) Only freopen() can reset stream orientation.");
    if (freopen("main.cpp", "r", fp) == NULL)
    {
       perror("freopen() failed");
       return EXIT_FAILURE;
    }
 
    puts("4) A reopened stream has no orientation.");
    show_orientation(fwide(fp, query));
 
    puts("5) Establish wide orientation.");
    show_orientation(fwide(fp, wide));
    try_read(fp);
 
    fclose(fp);
}