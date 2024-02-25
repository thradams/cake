//en.cppreference.com/w/c/io/fgetpos.html
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
 
int main(void)
{
    // prepare a file holding 4 values of type double
    enum {SIZE = 4};
    FILE* fp = fopen("test.bin", "wb");
    assert(fp);
    int rc = fwrite((double[SIZE]){1.1, 2.2, 3.3, 4.4}, sizeof(double), SIZE, fp);
    assert(rc == SIZE);
    fclose(fp);
 
    // demo using fsetpos to return to the beginning of a file
    fp = fopen("test.bin", "rb");
    fpos_t pos;
    fgetpos(fp, &pos);               // store start of file in pos
    double d;
    rc = fread(&d, sizeof d, 1, fp); // read the first double
    assert(rc == 1);
    printf("First value in the file: %.1f\n", d);
    fsetpos(fp,&pos);                 // move file position back to the start of the file
    rc = fread(&d, sizeof d, 1, fp);  // read the first double again
    assert(rc == 1);
    printf("First value in the file again: %.1f\n", d);
    fclose(fp);
 
    // demo error handling
    rc = fsetpos(stdin, &pos);
    if(rc) perror("could not fsetpos stdin");
}