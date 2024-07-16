//en.cppreference.com/w/c/language/enum.html
#include <stdio.h>
 
int main(void)
{
    enum TV { FOX = 11, CNN = 25, ESPN = 15, HBO = 22, MAX = 30, NBC = 32 };
 
    printf("List of cable stations:\n");
    printf(" FOX: \t%2d\n", FOX);
    printf(" HBO: \t%2d\n", HBO);
    printf(" MAX: \t%2d\n", MAX);
}