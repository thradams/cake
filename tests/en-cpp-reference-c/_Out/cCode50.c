//en.cppreference.com/w/c/io/fsetpos.html
#include <stdio.h>
#include <stdlib.h>
 
int main(void)
{
    /* Prepare an array of f-p values. */
    #define SIZE 5
    double A[SIZE] = {1.,2.,3.,4.,5.};
    /* Write array to a file. */
    FILE * fp = fopen("test.bin", "wb");
    fwrite(A,sizeof(double),SIZE,fp);
    fclose (fp);
 
    /* Read the f-p values into array B. */
    double B[SIZE];
    fp = fopen("test.bin","rb");
    fpos_t pos;
    if (fgetpos(fp,&pos) != 0)      /* current position: start of file */
    {
       perror("fgetpos()");
       fprintf(stderr,"fgetpos() failed in file %s at line # %d\n", __FILE__,__LINE__-3);
       exit(EXIT_FAILURE);
    }
 
    int ret_code = fread(B,sizeof(double),1,fp);   /* read one f-p value */
    /* current position: after reading one f-p value */
    printf("%.1f; read count = %d\n", B[0], ret_code);   /* print one f-p value and ret_code */
 
    if (fsetpos(fp,&pos) != 0)   /* reset current position to start of file */
    {
       if (ferror(fp))
       {
          perror("fsetpos()");
          fprintf(stderr,"fsetpos() failed in file %s at line # %d\n", __FILE__,__LINE__-5);
          exit(EXIT_FAILURE);
       }
    }
 
    ret_code = fread(B,sizeof(double),1,fp);   /* reread first f-p value */
    printf("%.1f; read count = %d\n", B[0], ret_code);   /* print one f-p value and ret_code */
    fclose(fp);
 
    return EXIT_SUCCESS; 
}