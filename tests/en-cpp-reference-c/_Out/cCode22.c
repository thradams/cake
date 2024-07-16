//en.cppreference.com/w/c/error/assert.html
#include <stdio.h>
// uncomment to disable assert()
// #define NDEBUG
#include <assert.h>
#include <math.h>
 
int main(void)
{
    double x = -1.0;
    assert(x >= 0.0);
    printf("sqrt(x) = %f\n", sqrt(x));   
 
    return 0;
}