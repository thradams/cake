//en.cppreference.com/w/c/io/tmpnam.html
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
int main(void)
{
    // Note, the compiler/linker may issue a security warning, e.g. GCC:
    // "warning: the use of `tmpnam' is dangerous, better use `mkstemp'"
    char* name1 = tmpnam(NULL);
    printf("temporary file name: %s\n", name1);
 
    char name2[L_tmpnam];
    if (tmpnam(name2))
        printf("temporary file name: %s\n", name2);
 
    // POSIX offers mkstemp. The following declaration might be
    // necessary as mkstemp is absent in the standard C <stdlib.h>.
    int mkstemp(char*);
 
    char name3[] = "/tmp/fileXXXXXX"; // at least six 'X' required ^_^
    int file_descriptor = mkstemp(name3);
    if (file_descriptor != -1)
        printf("temporary file name: %s\n", name3);
    else
        perror("mkstemp");
}