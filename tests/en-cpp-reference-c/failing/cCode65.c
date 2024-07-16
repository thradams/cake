//en.cppreference.com/w/c/io/setvbuf.html
// Make some POSIX functions, such as `int fileno(FILE*)`, visible:
#define _POSIX_SOURCE
 
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
 
int main(void)
{
    FILE* fp = fopen("/tmp/test.txt", "w+");
    if (fp == NULL)
    {
        perror("fopen");
        return EXIT_FAILURE;
    }
 
    struct stat stats;
    if (fstat(fileno(fp), &stats) == -1) // POSIX only
    {
        perror("fstat");
        return EXIT_FAILURE;
    }
 
    printf("BUFSIZ is %d, but optimal block size is %ld\n", BUFSIZ, stats.st_blksize);
    if (setvbuf(fp, NULL, _IOFBF, stats.st_blksize) != 0)
    {
        perror("setvbuf failed"); // POSIX version sets errno
        return EXIT_FAILURE;
    }
 
    int ch;
    while((ch=fgetc(fp)) != EOF); // read entire file: use truss/strace to
                                  // observe the read(2) syscalls used
 
    fclose(fp);
    return EXIT_SUCCESS;
}