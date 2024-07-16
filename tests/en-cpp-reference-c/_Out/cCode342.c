//en.cppreference.com/w/c/string/byte/strrchr.html
#include <string.h>
#include <stdio.h>
 
int main(void)
{
    char szSomeFileName[] = "foo/bar/foobar.txt";
    char *pLastSlash = strrchr(szSomeFileName, '/');
    char *pszBaseName = pLastSlash ? pLastSlash + 1 : szSomeFileName;
    printf("Base Name: %s", pszBaseName);
}