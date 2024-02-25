//en.cppreference.com/w/c/io/rename.html
#include <stdio.h>
#include <stdlib.h>
 
int main(void)
{
    FILE* fp = fopen("from.txt", "w"); // create file "from.txt"
    if (!fp)
    {
        perror("from.txt");
        return EXIT_FAILURE;
    }
    fputc('a', fp); // write to "from.txt"
    fclose(fp);
 
    int rc = rename("from.txt", "to.txt");
    if (rc)
    {
        perror("rename");
        return EXIT_FAILURE;
    }
 
    fp = fopen("to.txt", "r");
    if(!fp)
    {
        perror("to.txt");
        return EXIT_FAILURE;
    }
    printf("%c\n", fgetc(fp)); // read from "to.txt"
    fclose(fp);
 
    return EXIT_SUCCESS;
}