//en.cppreference.com/w/c/error/errno_macros.html
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
 
int main(void)
{
    errno = 0;
    printf("log(-1.0) = %f\n", log(-1.0));
    printf("%s\n\n",strerror(errno));
 
    errno = 0;
    printf("log(0.0)  = %f\n", log(0.0));
    printf("%s\n",strerror(errno));
}