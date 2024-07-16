//en.cppreference.com/w/c/io/fread.html
#include <stdio.h>
 
enum { SIZE = 5 };
 
int main(void)
{
    const double a[SIZE] = {1.0, 2.0, 3.0, 4.0, 5.0};
    printf("Array has size %ld bytes, element size: %ld\n", sizeof a, sizeof *a);
    FILE *fp = fopen("test.bin", "wb"); // must use binary mode
    fwrite(a, sizeof *a, SIZE, fp); // writes an array of doubles
    fclose(fp);
 
    double b[SIZE];
    fp = fopen("test.bin","rb");
    const size_t ret_code = fread(b, sizeof b[0], SIZE, fp); // reads an array of doubles
    if (ret_code == SIZE)
    {
        printf("Array at %p read successfully, contents:\n", (void*)&a);
        for (int n = 0; n != SIZE; ++n)
            printf("%f ", b[n]);
        putchar('\n');
    }
    else // error handling
    {
        if (feof(fp))
            printf("Error reading test.bin: unexpected end of file\n");
        else if (ferror(fp))
            perror("Error reading test.bin");
    }
 
    fclose(fp);
}