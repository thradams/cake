//en.cppreference.com/w/c/io/fseek.html
#include <stdio.h>
#include <stdlib.h>
 
int main(void)
{
    /* Prepare an array of double values. */
    #define SIZE 5
    double A[SIZE] = {1.0, 2.0, 3.0, 4.0, 5.0};
    /* Write array to a file. */
    FILE * fp = fopen("test.bin", "wb");
    fwrite(A, sizeof(double), SIZE, fp);
    fclose (fp);
 
    /* Read the double values into array B. */
    double B[SIZE];
    fp = fopen("test.bin", "rb");
 
    /* Set the file position indicator in front of third double value. */
    if (fseek(fp, sizeof(double) * 2L, SEEK_SET) != 0)
    {
        fprintf(stderr, "fseek() failed in file %s at line # %d\n", __FILE__, __LINE__ - 2);
        fclose(fp);
        return EXIT_FAILURE;
    }
 
    int ret_code = fread(B, sizeof(double), 1, fp); /* read one double value  */
    printf("ret_code == %d\n", ret_code);           /* print the number of values read */
    printf("B[0] == %.1f\n", B[0]);                 /* print one value */
 
    fclose(fp);
    return EXIT_SUCCESS;
}