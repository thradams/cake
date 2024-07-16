//en.cppreference.com/w/c/program/abort.html
#include <stdio.h>
#include <stdlib.h>
 
int main(void) 
{
    FILE *fp = fopen("data.txt","r");
    if (fp == NULL) {
       fprintf(stderr, "error opening file data.txt in function main()\n");
       abort();
    }
 
    /* Normal processing continues here. */
    fclose(fp);
    printf("Normal Return\n");
    return 0;
}