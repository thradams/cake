//en.cppreference.com/w/c/io/remove.html
#include <stdio.h>
#include <stdlib.h>
 
int main(void)
{
    FILE* fp = fopen("file1.txt", "w"); // create file
    if (!fp)
    {
        perror("file1.txt");
        return EXIT_FAILURE;
    }
    puts("Created file1.txt");
    fclose(fp);
 
    int rc = remove("file1.txt");
    if (rc)
    {
        perror("remove");
        return EXIT_FAILURE;
    }
    puts("Removed file1.txt");
 
    fp = fopen("file1.txt", "r"); // Failure: file does not exist
    if (!fp)
        perror("Opening removed file failed");
 
    rc = remove("file1.txt"); // Failure: file does not exist
    if (rc)
        perror("Double-remove failed");
 
    return EXIT_SUCCESS;
}