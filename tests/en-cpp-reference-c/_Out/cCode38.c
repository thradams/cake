//en.cppreference.com/w/c/io/fgets.html
#include <stdio.h>
#include <stdlib.h>
 
int main(void)
{
    FILE* tmpf = tmpfile();
    fputs("Alan Turing\n", tmpf);
    fputs("John von Neumann\n", tmpf);
    fputs("Alonzo Church\n", tmpf);
 
    rewind(tmpf);
 
    char buf[8];
    while (fgets(buf, sizeof buf, tmpf) != NULL)
          printf("\"%s\"\n", buf);
 
    if (feof(tmpf))
       puts("End of file reached");
}