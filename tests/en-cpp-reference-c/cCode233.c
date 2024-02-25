//en.cppreference.com/w/c/numeric/math/float_t.html
#include <float.h>
#include <math.h>
#include <stdio.h>
 
int main(void)
{
    printf("%d\n", FLT_EVAL_METHOD);
    printf("%zu  %zu\n", sizeof(float),sizeof(float_t));
    printf("%zu  %zu\n", sizeof(double),sizeof(double_t));
    return 0;
}